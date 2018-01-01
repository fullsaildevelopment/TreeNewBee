// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePC.h"
#include "Engine.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "GI_TheLastBastion.h"
#include "GameMode/GamePlayGM.h"
#include "SaveGame/SaveGame_TheLastBastion.h"
#include "Kismet/GameplayStatics.h"
#include "UI/InGameHUD.h"


//AGamePC::AGamePC(const FObjectInitializer & _objInit) : Super(_objInit)
//{
//	bReplicates = true;
//}
//void AGamePC::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(AGamePC, mGameInstanceRef);
//}
void AGamePC::CLIENT_Login_Implementation(int _index)
{
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

	SaveGameCheck();

	SERVER_UploadProfileAndRequestCharacter(playerProfile, _index);

	CreateInGameHUD();
}

void AGamePC::SERVER_UploadProfileAndRequestCharacter_Implementation
(const FPlayerProfile& _profile, int _index)
{
	UWorld* world = GetWorld();
	AGamePlayGM* gamePlayGM = Cast<AGamePlayGM>(UGameplayStatics::GetGameMode(world));

	gamePlayGM->GrabProfileAndSpawnPlayer(_profile, _index);
}

bool AGamePC::SERVER_UploadProfileAndRequestCharacter_Validate
(const FPlayerProfile& _profile, int _index)
{
	return true;
}

void AGamePC::CLIENT_AddPlayerToPlayerList_Implementation(const TArray<FPlayerProfile>& _allConnectedPlayers
	, int _index)
{
	//mInGameHUD->AddPlayerToPlayerList(_allConnectedPlayers, _allControllers);
}

void AGamePC::CreateInGameHUD()
{
	mInGameHUD = Cast<UInGameHUD>(CreateWidget<UUserWidget>(this, mGameInstanceRef->GetInGameHUD_Class()));
	mInGameHUD -> AddToViewport();
	mInGameHUD->InitPlayerRow(playerProfile);
}

void AGamePC::SaveGameCheck()
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
}

