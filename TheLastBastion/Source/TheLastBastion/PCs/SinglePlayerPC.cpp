// Fill out your copyright notice in the Description page of Project Settings.

#include "SinglePlayerPC.h"
#include "GI_TheLastBastion.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/SaveGame_TheLastBastion.h"
#include "UI/InGameHUD.h"
#include "CustomType.h"



ASinglePlayerPC::ASinglePlayerPC(const FObjectInitializer & _objInit) : Super(_objInit)
{
	if (!InGameHuD_WBPClass)
		UCustomType::FindClass<UUserWidget>(InGameHuD_WBPClass, TEXT("/Game/UI/In-Game/WBP_InGameHUD"));
}

void ASinglePlayerPC::OnPostLogin()
{
	FInputModeGameOnly inputMode;
	SetInputMode(inputMode);
	SaveGameCheck();
	CreateInGameHUD();
}

void ASinglePlayerPC::OnFinishSeamlessTravel()
{
	OnPostLogin();
}

void ASinglePlayerPC::InitUIOnBeginPlay(const UHeroStatsComponent * _heroStats)
{
	if (mInGameHUD)
		mInGameHUD->InitStats(_heroStats);
	else
	{
		UE_LOG(LogTemp, Error, TEXT("mInGameHUD is NULL - AGamePC::CLIENT_InitUI_Implementation"));
	}
}

void ASinglePlayerPC::OnHealthChange(const UPawnStatsComponent * _heroStats)
{
	if (mInGameHUD)
		mInGameHUD->SetHpOnHealthChange(_heroStats);
}

void ASinglePlayerPC::SaveGameCheck()
{
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
		saveGame->UseDefaultProfile();
		UGameplayStatics::SaveGameToSlot(saveGame, game_gi->GetPlayerSettingsSaveFString(), 0);
	}

	playerProfile = saveGame->mPlayerProfile;
	saveGame->LogOutProfile();
}

void ASinglePlayerPC::CreateInGameHUD()
{
	mInGameHUD = Cast<UInGameHUD>(CreateWidget<UUserWidget>(this, InGameHuD_WBPClass));

	playerProfile.mCharacterImage = USaveGame_TheLastBastion::GetRangerAvatarImage();
	playerProfile.bIsRangerClass = true;

	mInGameHUD->LoadPlayerProfile_Sp(playerProfile);
	mInGameHUD->AddToViewport();
}
