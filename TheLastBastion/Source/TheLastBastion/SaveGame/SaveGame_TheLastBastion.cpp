// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveGame_TheLastBastion.h"
#include "CustomType.h"


static FText defaultPlayerName = FText::FromString(TEXT("New Player"));
static TSubclassOf<ACharacter> defaultCharacterClass = nullptr;
static UTexture2D* defaultCharacterImage = nullptr;
static UTexture2D* defaultAvatarImage = nullptr;
static UTexture2D* builderCharacterImage = nullptr;


USaveGame_TheLastBastion::USaveGame_TheLastBastion(const FObjectInitializer& _init) :Super(_init)
{

	if (defaultAvatarImage == nullptr)
	{
		// Set the default player avatar image
		ConstructorHelpers::FObjectFinder<UTexture2D> AvatarImage(TEXT("/Game/Assets/Animation/UE4_Mannequin/Textures/UE4_LOGO_CARD"));
		if (AvatarImage.Succeeded())
			defaultAvatarImage = AvatarImage.Object;
		else
			UE_LOG(LogTemp, Error, TEXT("Can not find default avatar"));
	}

	if (defaultCharacterImage == nullptr)
	{
		// Set the default character Image
		ConstructorHelpers::FObjectFinder<UTexture2D> CharacterImage(TEXT("/Game/Assets/Image/ThumbNail_Ranger"));
		if (CharacterImage.Succeeded())
			defaultCharacterImage = CharacterImage.Object;
		else
			UE_LOG(LogTemp, Error, TEXT("Can not find defaultCharacterImage"));
	}

	if (builderCharacterImage == nullptr)
	{
		// Set the default character Image
		ConstructorHelpers::FObjectFinder<UTexture2D> builderImage(TEXT("/Game/Assets/Image/ThumbNail_Builder"));
		if (builderImage.Succeeded())
			builderCharacterImage = builderImage.Object;
		else
			UE_LOG(LogTemp, Error, TEXT("Can not find builderCharacterImage"));
	}

	if (defaultCharacterClass == nullptr)
	{
		UCustomType::FindClass<ACharacter>(defaultCharacterClass, TEXT("/Game/Blueprints/Heros/Ranger_Bp"));
	}

}

void USaveGame_TheLastBastion::UseDefaultProfile()
{
	mPlayerProfile.mPlayerName = defaultPlayerName;
	mPlayerProfile.mAvatarImage = defaultAvatarImage;
	mPlayerProfile.mCharacterClass = defaultCharacterClass;
	mPlayerProfile.mCharacterImage = defaultCharacterImage;
	mPlayerProfile.bIsRangerClass = true;
}

void USaveGame_TheLastBastion::LobbyInit()
{
	mPlayerProfile.mCharacterClass = defaultCharacterClass;
	mPlayerProfile.mCharacterImage = defaultCharacterImage;
	mPlayerProfile.bIsRangerClass = true;
}

void USaveGame_TheLastBastion::LogOutProfile()
{
	UE_LOG(LogTemp, Warning, TEXT("USaveGame_TheLastBastion::LogOutProfile"));
	UE_LOG(LogTemp, Warning, TEXT("Player Name: %s"), *mPlayerProfile.mPlayerName.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Character Class Name: %s"), *mPlayerProfile.mCharacterClass->GetName());

}

UTexture2D * USaveGame_TheLastBastion::GetRangerAvatarImage()
{
	return defaultCharacterImage;
}

UTexture2D * USaveGame_TheLastBastion::GetBuilderAvatarImage()
{
	return builderCharacterImage;
}
