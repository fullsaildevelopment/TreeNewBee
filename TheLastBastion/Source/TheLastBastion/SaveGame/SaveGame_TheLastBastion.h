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

	USaveGame_TheLastBastion(const FObjectInitializer& _objInit);
		
	FPlayerProfile mPlayerProfile;


public:

	const FPlayerProfile* GetPlayerProfile() const;


	void SetPlayerProfile(const FPlayerProfile& _in);
	//void SetPlayerName();
	
};
