// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TheLastBastionCharacter.generated.h"



UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	None = 0              UMETA(DisplayName = "None"),
	Ranger = 1            UMETA(DisplayName = "Ranger"),
	Builder = 2           UMETA(DisplayName = "Builder"),
	SouthArcher = 3 	  UMETA(DisplayName = "SouthArcher")
               
};


UCLASS(config=Game)
class ATheLastBastionCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay();

public:
	ATheLastBastionCharacter();

protected:


		const class UPawnStatsComponent* PawnStats;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		ECharacterType CharacterType;

#pragma region Movement Stats

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
		float minTurnRate_Travel = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float maxTurnRate_Travel = 630.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float minTurnRate_Combat = 360.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float maxTurnRate_Combat = 1440.0f;

#pragma endregion

public:

	FORCEINLINE float GetMaxTurnRateForTravel() const { return  maxTurnRate_Travel; }
	FORCEINLINE float GetMaxTurnRateForCombat() const { return  maxTurnRate_Combat; }

	FORCEINLINE float GetMinTurnRateForTravel() const { return  minTurnRate_Travel; }
	FORCEINLINE float GetMinTurnRateForCombat() const { return  minTurnRate_Combat; }



	FORCEINLINE float GetJogSpeed() const { return JogSpeed; }
	FORCEINLINE float GetSprintSpeed() const { return SprintSpeed; }

	FORCEINLINE ECharacterType GetCharacterType() const { return CharacterType; }
	FORCEINLINE const class UPawnStatsComponent*  GetPawnStatsComp() const { return PawnStats; }

};

