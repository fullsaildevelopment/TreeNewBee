// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyPC.h"
#include "GameMode/LobbyGM.h"
#include "Engine.h"
#include "GI_TheLastBastion.h"
#include "UnrealNetwork.h"
#include "SaveGame/SaveGame_TheLastBastion.h"
#include "CustomType.h"
#include "Kismet/GameplayStatics.h"
#include "UI/LobbyMenu.h"
#include "Engine/World.h"

ALobbyPC::ALobbyPC(const FObjectInitializer& _objectInitializer) : Super(_objectInitializer)
{
	mAmReady = false;
}

void ALobbyPC::CLIENT_OnGameModeLogIn_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnGameModeLogIn is called by a client"));

	if (HasAuthority())
		mAmReady = true;

	// Get Game Instance Ref
	UGameInstance* gi = GetGameInstance();
	if (gi == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("can get game instance  ---  ALobbyGM::PostLogin"));
		return;
	}

	mGameInstanceRef = Cast<UGI_TheLastBastion>(gi);

	if (mGameInstanceRef == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("game instance needs to be UGI_TheLastBastion ---  ALobbyGM::PostLogin"));
		return;
	}

	// get player profile
	SaveGameCheck();

	// Setup Lobby Menu;
	CreateLobbyUI();


	SERVER_UpdateAllConnectedPlayer(playerProfile);
}

/** Call on client */
void ALobbyPC::CLIENT_UpdateNumOfPlayer_Implementation(int _currentNumOfPlayers, int _maxNumOfPlayers)
{
	mLobbyMenuRef->SetCurrentPlayersNumber(_currentNumOfPlayers, _maxNumOfPlayers);
}

void ALobbyPC::CLIENT_AddPlayerToPlayerList_Implementation(const TArray<FPlayerProfile>& _connectedPlayers)
{
	mLobbyMenuRef->AddPlayerToPlayerList(_connectedPlayers);
}

void ALobbyPC::CLIENT_InitLobbyUI_Implementation(const FText& _lobbyName)
{
	mLobbyMenuRef->SetLobbyName(_lobbyName);
}

void ALobbyPC::SERVER_UpdateAllConnectedPlayer_Implementation(const FPlayerProfile & _playerProfile)
{
	UWorld* world = GetWorld();
	ALobbyGM* lobbyGM = Cast<ALobbyGM>(UGameplayStatics::GetGameMode(world));
	// Update the server version of playerProfile of this controller
	playerProfile = _playerProfile;
	lobbyGM->UpdateAllConnectedPlayers();
}

bool ALobbyPC::SERVER_UpdateAllConnectedPlayer_Validate(const FPlayerProfile & _playerProfile)
{
	return true;
}

void ALobbyPC::SERVER_OnClientReadyButtonClicked_Implementation()
{
	UWorld* world = GetWorld();
	ALobbyGM* lobbyGM = Cast<ALobbyGM>(UGameplayStatics::GetGameMode(world));

	// Toggle the player status
	if (mAmReady)
	{
		mAmReady = false;
		playerProfile.mPlayerStatus = FText::FromString(TEXT("Not Ready"));
	}
	else
	{
		mAmReady = true;
		playerProfile.mPlayerStatus = FText::FromString(TEXT("Ready"));
	}

	// check if all the players is ready? since we are on server, we can access game mode		
	bool allPlayersAreReady = true;
	ALobbyPC* lobbyPC = nullptr;
	TArray<APlayerController*> allPC = lobbyGM->GetAllPlayerControllers();
	for (int i = 0; i < allPC.Num(); i++)
	{
		lobbyPC = Cast<ALobbyPC>(allPC[i]);
		if (!lobbyPC->GetAmReady())
		{
			allPlayersAreReady = false;
			break;
		}
	}
	// Update all clients about this change
	lobbyGM->UpdateAllConnectedPlayers();
	if (allPC.Num() == lobbyGM->GetMaxNumOfPlayers())
	{
		// Enable Start Match Button if all players are ready and has enough players
		lobbyPC = Cast<ALobbyPC>(allPC[0]);
		lobbyPC->mLobbyMenuRef->SetStartMatchButtonEnabled(allPlayersAreReady);
	}
}

bool ALobbyPC::SERVER_OnClientReadyButtonClicked_Validate()
{
	return true;
}


//void ALobbyPC::Client_SetStartMatchButtonIsEnabled_Implementation(bool _val)
//{
//	mLobbyMenuRef->SetStartMatchButtonEnabled(_val);
//
//}

void ALobbyPC::SERVER_LeaveLobbyAndUpdateConnectedPlayer_Implementation()
{
	UWorld* world = GetWorld();
	ALobbyGM* lobbyGM = Cast<ALobbyGM>(UGameplayStatics::GetGameMode(world));
	// Remove this control from all controllers
	lobbyGM->RemovePC(this);

	// call update
	lobbyGM->UpdateAllConnectedPlayers();
}

bool ALobbyPC::SERVER_LeaveLobbyAndUpdateConnectedPlayer_Validate()
{
	return true;
}

void ALobbyPC::SaveGameCheck()
{

	USaveGame_TheLastBastion* saveGame = mGameInstanceRef->LoadSaveGame();

	// check again if we have a save game
	if (saveGame == nullptr)
	{
		// if we dont have a save game create a new one
		saveGame = Cast<USaveGame_TheLastBastion>(UGameplayStatics::CreateSaveGameObject(USaveGame_TheLastBastion::StaticClass()));
		UGameplayStatics::SaveGameToSlot(saveGame, mGameInstanceRef->GetPlayerSettingsSaveFString(), 0);
	}

	playerProfile = saveGame->mPlayerProfile;

	if (HasAuthority())
		playerProfile.mPlayerStatus = FText::FromString(TEXT("Host"));
	else
		playerProfile.mPlayerStatus = FText::FromString(TEXT("Not Ready"));

}

void ALobbyPC::CreateLobbyUI()
{
	mLobbyMenuRef = Cast<ULobbyMenu>(CreateWidget<UUserWidget>(this, mGameInstanceRef->GetLobbyMenu_Class()));

	bShowMouseCursor = true;

	mLobbyMenuRef->AddToViewport();

	FInputModeGameAndUI inputMode;
	inputMode.SetHideCursorDuringCapture(true);
	inputMode.SetWidgetToFocus(mLobbyMenuRef->TakeWidget());
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(inputMode);
}

void ALobbyPC::CLIENT_OnQuitLobby_Implementation()
{
	UGI_TheLastBastion* gi = Cast<UGI_TheLastBastion>(UGameplayStatics::GetGameInstance(this));
	gi->BackToMainMenu();
}

void ALobbyPC::CLIENT_CleanAndSave_Implementation()
{
	mLobbyMenuRef->RemoveFromParent();
	USaveGame_TheLastBastion* saveGame = mGameInstanceRef->LoadSaveGame();

	// check again if we have a save game
	if (saveGame == nullptr)
	{
		// if we dont have a save game create a new one
		saveGame = Cast<USaveGame_TheLastBastion>(UGameplayStatics::CreateSaveGameObject(USaveGame_TheLastBastion::StaticClass()));
		UGameplayStatics::SaveGameToSlot(saveGame, mGameInstanceRef->GetPlayerSettingsSaveFString(), 0);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Client profile is saved"));
	}

}
