// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Hero_AnimInstance.generated.h"

/**
 * 
 */
UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EEquipType : uint8
{
	Travel = 0 	    UMETA(DisplayName = "Travel"),
	ShieldSword 	UMETA(DisplayName = "ShieldSword"),
	TwoHandSword	UMETA(DisplayName = "TwoHandSword"),
	Bow             UMETA(DisplayName = "Bow")
};

UENUM(BlueprintType)
enum class EAttackState : uint8
{
	/** Player init state*/
	None = 0   UMETA(DisplayName = "None"),
	/** Player can dodge freely in this state*/
	PreWinding = 1  UMETA(DisplayName = "PreWinding"),
	/** Player action will be locked in this state*/
	Attacking = 2   UMETA(DisplayName = "Attacking"),
	/** Player action can move freely*/
	ReadyForNext = 3 UMETA(DisplayName = "ReadyForNext")
};



UCLASS()
class THELASTBASTION_API UHero_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:

	UHero_AnimInstance(const FObjectInitializer& _objectInitalizer);

protected:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
	    bool bSpeedOverrideByAnim;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
	    bool bRotationRateOverrideByAnim;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		bool bEnableJump;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		bool bTryToJump;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		bool bTryToMove;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float currentSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float turn;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = HeadTrack)
		float HeadTrackRate;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = HeadTrack)
		float HeadTrackYaw;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = HeadTrack)
		float HeadTrackPitch;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = HeadTrack)
		float HeadTrackAlpha;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		FVector Acceleration_bodySpace;


	/** Current activated equipment type */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Equipment)
		EEquipType ActivatedEquipment;

	/** The type of weapon is going to use when player draw weapon*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Equipment)
		EEquipType CurrentEquipment;
	
	/** Check if player is trying to attack, set by player input,
	and reset when actual attack is happen, or player's attack get interruptted*/
	UPROPERTY(BlueprintReadOnly, Category = Action)
		bool bTryToAttack;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Action)
		class UAnimMontage* Equip_Montage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Action)
		class UAnimMontage* Hit_Montage;

	UPROPERTY(BlueprintReadOnly, Category = Action, meta = (AllowPrivateAccess = "true"))
		EAttackState AttackState;




	UPROPERTY(BlueprintReadOnly)
		class ATheLastBastionCharacter* mCharacter;


protected:

		virtual void OnBeginPlay();

		virtual void OnInit();

		virtual void OnUpdate(float _deltaTime);

		virtual void OnPostEvaluate();	

#pragma region Anim Notification

		UFUNCTION(BlueprintCallable)
			void StartOverrideSpeed();

		UFUNCTION(BlueprintCallable)
			void StopOverrideSpeed();

		UFUNCTION(BlueprintCallable)
			void EnableJump();

		UFUNCTION(BlueprintCallable)
			void DisableJump();

			virtual void OnEnableDamage(bool bIsright = true, bool bIsAll = false);

			virtual void OnDisableDamage(bool bIsright = true, bool bIsAll = false);

		UFUNCTION(BlueprintCallable)
			virtual void OnNextAttack();

		UFUNCTION(BlueprintCallable)
			virtual void OnResetCombo();

#pragma endregion

		float PlayMontage(class UAnimMontage* _animMontage, float _rate, FName _startSectionName = NAME_None);



public:


	virtual void OnAttack();
	virtual void OnEquip();
	virtual void OnBeingHit( const class AActor* const _attacker);

	FORCEINLINE bool IsSpeedOverrideByAnim() const { return bSpeedOverrideByAnim; }
	FORCEINLINE bool IsRotationRateOverrideByAnim() const { return bRotationRateOverrideByAnim; }
	FORCEINLINE bool GetIsJumpEnable() const { return bEnableJump; }
	FORCEINLINE void SetIsJump(bool _val) { bTryToJump = _val; }

	FORCEINLINE EEquipType GetCurrentEquipmentType() const { return CurrentEquipment; }
	FORCEINLINE EEquipType GetActivatedEquipmentType() const { return ActivatedEquipment;  }

private:

	void HeadTrack();
};
