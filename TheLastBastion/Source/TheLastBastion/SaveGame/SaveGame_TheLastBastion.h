// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CustomType.h"
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "SaveGame_TheLastBastion.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API USaveGame_TheLastBastion : public USaveGame
{
	GENERATED_BODY()


	UPROPERTY()
		FPlayerProfile mPlayerProfile;


public:

	//USaveGame_TheLastBastion(const FObjectInitializer& _objectInit);

	const FPlayerProfile* GetPlayerProfile() const;


	void SetPlayerProfile(const FPlayerProfile& _in);
	//void SetPlayerName();
	
};
