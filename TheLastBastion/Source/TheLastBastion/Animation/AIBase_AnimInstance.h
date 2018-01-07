// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AIBase_AnimInstance.generated.h"



UENUM(BlueprintType)
enum class EAIActionState : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	MeleeAttack = 1 UMETA(DisplayName = "Melee Attack"),
	Fire = 2 UMETA(DisplayName = "Fire"),
	Dodge = 3 UMETA(DisplayName = "Dodge"),
	Defend = 4 UMETA(DisplayName = "Defend")
};





DECLARE_DELEGATE_OneParam(FOnFinishAttackSignature, class UBehaviorTreeComponent*);

UCLASS()
class THELASTBASTION_API UAIBase_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	
public:
	UAIBase_AnimInstance(const FObjectInitializer& _objectInitalizer);

protected:

	UPROPERTY()
	    class ATheLastBastionEnemyCharacter* mCharacter;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		/** Squared Speed*/
		float currentSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float turn;
	
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	    FVector mAccelerationDirection;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
		EAIActionState CurrentActionState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
		/** raise the weapon to eye*/
		bool bAim;


public:

	FOnFinishAttackSignature OnFinishAttackDelegate;

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

	// Called when an enemy BT decide to attack
	virtual void Attack();

	// Called When Attack Sequence is done by the end of animation sequence
	virtual void FinishAttack();

	UFUNCTION(BlueprintPure)
		FORCEINLINE EAIActionState GetCurrentActionState() const {return CurrentActionState;}

};
