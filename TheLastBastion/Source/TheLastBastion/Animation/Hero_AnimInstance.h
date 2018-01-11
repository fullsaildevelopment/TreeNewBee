// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/Base_AnimInstance.h"
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
	CrossBow        UMETA(DisplayName = "CrossBow")
};

UENUM(BlueprintType)
enum class EAttackState : uint8
{
	/** Player init state*/
	None = 0   UMETA(DisplayName = "None"),
	/** Player speed will be override by animation, can dodge freely in this state*/
	PreWinding = 1  UMETA(DisplayName = "PreWinding"),
	/** Player action will be locked in this state*/
	Attacking = 2   UMETA(DisplayName = "Attacking"),
	/** Player action can move freely*/
	ReadyForNext = 3 UMETA(DisplayName = "ReadyForNext"),
	/** Player speed and direction will be override by dodge animation in this state
	*   No Only Action allowed */
	Dodging = 4 UMETA(DisplayName = "Dodging"),
	/** Player speed and direction still override by Dodging anim, but can perform next action, but not dodge*/
	PostDodging = 5 UMETA(DisplayName = "PostDodging")
};

UENUM(BlueprintType)
enum class EActionType : uint8
{
	None = 0   UMETA(DisplayName = "None"),
	Attack = 1  UMETA(DisplayName = "Attack"),
	Skill = 2  UMETA(DisplayName = "Skill"),
	Dodge = 3   UMETA(DisplayName = "Dodge")
};

UENUM(BlueprintType)
enum class EFocusDodgeDirection :uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Forward  = 1  UMETA(DisplayName = "Forward"),
	Right45 = 2   UMETA(DisplayName = "Right45"),
	Right90 = 3   UMETA(DisplayName = "Right90"),
	Right135 = 4  UMETA(DisplayName = "Right135"),
	Back = 5      UMETA(DisplayName = "Back"),
	Left135 = 6   UMETA(DisplayName = "Left135"),
	Left90 = 7    UMETA(DisplayName = "Left90"),
	Left45 = 8    UMETA(DisplayName = "Left45")
};

UCLASS()
class THELASTBASTION_API UHero_AnimInstance : public UBase_AnimInstance
{
	GENERATED_BODY()


public:

	UHero_AnimInstance(const FObjectInitializer& _objectInitalizer);

protected:

	UPROPERTY(BlueprintReadOnly)
		class ATheLastBastionHeroCharacter* mCharacter;


#pragma region Movement


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		/** If true, the velocity of character is controlled by animation
		* and calculated direction */
		bool bVelocityOverrideByAnim;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		/** If true, the rotation rate is controlled by animation */
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
	FVector Acceleration_bodySpace;

	FVector mAccelerationDirection;

	FVector mSpeedOverrideDirection;

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


#pragma region Combat
	
	UPROPERTY(BlueprintReadOnly, Category = Combat)
		/** Catch the next action,
		whether is dodge, attack or use skills
		None if there isnt one */
		EActionType NextAction;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Combat)
		/** Current activated equipment type */
		EEquipType ActivatedEquipment;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Combat)
		/** The type of weapon is going to use when player draw weapon*/
		EEquipType CurrentEquipment;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
		EAttackState AttackState;

				
#pragma endregion

	
protected:

		virtual void OnBeginPlay() override;

		virtual void OnInit() override;

		virtual void OnUpdate(float _deltaTime) override;

		virtual void OnPostEvaluate() override;


#pragma region Anim Notification


		UFUNCTION(BlueprintCallable)
			/** Called by animation state machine, to sync animation and movement */
			void StartOverrideSpeed();

		UFUNCTION(BlueprintCallable)
			void StopOverrideSpeed();

		/** Called when enter idle state and loop state during travel mode*/
		UFUNCTION(BlueprintCallable)
			void EnableJump();

		/** Called when leave idle state or loop state, and also called when exit travel mode*/
		UFUNCTION(BlueprintCallable)
			void DisableJump();

		/** Called when Character draw his weapon during animation,
		toggle on use desired control rotation*/
		UFUNCTION(BlueprintCallable)
			virtual void OnEquipWeapon();

		/** Called when Character draw his weapon during animation,
		toggle off use desired control rotation*/
		UFUNCTION(BlueprintCallable)
			virtual void OnSheathWeapon();

#pragma endregion

public:

#pragma region Action Mapping


	/** Called when attack button is called*/
	virtual void OnAttack();
	/** Called when equip button is pressed*/
	virtual bool OnEquip();
	/** Called when Jump button is pressed*/
	virtual void OnJumpStart();
	/** Called when Jump button is released*/
	virtual void OnJumpStop();
	/** Called when Sprint Button is pressed*/
	void OnSprintPressed();
	/** Called when Sprint Button is released*/
	void OnSprintReleased();



	///** Custom Binding based on Character type

	/** Called when middle mouse button is pressed*/
	virtual void OnMiddleMouseButtonPressed();

	/** Called when C or LAlt (Crouch / Dodge) is pressed*/
	virtual void OnCorLAltPressed();

	/** Called when RMB is pressed*/
	virtual void OnRightMouseButtonPressed();

	/** Called when RMB is released*/
	virtual void OnRightMouseButtonReleased();



#pragma endregion

	virtual void OnBeingHit
	( float _damage, FName boneName, const FVector& _shotFromDirection, const class UPawnStatsComponent* _pawnStats) override;

	virtual void OnActionInterrupt();


protected:


	/** Called when player try to attack and dodge and use skill without equip at the first place
	    Simply attach a weapon on the slot without play the animation, disable jump*/

	void SkipEquip();

	void HeadTrack();


public:

		FORCEINLINE bool IsVelocityOverrideByAnim() const { return bVelocityOverrideByAnim; }
		FORCEINLINE bool IsRotationRateOverrideByAnim() const { return bRotationRateOverrideByAnim; }
		FORCEINLINE bool GetIsJumpEnable() const { return bEnableJump; }
		FORCEINLINE void SetIsJump(bool _val) { bTryToJump = _val; }

		FORCEINLINE EEquipType GetCurrentEquipmentType() const { return CurrentEquipment; }
		FORCEINLINE EEquipType GetActivatedEquipmentType() const { return ActivatedEquipment; }
};
