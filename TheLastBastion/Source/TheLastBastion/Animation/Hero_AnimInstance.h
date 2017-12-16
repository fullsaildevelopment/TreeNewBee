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
	

#pragma region Movement

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

	/** Is Character currently sprinting*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
		bool bIsSprinting;

	/** Is Sprinting button is still pressed, only reset by button release */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
		bool bTryToSprint;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float currentSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float turn;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		FVector Acceleration_bodySpace;
#pragma endregion


#pragma region HeadTrack
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = HeadTrack)
		float HeadTrackRate;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = HeadTrack)
		float HeadTrackYaw;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = HeadTrack)
		float HeadTrackPitch;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = HeadTrack)
		float HeadTrackAlpha;
#pragma endregion




	/** Check if player is trying to attack, set by player input,
	and reset when actual attack is happen, or player's attack get interruptted*/
	UPROPERTY(BlueprintReadOnly, Category = Combat)
		bool bTryToAttack;

	/** Current activated equipment type */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Combat)
		EEquipType ActivatedEquipment;

	/** The type of weapon is going to use when player draw weapon*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Combat)
		EEquipType CurrentEquipment;
	

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Combat)
		class UAnimMontage* Equip_Montage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Combat)
		class UAnimMontage* Hit_Montage;

	UPROPERTY(BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		EAttackState AttackState;

	UPROPERTY(BlueprintReadOnly)
		class ATheLastBastionHeroCharacter* mCharacter;


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


		UFUNCTION(BlueprintCallable)
			virtual void OnNextAttack();

		UFUNCTION(BlueprintCallable)
			virtual void OnResetCombo();


		virtual void OnEnableWeapon(bool bIsright = true, bool bIsAll = false);

		virtual void OnDisableWeapon(bool bIsright = true, bool bIsAll = false);


		/** Called when Character draw his weapon during animation*/
		UFUNCTION(BlueprintCallable)
		virtual void OnEquipWeapon();

		/** Called when Character collect his weapon during animation*/
		UFUNCTION(BlueprintCallable)
		virtual void OnSheathWeapon();

#pragma endregion

		float PlayMontage(class UAnimMontage* _animMontage, float _rate, FName _startSectionName = NAME_None);


public:

	/** Called when attack button is called*/
	virtual void OnAttack();
	/** Called when equip button is pressed*/
	virtual void OnEquip();

	virtual void OnJumpStart();

	virtual void OnJumpStop();

	virtual void OnBeingHit( const class AActor* const _attacker);

	virtual void OnComboInterrupt();

	void SetTryToSprint(bool _val);

	/** the Animation Instance react to Sprint Key Button*/ 
	void OnSprintPressed();
	void OnSprintReleased();

	FORCEINLINE bool IsSpeedOverrideByAnim() const { return bSpeedOverrideByAnim; }
	FORCEINLINE bool IsRotationRateOverrideByAnim() const { return bRotationRateOverrideByAnim; }
	FORCEINLINE bool GetIsJumpEnable() const { return bEnableJump; }
	FORCEINLINE void SetIsJump(bool _val) { bTryToJump = _val; }

	FORCEINLINE EEquipType GetCurrentEquipmentType() const { return CurrentEquipment; }
	FORCEINLINE EEquipType GetActivatedEquipmentType() const { return ActivatedEquipment;  }

private:

	void HeadTrack();
};
