// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGM.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API ALobbyGM : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ALobbyGM();
	/** Called when a server or a client enter this level*/
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
private:

	UPROPERTY()
	    TArray<class APlayerController*> AllPlayerControllers;

	UPROPERTY()
		TArray<struct FPlayerProfile>    AllConnectedPlayers;

	UPROPERTY()
		int MaxNumOfPlayers;

	UPROPERTY()
		int CurrentNumOfPlayers;

	UPROPERTY()
		FText LobbyName;

public:

	/** Called after a new client logged in, and ask server
	to update his info to all connected clients*/
	    void UpdateAllConnectedPlayers();

		void RemovePC(class APlayerController* _pc);

		void StartGame();

	UFUNCTION()
		/** Called after a new client logged in, and ask server to update his info to all connected clients*/
		void SpawnPlayer(APlayerController* _pc);


	FORCEINLINE FText GetLobbyName() const { return LobbyName; }
	FORCEINLINE int GetMaxNumOfPlayers() const { return MaxNumOfPlayers; }
	FORCEINLINE int GetCurrentNumOfPlayers() const {return CurrentNumOfPlayers; }
	FORCEINLINE TArray<class APlayerController*> GetAllPlayerControllers() const {return AllPlayerControllers;}
};
