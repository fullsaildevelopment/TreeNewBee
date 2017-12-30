// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GamePlayGM.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API AGamePlayGM : public AGameModeBase
{
	GENERATED_BODY()
	
	
public:

	AGamePlayGM(const FObjectInitializer & _objectInitilizer);

	/** Called when a server or a client enter this level*/
	void PostLogin(APlayerController* NewPlayer) override;

private:

	UPROPERTY()
		TArray<class AGamePC*> AllPlayerControllers;

	UPROPERTY()
		TArray<struct FPlayerProfile> AllConnectedPlayers;

	UPROPERTY()
		int NumOfPlayers;


public:

	UFUNCTION()
		/** Called after a new client logged in, and ask server to update his info to all connected clients*/
		void SpawnPlayer(APlayerController* _pc);

	FORCEINLINE TArray<class AGamePC*> GetAllPlayerControllers() const { return AllPlayerControllers; }
};
