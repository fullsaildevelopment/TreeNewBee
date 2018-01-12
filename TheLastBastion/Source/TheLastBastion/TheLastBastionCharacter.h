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
	LanTrooper_T0 = 3 	  UMETA(DisplayName = "Lannester Trooper New Recruit")
             
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

	UPROPERTY()
	    class UPawnStatsComponent* PawnStats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterType)
		ECharacterType CharacterType;

#pragma region Movement Stats

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		bool bIsWalking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
		float CapHalfSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
		float CapRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
		float SprintSpeed = 850.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
		float JogSpeed = 595.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
		float walkSpeed = 255.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
		float minTurnRate_Travel = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
		float maxTurnRate_Travel = 630.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
		float minTurnRate_Combat = 360.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
		float maxTurnRate_Combat = 1440.0f;

#pragma endregion

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	    bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsGodMode;


protected:
	/** Config character based on character type during beginplay*/
	void CharacterCustomInit();

	UFUNCTION()
		virtual void OnHealthChangedHandle(const class UPawnStatsComponent * _pawnStatsComp, float _damage, const class UDamageType * _damageType, FName _boneNmame, const FVector& _shotFromDirection, const FVector& _hitLocation);

public:

	FORCEINLINE float GetMaxTurnRateForTravel() const { return  maxTurnRate_Travel; }
	FORCEINLINE float GetMaxTurnRateForCombat() const { return  maxTurnRate_Combat; }

	FORCEINLINE float GetMinTurnRateForTravel() const { return  minTurnRate_Travel; }
	FORCEINLINE float GetMinTurnRateForCombat() const { return  minTurnRate_Combat; }

	FORCEINLINE float GetWalkSpeed() const { return walkSpeed; }
	FORCEINLINE float GetJogSpeed() const { return JogSpeed; }
	FORCEINLINE float GetSprintSpeed() const { return SprintSpeed; }

	FORCEINLINE ECharacterType GetCharacterType() const { return CharacterType; }
	FORCEINLINE class UPawnStatsComponent*  GetPawnStatsComp() const { return PawnStats; }
	FORCEINLINE void SetCharacterType(ECharacterType _val) { CharacterType = _val; }
	FORCEINLINE bool GetIsDead() const { return bIsDead; }

	FORCEINLINE bool GetIsGodMode() const { return bIsGodMode; }


};

