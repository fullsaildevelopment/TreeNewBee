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


	UPROPERTY()
	FPlayerProfile mPlayerProfile;


	
};
