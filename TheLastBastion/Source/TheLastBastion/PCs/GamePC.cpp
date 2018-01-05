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
#include "CustomType.h"


static TSubclassOf<UUserWidget> InGameHuD_WBPClass;

AGamePC::AGamePC(const FObjectInitializer & _objInit) : Super(_objInit)
{
	if (!InGameHuD_WBPClass)
	{
		UCustomType::FindClass<UUserWidget>(InGameHuD_WBPClass, TEXT("/Game/UI/In-Game/WBP_InGameHUD"));
	}
}
//void AGamePC::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(AGamePC, mGameInstanceRef);
//}

void AGamePC::CLIENT_Login_Implementation(int _index)
{
	FInputModeGameOnly inputMode;
	SetInputMode(inputMode);

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

void AGamePC::CLIENT_InitUI_Implementation(const class UHeroStatsComponent* _heroStats)
{
	if (mInGameHUD)
	{
		mInGameHUD->InitStats(_heroStats);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("mInGameHUD is NULL - AGamePC::CLIENT_InitUI_Implementation"));
	}
}

void AGamePC::CLIENT_UpdateHpOnHealthChanged_Implementation(const UPawnStatsComponent * _pawnStats)
{
	if (mInGameHUD)
		mInGameHUD->SetHpOnHealthChange(_pawnStats);
}

void AGamePC::CreateInGameHUD()
{
	mInGameHUD = Cast<UInGameHUD>(CreateWidget<UUserWidget>(this, InGameHuD_WBPClass));
	mInGameHUD->SetPlayerName(playerProfile);
	mInGameHUD->AddToViewport();
}

void AGamePC::SaveGameCheck()
{
	// Get Game Instance Ref

	UGI_TheLastBastion* game_gi = Cast<UGI_TheLastBastion>(GetGameInstance());
	if (game_gi == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("game instance needs to be UGI_TheLastBastion ---  ALobbyGM::PostLogin"));
		return;
	}

	USaveGame_TheLastBastion* saveGame = game_gi->LoadSaveGame();

	// check again if we have a save game
	if (saveGame == nullptr)
	{
		// if we dont have a save game create a new one
		saveGame = Cast<USaveGame_TheLastBastion>(UGameplayStatics::CreateSaveGameObject(USaveGame_TheLastBastion::StaticClass()));
		UGameplayStatics::SaveGameToSlot(saveGame, game_gi->GetPlayerSettingsSaveFString(), 0);
	}

	playerProfile = saveGame->mPlayerProfile;
	saveGame->LogOutProfile();
}

void AGamePC::BeginPlay()
{

}

