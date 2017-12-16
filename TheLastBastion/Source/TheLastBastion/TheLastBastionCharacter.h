// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TheLastBastionCharacter.generated.h"



UCLASS(config=Game)
class ATheLastBastionCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay();

public:
	ATheLastBastionCharacter();

protected:


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		class UPawnStatsComponent* PawnStats;

#pragma region Hero Movement Stats

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float CapHalfSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float CapRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float SprintSpeed = 850.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float JogSpeed = 595.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float walkSpeed = 255.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float minTurnRate = 180.0f;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float maxTurnRate = 630.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float maxTurnRate_Travel = 630.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float maxTurnRate_Combat = 1440.0f;

#pragma endregion


private:



public:

	FORCEINLINE void  SetMaxTurnRate(float _val) { maxTurnRate = _val; }
	FORCEINLINE float GetMaxTurnRateForTravel() const { return  maxTurnRate_Travel; }
	FORCEINLINE float GetMaxTurnRateForCombat() const { return  maxTurnRate_Combat; }

	FORCEINLINE float GetMinTurnRate() const { return minTurnRate; }
	FORCEINLINE float GetMaxTurnRate() const { return maxTurnRate; }


	FORCEINLINE float GetJogSpeed() const { return JogSpeed; }
	FORCEINLINE float GetSprintSpeed() const { return SprintSpeed; }

	FORCEINLINE class UPawnStatsComponent*  GetPawnStatsComp() const { return PawnStats; }

};

