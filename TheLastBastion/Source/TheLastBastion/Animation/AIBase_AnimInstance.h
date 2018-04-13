// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/Base_AnimInstance.h"
#include "AIBase_AnimInstance.generated.h"



UENUM(BlueprintType)
enum class EAIActionState : uint8
{
	None = 0       UMETA(DisplayName = "None"),
	MeleePreAttack UMETA(DisplayName = "Melee PreAttack"),   // Winding up weapon
	MeleeAttack    UMETA(DisplayName = "Melee Attack"),      // During attack
	MeleePostAttack UMETA(DisplayName = "Melee PostAttack"), // Recover from attack
	Fire           UMETA(DisplayName = "Fire"),
	Dodge          UMETA(DisplayName = "Dodge"),
	Defend         UMETA(DisplayName = "Defend"),
	GettingHurt    UMETA(DisplayName = "GettingHurt")
};

UENUM(BlueprintType)
enum class EAIMeleeAttackType : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	InPlace = 1      UMETA(DisplayName = "In Place"),
	Move = 2         UMETA(DisplayName = "Move"),
	Move_InPlace = 3 UMETA(DisplayName = "Move_InPlace")
};


#define HEADBONE TEXT("neck_01")
#define RIGHTLEGBONE TEXT("calf_r")
#define LEFTLEGBONE TEXT("calf_l")

#define SH_HitReaction_HEAD_RIGHT      TEXT("Head_R")
#define SH_HitReaction_HEAD_LEFT       TEXT("Head_L")
#define SH_HitReaction_HEAD_FRONT      TEXT("Head_F")
#define SH_HitReaction_BODY_RIGHT      TEXT("Body_R")
#define SH_HitReaction_BODY_LEFT       TEXT("Body_L")
#define SH_HitReaction_BODY_FRONT      TEXT("Body_F")
#define SH_HitReaction_LEG_RIGHT       TEXT("Leg_R")
#define SH_HitReaction_LEG_LEFT        TEXT("Leg_L")

#define HV_HitReact_Back_0         TEXT("HitBack_0")
#define HV_HitReact_Back_1         TEXT("HitBack_1")
#define HV_HitReact_Right_0        TEXT("HitRight_0")
#define HV_HitReact_Right_1        TEXT("HitRight_1")
#define HV_HitReact_Left_0         TEXT("HitLeft_0")
#define HV_HitReact_Left_1         TEXT("HitLeft_1")
#define HV_HitReact_Front_0        TEXT("HitFront_0")
#define HV_HitReact_Front_1        TEXT("HitFront_1")
#define HV_HitReact_BackTurn_Right TEXT("HitBack_TurnRight")
#define HV_HitReact_BackTurn_Left  TEXT("HitBack_TurnLeft")

#define TH_HitReact_Back     TEXT("HitBack")
#define TH_HitReact_LeftTop  TEXT("HitLeft_Head")
#define TH_HitReact_RightTop TEXT("HitRight_Head")
#define TH_HitReact_FrontTop TEXT("HitFront_Head")
#define TH_HitReact_Left     TEXT("HitLeft")
#define TH_HitReact_Right    TEXT("HitRight")
#define TH_HitReact_Front    TEXT("HitFront_Hip")

#define SnsCB_HitReact_Front     TEXT("HitFront")
#define SnsCB_HitReact_FrontTop  TEXT("HitHead")
#define SnsCB_HitReact_Left      TEXT("HitLeft")
#define SnsCB_HitReact_Right     TEXT("HitRight")
#define SnsCB_HitReact_Back      TEXT("HitBack")

DECLARE_DELEGATE_OneParam(FOnFinishAttackSignature, class UBehaviorTreeComponent*);
DECLARE_DELEGATE_OneParam(FOnRecoverFromHitSignature, class UBehaviorTreeComponent*);


UCLASS()
class THELASTBASTION_API UAIBase_AnimInstance : public UBase_AnimInstance
{
	GENERATED_BODY()
	
	
public:
	UAIBase_AnimInstance(const FObjectInitializer& _objectInitalizer);

protected:


	UPROPERTY(BlueprintReadOnly, Category = Movement)
		FVector mAccelerationDirection;


	UPROPERTY()
	    class ATheLastBastionAIBase* mCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
		EAIActionState CurrentActionState;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
		EAIActionState NextAction;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
		bool bInMeleeRange;

public:

	FOnFinishAttackSignature OnFinishAttackDelegate;
	FOnRecoverFromHitSignature OnRecoverFromHitSignature;

protected:

	UFUNCTION(BlueprintCallable)
	virtual void OnBeginPlay();

	UFUNCTION(BlueprintCallable)
	virtual void OnInit();

	UFUNCTION(BlueprintCallable)
	virtual void OnUpdate(float _deltaTime);

	UFUNCTION(BlueprintCallable)
	virtual void OnPostEvaluate();

	UFUNCTION()
		void OnMontageStartHandle(class UAnimMontage* _animMontage) override;

	UFUNCTION()
		void OnMontageBlendOutStartHandle(class UAnimMontage* _animMontage, bool _bInterruptted) override;


	// FX
	UFUNCTION(BlueprintCallable)
		void FxMeleeSwing(bool _rightHand = true) override;


public:

	// Called when an enemy BT decide to attack in Melee
	virtual void Attack(int _attackType, int _maxCounter) {}

	// Called when an enemy BT decide to shot from far range
	virtual void Fire(const AActor* _target);

	virtual void StopFire();

	// Called When Attack Sequence is done by the end of animation sequence
	virtual void FinishAttack();

	// Called when ai being hit
	void ResetOnBeingHit() override;

	void OnBeingHit(FName boneName, const FVector& _damageCauseRelative,
		const FVector& _hitLocation) override;

	bool OnCounterAttack(const FVector & _damageCauserRelative) override;

	void OnParry(FName sectionName, UAnimMontage* const _parryMontage);

	void UpdateAnimationSetOnWeaponChange(EGearType _gearType) override;

	UFUNCTION(BlueprintPure)
		FORCEINLINE EAIActionState GetCurrentActionState() const {return CurrentActionState;}

	UFUNCTION(BlueprintPure)
		/** Check if animation ready to perform next fire */
		FORCEINLINE bool CanFire() const { return !Montage_IsPlaying(Fire_Montage) && CurrentActionState == EAIActionState::None;}

protected:

	void SyncMotionForMeleeAttack();

	void SyncMotionForGettingHurt();

	void SyncMotionForDefend();

	void SyncMotionForNone();

	FName HitReaction_SHSword(FName boneName, const FVector& _shotFromDirection, const FVector& _hitLocation);
	FName HitReaction_Katana(FName boneName, const FVector& _shotFromDirection, const FVector& _hitLocation);
	FName HitReaction_HV(FName boneName, const FVector& _shotFromDirection, const FVector& _hitLocation);
	FName HitReaction_Sns_CB(FName boneName, const FVector& _shotFromDirection, const FVector& _hitLocation);

	// Called on HitMontage end uninterruptted
	void OnHitMontageEnd();

	// Called on recoverred from ragdoll
	void OnGetupMontageEnd();

};
