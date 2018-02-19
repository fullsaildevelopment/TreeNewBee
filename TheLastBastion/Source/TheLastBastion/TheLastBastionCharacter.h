// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TheLastBastionCharacter.generated.h"

#define FontSize_Regular 22
#define FontSize_Critical 26
#define RagDoll_RecoverLerpSpeed 5.0f
#define RagDoll_MinimumGetUpTime 2.0f

DECLARE_MULTICAST_DELEGATE(FOnBecomeUnavailbleTargetEvent);


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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UAudioComponent* AudioComp;

	UPROPERTY()
	    class UPawnStatsComponent* PawnStats;

	UPROPERTY()
		class UBase_AnimInstance* mBaseAnimRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterType)
		ECharacterType CharacterType;

	UPROPERTY()
		/** Timer Handle after death, and get up*/
		FTimerHandle mRagDollTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = RagDoll)
		/** Toggle body velocity check after min ragdoll period*/
		bool bCheckBodyVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = RagDoll)
		uint8 newRagDollIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = RagDoll)
		uint8 oldRagDollIndex;

#pragma region Movement Stats


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = RagDoll)
		bool bIsRagDoll;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = RagDoll)
		bool bIsRecoveringFromRagDoll;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		bool bIsWalking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = RagDoll)
		float RagDollBlendWeight;


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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bIsEnemy;

protected:


	/** Config character based on character type during beginplay*/
	void CharacterCustomInit();

	//FVector GetRagDollPulse(const FVector& dir, const AActor* _damageCauser) const;


	UFUNCTION()
		virtual void OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType
			, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
		virtual void OnTakePointDamageHandle(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
			class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
			const class UDamageType* DamageType, AActor* DamageCauser);

	/** Turn character to ragdoll*/
	virtual void KnockOut(const FVector& dir, const AActor* _damageCauser, const FName& _boneName);

	/** Unregister all the group that I am a threat to it
	* put character to ragdoll, Called on Hp = 0 */
	virtual void OnDead(const FVector& dir, const AActor* _damageCauser, const FName& _boneName);

	// Called on actor destroyed
	virtual void Kill();

	// Called ragdoll recover start
	virtual void OnGetUp();

public:
	
	FOnBecomeUnavailbleTargetEvent OnBecomeUnvailbleTargetEvent;


private:

	void ClampCapsuleToMesh();

	void ToggleBodyStopCheck();


public:

	bool IsEnemy() const { return bIsEnemy; }
	void DuringRagDoll();
	virtual void RagDollRecoverOnFinish();
	void DuringRagDollRecovering(float _deltaTime);
	bool IsOldKnockOut() const { return oldRagDollIndex == newRagDollIndex; }

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

	FORCEINLINE bool IsRagDoll() const { return bIsRagDoll; }
	FORCEINLINE bool IsRagDollRecovereing() const { return bIsRecoveringFromRagDoll; }
	FORCEINLINE class UAudioComponent* GetAudioComp() const { return AudioComp; }
	bool IsOnDefend() const;

	/** Check if this character is able to counter attack this attack*/
	bool OnCounterAttack(const FVector& _damageCauserRelative);

	float GetCurrentMaxSpeed() const;
	class AGear* GetCurrentWeapon() const;

};

