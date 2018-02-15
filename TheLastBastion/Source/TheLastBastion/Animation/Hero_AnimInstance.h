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
	CrossBow        UMETA(DisplayName = "CrossBow"),
	HeavyWeapon     UMETA(DisplayName = "HeavyWeapon")

};

UENUM(BlueprintType)
enum class EAttackState : uint8
{
	/** Player init state*/
	None = 0   UMETA(DisplayName = "None"),
	/** Player speed will be override by animation, can dodge freely in this state*/
	PreWinding = 1  UMETA(DisplayName = "PreWinding"),
	/** Player action, movement will be locked in this state*/
	Attacking = 2   UMETA(DisplayName = "Attacking"),
	/** Player can perform next action freely, movement is locked*/
	ReadyForNext = 3 UMETA(DisplayName = "ReadyForNext"),
	/** Player speed and direction will be override by dodge animation in this state
	*   No Action allowed */
	Dodging = 4 UMETA(DisplayName = "Dodging"),
	/** Player speed and direction still override by Dodging anim, but can perform next action, but not dodge*/
	PostDodging = 5 UMETA(DisplayName = "PostDodging"),
	/** Player has no control */
	BeingHit = 6        UMETA(DisplayName = "BeingHit"),
	//AirAttacking = 7    UMETA(DisplayName = "AirAttacking"),
	//AirReadyForNext = 8 UMETA(DisplayName = "AirReadyForNext")
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

enum class EGearType : uint8;

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


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Movement)
		/** If true, the velocity of character is controlled by animation
		* and calculated direction */
		bool bVelocityOverrideByAnim;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Movement)
		bool bEnableJump;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Movement)
		bool bTryToJump;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Movement)
		bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Movement)
		bool bTryToMove;

	/** Is Character currently sprinting*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
		bool bIsSprinting;

	/** Is Sprinting button is still pressed, only reset by button release */
	UPROPERTY(BlueprintReadOnly, Category = Movement)
		bool bTryToSprint;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
		/** A boolean needs to be toggle when equip or unequip gears*/
		bool bTryToEquip;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
	    FVector Acceleration_bodySpace;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	FVector mAccelerationDirection;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	FVector mSpeedOverrideDirection;

#pragma endregion

#pragma region Bone Control

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = HeadTrack)
		float HeadTrackRate;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = HeadTrack)
		float HeadTrackYaw;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = HeadTrack)
		float HeadTrackPitch;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = HeadTrack)
		float HeadTrackAlpha;

	UPROPERTY(BlueprintReadOnly, Category = MeleeAttack)
		/** spine angle to blend alpha*/
		float spineAngleOverrideAlpha;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = MeleeAttack)
		float ShieldUpBlendWeight;

	UPROPERTY(BlueprintReadOnly, Category = MeleeAttack)
		/** spine angle to blend */
		FRotator spineAngleRotator;

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

	UPROPERTY(BlueprintReadOnly, Category = Combat)
		/** Must be assigned for before focused dodge*/
		EFocusDodgeDirection FocusDodgeDirection;	

	UPROPERTY(BlueprintReadOnly, Category = Combat)
		/** which section I am going play next in Attack_Montage*/
		int CurrentComboIndex;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = MeleeCombo)
		class UAnimMontage*  LongSword_Montage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly ,Category = MeleeCombo)
		class UAnimMontage*  AxeMace_Montage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = MeleeCombo)
		class UAnimMontage*  Katana_Montage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly,Category = MeleeCombo)
		class UAnimMontage*  GreatSwordHammer_Montage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = MeleeCombo)
		class UAnimMontage*  BattleAxe_Montage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = MeleeCombo)
		class UAnimMontage*  KatanaAirAttack_Montage;

	/** A pointer point to the combo list for current melee weapon, 
	    when change weapon and equip to determine which combo list we used for melee attack */
	const TArray<FName>* Current_AttackSectionName;



#pragma endregion

#pragma region MeleeFocus

	UPROPERTY(BlueprintReadOnly, Category = MeleeFocus)
		/** Flag to check if our character is in strafe mode,
		*  Set by Focus Button, Reset by Equip button and focus button
		*  if player is in travel mode, focus button will automatically implement equip*/
		bool bIsFocused;

	UPROPERTY(BlueprintReadOnly, Category = MeleeFocus)
		/** Due to I dont want to mess up my dodge movement property,
		*   if someone try to enter focus mode  during dodge, toggle this boolean to true, and wait
		*   until the dodge finish, enter the focus mode, reset this boolean*/
		bool bIsFocusEnterPending;

	UPROPERTY(BlueprintReadOnly, Category = MeleeFocus)
		/** Due to I dont want to mess up my dodge movement property,
		*   if someone try to exit focus mode during dodge, toggle this boolean to true, and wait
		*   until the dodge finish, exit the focus mode, reset this boolean */
		bool bIsFocusExitPending;

#pragma endregion

#pragma region RangeFire

	UPROPERTY(BlueprintReadOnly, Category = Fire)
		bool bTryToZoomIn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fire)
		/** FOV blend rate zoom in and zoom out*/
		float CameraZoomInRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fire)
		/** Camera blend rate between travel mode and crossbow mode*/
		float CameraShiftRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fire)
		/** Camera relative location during equip */
		FVector CameraEquipOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fire)
		/** Camera relative location during Zoom In */
		FVector CameraZoomOffset;

#pragma endregion


protected:

	UFUNCTION(BlueprintCallable)
		void OnBeginPlay() override;
	UFUNCTION(BlueprintCallable)
		void OnInit() override;
	UFUNCTION(BlueprintCallable)
		void OnUpdate(float _deltaTime) override;
	UFUNCTION(BlueprintCallable)
		void OnPostEvaluate() override;
	UFUNCTION()
		void OnMontageStartHandle(class UAnimMontage* _animMontage) override;
	UFUNCTION()
		void OnMontageBlendOutStartHandle(class UAnimMontage* _animMontage, bool _bInterruptted) override;

public:

#pragma region Action Mapping

	/** Called when attack button is called*/
	void OnAttack();
	/** Called when equip button is pressed*/
	bool OnEquip();
	/** Called when Jump button is pressed*/
	void OnJumpStart();
	/** Called when Jump button is released*/
	void OnJumpStop();
	/** Called when Sprint Button is pressed*/
	void OnSprintPressed();
	/** Called when Sprint Button is released*/
	void OnSprintReleased();

	void OnFocus();

	///** Custom Binding based on Character type

	/** Called when middle mouse button is pressed*/
	void OnMiddleMouseButtonPressed();

	/** Called when C or LAlt (Crouch / Dodge) is pressed*/
	void OnCorLAltPressed();

	/** Called when RMB is pressed*/
	void OnRightMouseButtonPressed();

	/** Called when RMB is released*/
	void OnRightMouseButtonReleased();

	/** Called when TAB button pressed*/
	void OnSwapBetweenMeleeAndRange();

	void OnChangeWeapon(EEquipType _next);

	/** Called to end Change Weapon event*/
	void OnChangeWeaponFinsh();

#pragma endregion

	void OnBeingHit
	(FName boneName, const FVector& _shotFromDirection, const FVector& _hitLocation) override;

	/** Called When switch melee weapon and equip*/
	void UpdateComboList(EGearType _gearType);

	void AnimInstanceResetOnRagDoll() override;

private:

	/** Update the Yaw and Pitch for head track based on Control rotation between mesh forward vector*/
	void HeadTrack();

	/** Called when player try to attack and dodge and use skill without equip at the first place
	    Simply attach a weapon on the slot without play the animation, disable jump, change movement rules to strafe*/
	void SkipEquip();

	/** Similar to SkipEquip, without change movement rules*/
	void AttachWeapon();


	void RecoverFromBeingHit(bool _bInterrupted);



	///*** Melee Function ***///

	void MeleeUpdate(class UCharacterMovementComponent* movementComp, float _deltaTime);
	void MeleeRotateRateAndMaxSpeedUpdate(class UCharacterMovementComponent* movementComp);

	/** Called OnAttack is called and Player's current Equipment is melee weapon*/
	void OnMeleeAttack();

	void OnDefendOn();
	void OnDefendOff();

	void LaunchCombo();
	/** Reset Combo when attack montage gets blended out without any interrupt*/
	void ResetCombo();
	/** Called when player press LAlt or C*/
	void OnDodge();
	void LaunchDodge();
	void OnDodgeFinish(bool _bInterruptted);

	FName GetForwardDodgeSection() const;
	FName GetRightDodgeSection() const;
	FName GetLeftDodgeSection() const;
	FName GetBackDodgeSection() const;

	FVector GetFocusDodgeDirection() const;
	/** Toggle Focus mode on and off*/
	void ToggleFocusMode(bool _IsOn);

	///*** Range Function***///

	void RangeUpdate(class UCharacterMovementComponent* movementComp, float _deltaTime);
	/** Called OnAttack is called if Player's current Equipment is range weapon*/
	void OnRangeAttack();

	/** Called When RMB is pressed, if Player's current Equipment is range weapon*/
	void OnZoomIn();

	/** Called When RMB is released, if Player's current Equipment is range weapon*/
	void OnZoomOut();


protected:


#pragma region Travel Event

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

#pragma endregion

#pragma region  Equip Event

	/** Called when Character draw his weapon during animation,
	toggle on use desired control rotation*/
	UFUNCTION(BlueprintCallable)
		virtual void OnEquipWeapon();

	/** Called when Character draw his weapon during animation,
	toggle off use desired control rotation*/
	UFUNCTION(BlueprintCallable)
		virtual void OnSheathWeapon();

#pragma endregion

#pragma region Melee Attack Event

	UFUNCTION(BlueprintCallable)
		/** Called when before attack state enter the ReadyForNext Stage,
		*   Trigger the catched action, if the next action is not None */
		void OnNextAttack();

	UFUNCTION(BlueprintCallable)
		void OnEnableDamage(bool bIsright = true, bool bIsAll = false);

	UFUNCTION(BlueprintCallable)
		void OnDisableDamage(bool bIsright = true, bool bIsAll = false);

#pragma endregion

#pragma region Dodge Event

	UFUNCTION(BlueprintCallable)
		/** Called when before attack state enter the DodgePost Stage,
		*   Trigger the catched action (except for dodge), if the next action is not None */
		void OnDodgePost();



#pragma endregion


	void ResetOnBeingHit() override;

public:

	FORCEINLINE bool IsVelocityOverrideByAnim() const { return bVelocityOverrideByAnim; }
	FORCEINLINE bool GetIsJumpEnable() const { return bEnableJump; }
	FORCEINLINE void SetIsJump(bool _val) { bTryToJump = _val; }

	FORCEINLINE EEquipType GetCurrentEquipmentType() const { return CurrentEquipment; }
	FORCEINLINE EEquipType GetActivatedEquipmentType() const { return ActivatedEquipment; }

	FORCEINLINE bool GetIsFocus() const { return bIsFocused; }
	FORCEINLINE bool GetFocusPendingEnter() const { return bIsFocusEnterPending; }
	FORCEINLINE bool GetFocusPendingExit() const { return bIsFocusExitPending; }

};
