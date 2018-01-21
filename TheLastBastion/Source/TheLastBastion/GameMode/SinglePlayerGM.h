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


protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CharacterClass)
		TSubclassOf<class ATheLastBastionCharacter> LannesterTrooper_T0_BP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CharacterClass)
		TSubclassOf<class ATheLastBastionCharacter> LannesterShooter_T0_BP;


private:

	// resource

	//
	UPROPERTY()
		TArray<class ASpawnLocation*> LannesterSpawnLocations_One;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		int EnemyAmount;

	
private:

	void GetAllCharacterClass();

public:

	void UpdateEnemyAmount(int _val);

};
