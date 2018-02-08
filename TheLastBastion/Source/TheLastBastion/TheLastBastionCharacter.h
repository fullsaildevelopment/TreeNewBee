// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TheLastBastionCharacter.generated.h"

#define FontSize_Regular 22
#define FontSize_Critical 26




UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	None = 0              UMETA(DisplayName = "None"),
	Ranger = 1            UMETA(DisplayName = "Ranger"),
	Builder = 2           UMETA(DisplayName = "Builder"),
	LanTrooper_T0 = 3 	  UMETA(DisplayName = "Lannester Trooper New Recruit"),
	LanCB_T0 = 4          UMETA(DisplayName = "Lannester Shooter New Recruit")
             
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


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GroupBehavior)
		TArray<class AAIGroupBase*> ThreatingGroup;

	UPROPERTY()
	    class UPawnStatsComponent* PawnStats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterType)
		ECharacterType CharacterType;

	UPROPERTY()
		/** Timer Handle after death*/
		FTimerHandle mKillTimer;

#pragma region Movement Stats

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		bool bIsWalking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (BlueprintProtected))
		float SprintSpeed = 850.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (BlueprintProtected))
		float JogSpeed = 595.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (BlueprintProtected))
		float walkSpeed = 255.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (BlueprintProtected))
		float minTurnRate_Travel = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (BlueprintProtected))
		float maxTurnRate_Travel = 630.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (BlueprintProtected))
		float minTurnRate_Combat = 360.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (BlueprintProtected))
		float maxTurnRate_Combat = 1440.0f;

#pragma endregion

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BlueprintProtected))
	    bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected))
		bool bIsGodMode;

protected:
	/** Config character based on character type during beginplay*/
	void CharacterCustomInit();


	UFUNCTION()
		virtual void OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType
			, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
		virtual void OnTakePointDamageHandle(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
			class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
			const class UDamageType* DamageType, AActor* DamageCauser);

	/** Unregister all the group that I am a threat to it
	* put character to ragdoll, Called on Hp = 0 */
	virtual void OnDead();

	// Called on actor destroyed
	virtual void Kill();


public:
	
	// Mark a group that under my threat
	void RegisterThreat(AAIGroupBase* _threatingGroup);

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
	FORCEINLINE const class UPawnStatsComponent*  GetPawnStatsComp_Const() const { return PawnStats; }
	FORCEINLINE void SetCharacterType(ECharacterType _val) { CharacterType = _val; }
	FORCEINLINE bool GetIsDead() const { return bIsDead; }
	FORCEINLINE bool GetIsGodMode() const { return bIsGodMode; }

	float GetCurrentMaxSpeed() const;
};

