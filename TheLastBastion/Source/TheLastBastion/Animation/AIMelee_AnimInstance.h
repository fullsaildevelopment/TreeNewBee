// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AIBase_AnimInstance.h"
#include "AIMelee_AnimInstance.generated.h"

/**
 * 
 */


UCLASS()
class THELASTBASTION_API UAIMelee_AnimInstance : public UAIBase_AnimInstance
{
	GENERATED_BODY()
	
protected:

	UFUNCTION(BlueprintCallable)
		 void OnBeginPlay() override;

	UFUNCTION(BlueprintCallable)
		 void OnInit() override;

	UFUNCTION(BlueprintCallable)
		 void OnUpdate(float _deltaTime) override;

	UFUNCTION(BlueprintCallable)
		 void OnPostEvaluate() override;

#pragma region Anim Notification

	UFUNCTION(BlueprintCallable)
		/** Enable Damage Collision for melee weapon*/
		void OnEnableWeapon(bool bIsright = true, bool bIsAll = false) override;
	UFUNCTION(BlueprintCallable)
		/** Disable Damage Collision for melee weapon*/
		void OnDisableWeapon(bool bIsright = true, bool bIsAll = false) override;
	
	UFUNCTION(BlueprintCallable)
		/** Called at the end of attack recover, and let BT know attack is finished*/
		void FinishAttack() override;

	UFUNCTION(BlueprintCallable)
		/** Called at the end of attack frame, and let BT know attack is finished*/
		void InitAttack();

	UFUNCTION(BlueprintCallable)
		/** Called at the end of attack frame, and so the character can parry the attack*/
		void AttackRecover();


#pragma endregion




public:

	UFUNCTION(BlueprintCallable)
		void Attack(int _attackType, int _maxCounter) override;

	UFUNCTION(BlueprintCallable)
		void OnNextCombo();

	void ResetOnBeingHit() override;


protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = MeleeAttackChoice)
		float attackChoice;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = MeleeAttackChoice)
		/** How many times we launch combos before interrupt*/
		int MaxComboCounter;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = MeleeAttackChoice)
		/** How many times we already have launched combos */
		int CurrentComboCounter;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = MeleeAttackChoice)
		/** is the combo has odd index*/
		bool bComboOddIndex;

};
