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
		void OnEnableWeapon(bool bIsright = true, bool bIsAll = false);
	UFUNCTION(BlueprintCallable)
		/** Disable Damage Collision for melee weapon*/
		void OnDisableWeapon(bool bIsright = true, bool bIsAll = false);
	
	UFUNCTION(BlueprintCallable)
		/** Called at the end of attack frame, and let BT know attack is finished*/
		void FinishAttack() override;

	UFUNCTION(BlueprintCallable)
		/** Called at the end of attack frame, and let BT know attack is finished*/
		void InitAttack();


#pragma endregion

public:

	UFUNCTION(BlueprintCallable)
		void Attack(EAIMeleeAttackType _attackType) override;


private:

	void SyncMotionForMeleeAttack();


protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = MeleeAttackChoice)
		EAIMeleeAttackType attackChoice;



};
