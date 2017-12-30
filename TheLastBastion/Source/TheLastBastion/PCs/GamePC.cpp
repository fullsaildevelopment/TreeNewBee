// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePC.h"
#include "GI_TheLastBastion.h"
#include "SaveGame/SaveGame_TheLastBastion.h"
#include "Kismet/GameplayStatics.h"

void AGamePC::CLIENT_Login_Implementation()
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
}

void AGamePC::SERVER_UploadProfileAndRequestCharacter_Implementation()
{

}

bool AGamePC::SERVER_UploadProfileAndRequestCharacter_Validate()
{
	return true;
}

void AGamePC::CLIENT_CreateInGameHUD_Implementation()
{

}

void AGamePC::SaveGameCheck()
{
	USaveGame_TheLastBastion* saveGame = mGameInstanceRef->GetSaveGame();

	// check again if we have a save game
	if (saveGame == nullptr)
	{
		// if we dont have a save game create a new one
		saveGame = Cast<USaveGame_TheLastBastion>(UGameplayStatics::CreateSaveGameObject(USaveGame_TheLastBastion::StaticClass()));
		FPlayerProfile newProfile;

		//newProfile.mPlayerName = FText::FromString(TEXT("NewPlayer"));

		//// Grab a default player Avatar Image
		//ConstructorHelpers::FObjectFinder<UTexture2D> objectFinder(TEXT("/Game/Assets/Animation/UE4_Mannequin/Textures/UE4_LOGO_CARD"));
		//if (objectFinder.Succeeded())
		//	newProfile.mAvatarImage = objectFinder.Object;
		//else
		//	UE_LOG(LogTemp, Error, TEXT("Can not find default avatar"));





		saveGame->SetPlayerProfile(newProfile);
		UGameplayStatics::SaveGameToSlot(saveGame, mGameInstanceRef->GetPlayerSettingsSaveFString(), 0);
	}

	playerProfile = *(saveGame->GetPlayerProfile());
}

