// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CustomType.h"

#include "SaveGame_TheLastBastion.generated.h"

/**
 * 
 */


UCLASS()
class THELASTBASTION_API USaveGame_TheLastBastion : public USaveGame
{
	GENERATED_BODY()
		
public:

	USaveGame_TheLastBastion(const FObjectInitializer& _init);

public: 

	void UseDefaultProfile();

	/** Called when Log into Lobby on SaveGameCheck, set character class to default class*/
	void LobbyInit();

	void LogOutProfile();

	static UTexture2D* GetRangerAvatarImage();
	static UTexture2D* GetBuilderAvatarImage();

	UPROPERTY()
	FPlayerProfile mPlayerProfile;


	
};
