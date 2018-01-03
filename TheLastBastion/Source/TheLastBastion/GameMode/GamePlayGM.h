// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GamePlayGM.generated.h"

/**
*
*/



USTRUCT()
struct FMatchPlayer
{
	GENERATED_BODY()

public:

	UPROPERTY()
		class AGamePC* controller;
	UPROPERTY()
		class ATheLastBastionHeroCharacter* character;
	UPROPERTY()
		struct FPlayerProfile profile;
	UPROPERTY()
		int index = -1;
};


UCLASS()
class THELASTBASTION_API AGamePlayGM : public AGameModeBase
{
	GENERATED_BODY()


public:

	AGamePlayGM(const FObjectInitializer & _objectInitilizer);

	/** Called when a server or a client enter this level*/
	void PostLogin(APlayerController* NewPlayer) override;

protected:

	void BeginPlay() override;

private:

	UPROPERTY()
		TArray<FMatchPlayer> AllPlayers;

	UPROPERTY()
		int NumOfPlayers;


public:

	UFUNCTION()
		void GrabProfileAndSpawnPlayer(const FPlayerProfile& _profile, int _index);


	UFUNCTION()
		void UpdateAllConnectedPlayers(int _index);

public:

	FORCEINLINE const TArray<FMatchPlayer>& GetAllPlayers() const { return AllPlayers; }
};
