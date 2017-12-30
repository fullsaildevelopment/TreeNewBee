// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveGame_TheLastBastion.h"
#include "GameFramework/Character.h"



USaveGame_TheLastBastion::USaveGame_TheLastBastion(const FObjectInitializer& _objectInit) : Super(_objectInit)
{

		// Set the default player class
		UCustomType::FindClass<ACharacter>(mPlayerProfile.mCharacterClass, TEXT("/Game/Blueprints/Heros/Ranger_Bp"));
	
		// Set the default character Image
		ConstructorHelpers::FObjectFinder<UTexture2D> CharacterImage(TEXT("/Game/Assets/Image/ThumbNail_Ranger"));
		if (CharacterImage.Succeeded())
			mPlayerProfile.mCharacterImage = CharacterImage.Object;
		else
			UE_LOG(LogTemp, Error, TEXT("Can not find default avatar"));
	
		// Set the default name
		mPlayerProfile.mPlayerName = FText::FromString(TEXT("NewPlayer"));
	
		// Set the default player avatar image
		ConstructorHelpers::FObjectFinder<UTexture2D> AvatarImage(TEXT("/Game/Assets/Animation/UE4_Mannequin/Textures/UE4_LOGO_CARD"));
		if (AvatarImage.Succeeded())
			mPlayerProfile.mAvatarImage = AvatarImage.Object;
		else
			UE_LOG(LogTemp, Error, TEXT("Can not find default avatar"));
}




const FPlayerProfile * USaveGame_TheLastBastion::GetPlayerProfile() const
{
	return &mPlayerProfile;
}

void USaveGame_TheLastBastion::SetPlayerProfile(const FPlayerProfile& _in)
{
	mPlayerProfile = _in;
}
