// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AIBase_AnimInstance.generated.h"



UENUM(BlueprintType)
enum class EAIActionState : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Attack = 1 UMETA(DisplayName = "Attack"),
	Dodge = 2 UMETA(DisplayName = "Dodge"),
	Defend = 3 UMETA(DisplayName = "Defend")
};
/**
 * 
 */

DECLARE_DELEGATE_OneParam(FOnFinishAttackSignature, class UBehaviorTreeComponent*);


UCLASS()
class THELASTBASTION_API UAIBase_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	
public:
	UAIBase_AnimInstance(const FObjectInitializer& _objectInitalizer);

protected:

		class ATheLastBastionEnemyCharacter* mCharacter;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float currentSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float turn;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	    FVector mAccelerationDirection;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
		EAIActionState CurrentActionState;

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

#pragma region Anim Notification

	UFUNCTION(BlueprintCallable)
	void OnEnableWeapon(bool bIsright = true, bool bIsAll = false);

	UFUNCTION(BlueprintCallable)
	void OnDisableWeapon(bool bIsright = true, bool bIsAll = false);


#pragma endregion

public:

	// Called when an enemy BT decide to attack
	UFUNCTION(BlueprintCallable)
	void Attack();

	// Called When Attack Sequence is done by the end of animation sequence
	UFUNCTION(BlueprintCallable)
	void FinishAttack();

	UFUNCTION(BlueprintPure)
		FORCEINLINE EAIActionState GetCurrentActionState() const {return CurrentActionState;}
};
