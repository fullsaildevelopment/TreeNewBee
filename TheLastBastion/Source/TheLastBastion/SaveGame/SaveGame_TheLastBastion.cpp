// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveGame_TheLastBastion.h"
#include "CustomType.h"


static FText defaultPlayerName = FText::FromString(TEXT("New Player"));
static TSubclassOf<ACharacter> defaultCharacterClass = nullptr;
static UTexture2D* defaultCharacterImage = nullptr;
static UTexture2D* defaultAvatarImage = nullptr;


USaveGame_TheLastBastion::USaveGame_TheLastBastion(const FObjectInitializer& _init) :Super(_init)
{

	if (defaultAvatarImage == nullptr)
	{

	}

	if (defaultCharacterImage == nullptr)
	{

	}

	if (defaultCharacterClass = nullptr)
	{

	}



	UCustomType::FindClass<ACharacter>(mPlayerProfile.mCharacterClass, TEXT("/Game/Blueprints/Heros/Ranger_Bp"));

	// Set the default character Image
	ConstructorHelpers::FObjectFinder<UTexture2D> CharacterImage(TEXT("/Game/Assets/Image/ThumbNail_Ranger"));
	if (CharacterImage.Succeeded())
		mPlayerProfile.mCharacterImage = CharacterImage.Object;
	else
		UE_LOG(LogTemp, Error, TEXT("Can not find default avatar"));

	// Set the default player avatar image
	ConstructorHelpers::FObjectFinder<UTexture2D> AvatarImage(TEXT("/Game/Assets/Animation/UE4_Mannequin/Textures/UE4_LOGO_CARD"));
	if (AvatarImage.Succeeded())
		mPlayerProfile.mAvatarImage = AvatarImage.Object;
	else
		UE_LOG(LogTemp, Error, TEXT("Can not find default avatar"));
}

void USaveGame_TheLastBastion::UseNewProfile()
{
	mPlayerProfile.mPlayerName = defaultPlayerName;
}

void USaveGame_TheLastBastion::LogOutProfile()
{
	UE_LOG(LogTemp, Warning, TEXT("USaveGame_TheLastBastion::LogOutProfile"));
	UE_LOG(LogTemp, Warning, TEXT("Player Name: %s"), *mPlayerProfile.mPlayerName.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Character Class Name: %s"), *mPlayerProfile.mCharacterClass->GetName());

}
