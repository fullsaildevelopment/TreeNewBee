// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/Base_AnimInstance.h"
#include "AIBase_AnimInstance.generated.h"



UENUM(BlueprintType)
enum class EAIActionState : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	MeleeAttack = 1 UMETA(DisplayName = "Melee Attack"),
	Fire = 2 UMETA(DisplayName = "Fire"),
	Dodge = 3 UMETA(DisplayName = "Dodge"),
	Defend = 4 UMETA(DisplayName = "Defend"),
	GettingHurt = 5 UMETA(DisplayName = "GettingHurt")
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
	    class ATheLastBastionEnemyCharacter* mCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
		EAIActionState CurrentActionState;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
		EAIActionState NextAction;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
		/** raise the weapon to eye*/
		bool bAim;

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

public:

	// Called when an enemy BT decide to attack in Melee
	virtual void Attack(EAIMeleeAttackType _attackType);

	// Called when an enemy BT decide to shot from far range
	virtual void Fire(const AActor* _target);

	virtual void StopFire();

	// Called When Attack Sequence is done by the end of animation sequence
	virtual void FinishAttack();

	UFUNCTION(BlueprintPure)
		FORCEINLINE EAIActionState GetCurrentActionState() const {return CurrentActionState;}

protected:

	void SyncMotionForMeleeAttack();

	void SyncMotionForGettingHurt();

	FName HitReaction_SHSword(FName boneName, const FVector & _shotFromDirection, const FVector& _hitLocation);


};
