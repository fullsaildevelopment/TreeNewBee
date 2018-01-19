// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SinglePlayerGM.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API ASinglePlayerGM : public AGameModeBase
{
	GENERATED_BODY()
	

public:

	ASinglePlayerGM(const FObjectInitializer & _objectInitilizer);
	
	void PostLogin(APlayerController* NewPlayer) override;

	void HandleSeamlessTravelPlayer(AController*& C) override;

protected:

	void BeginPlay() override;




};
