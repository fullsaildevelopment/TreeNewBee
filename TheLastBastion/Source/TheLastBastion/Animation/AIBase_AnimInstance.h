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
	Defend = 4 UMETA(DisplayName = "Defend")
};

UENUM(BlueprintType)
enum class EAIMeleeAttackType : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	InPlace = 1      UMETA(DisplayName = "In Place"),
	Move = 2         UMETA(DisplayName = "Move"),
	Move_InPlace = 3 UMETA(DisplayName = "Move_InPlace")
};



DECLARE_DELEGATE_OneParam(FOnFinishAttackSignature, class UBehaviorTreeComponent*);

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
	virtual void Fire();

	// Called When Attack Sequence is done by the end of animation sequence
	virtual void FinishAttack();

	UFUNCTION(BlueprintPure)
		FORCEINLINE EAIActionState GetCurrentActionState() const {return CurrentActionState;}


	void OnBeingHit(float _damage, FName boneName, 
		const FVector& _shotFromDirection, const class UPawnStatsComponent* _pawnStats) override;


private:

	FName HitReaction_SHSword(FName boneName, const FVector & _shotFromDirection);


};
