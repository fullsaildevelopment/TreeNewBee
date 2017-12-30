// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGM.h"
#include "CustomType.h"
#include "GI_TheLastBastion.h"
#include "LobbyPC.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h "
#include "UnrealNetwork.h"



const FString GAME_MAP = TEXT("/Game/Maps/Gameplay_Dev?listen");
ALobbyGM::ALobbyGM()
{
	UCustomType::FindClass<APawn>(DefaultPawnClass, TEXT("/Game/Blueprints/Heros/LobbyPawn_Bp"));
	PlayerControllerClass = ALobbyPC::StaticClass();
	bUseSeamlessTravel = true;
}

void ALobbyGM::PostLogin(APlayerController * NewPlayer)
{

	Super::PostLogin(NewPlayer);

	if (HasAuthority())
	{
		// if this is a server
		AllPlayerControllers.Add(NewPlayer);


		// get a copy of lobby info from game instance
		UGameInstance* gi = GetGameInstance();
		if (gi == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("can get game instance  ---  ALobbyGM::PostLogin"));
			return;
		}

		UGI_TheLastBastion* game_gi = Cast<UGI_TheLastBastion>(gi);

		if (game_gi == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("game instance needs to be UGI_TheLastBastion ---  ALobbyGM::PostLogin"));
			return;
		}

		MaxNumOfPlayers = game_gi->GetMaxConnection();
		LobbyName       = game_gi->GetLobbyName();
		// UE_LOG(LogTemp, Warning, TEXT("%s"), *LobbyName.ToString());

		// Update client
		ALobbyPC* lobbyPC = Cast<ALobbyPC>(NewPlayer);
		if (lobbyPC == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("player controller needs to be lobbyPC ---  ALobbyGM::PostLogin"));
			return;
		}

		SpawnPlayer(lobbyPC);
		lobbyPC->CLIENT_OnGameModeLogIn();
		lobbyPC->CLIENT_InitLobbyUI(LobbyName);
	}
}

void ALobbyGM::UpdateAllConnectedPlayers()
{
	CurrentNumOfPlayers = AllPlayerControllers.Num();
	if (CurrentNumOfPlayers > 0)
	{
		AllConnectedPlayers.Empty();
		ALobbyPC* lobbyPC = nullptr;

	
		for (int iCtrl = 0; iCtrl < CurrentNumOfPlayers; iCtrl++)
		{
			// gather player profiles from controller
			lobbyPC = Cast<ALobbyPC>(AllPlayerControllers[iCtrl]);
			FPlayerProfile playerProfile = lobbyPC->GetPlayerProfile();
			AllConnectedPlayers.Add(playerProfile);
			// Update the player number on each clients
			lobbyPC->CLIENT_UpdateNumOfPlayer(CurrentNumOfPlayers, MaxNumOfPlayers);
		}

		// then put all connect clients' info on each clients UI
		for (int iCtrl = 0; iCtrl < CurrentNumOfPlayers; iCtrl++)
		{
			lobbyPC = Cast<ALobbyPC>(AllPlayerControllers[iCtrl]);
			lobbyPC->CLIENT_AddPlayerToPlayerList(AllConnectedPlayers);
		}
	}

}

void ALobbyGM::RemovePC(APlayerController * _pc)
{
	AllPlayerControllers.Remove(_pc);
}

void ALobbyGM::StartGame()
{
	GetWorld()->ServerTravel(GAME_MAP);
	for (int iPc = 0; iPc < AllPlayerControllers.Num(); iPc++)
	{
		Cast<ALobbyPC>(AllPlayerControllers[iPc])->CLIENT_RemoveLobbyUI();
	}
}

void ALobbyGM::SpawnPlayer(APlayerController * _pc)
{
	APawn* pawn = _pc->GetPawn();
	if (pawn)
		pawn->Destroy();
	pawn = nullptr;

	if (DefaultPawnClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultPawnClass is NULL,  ALobbyGM::SpawnPlayer"))
	}

	TArray<AActor*> playerStarts;
	 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStarts);


	if (playerStarts.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Please put a playerStart in Lobby map,  ALobbyGM::SpawnPlayer"))
		return;
	}
	pawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, playerStarts[0]->GetTransform());
	_pc->Possess(pawn);
}
