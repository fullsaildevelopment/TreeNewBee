// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/Hero_AnimInstance.h"
#include "MeleeHero_AnimInstance.generated.h"

/**
 * 
 */

UCLASS()
class THELASTBASTION_API UMeleeHero_AnimInstance : public UHero_AnimInstance
{
	GENERATED_BODY()


public:

	UMeleeHero_AnimInstance(const FObjectInitializer& _objectInitalizer);


protected:
	
	UFUNCTION(BlueprintCallable)
		void OnBeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void OnInit();

	UFUNCTION(BlueprintCallable)
		void OnUpdate(float _deltaTime);

	UFUNCTION(BlueprintCallable)
		void OnPostEvaluate();

public:

	void OnAttack() override;
	void OnEquip() override;


protected:


	// perform the next non - first attack
	UFUNCTION(BlueprintCallable)
		void OnNextAttack() override;


	UFUNCTION(BlueprintCallable)
		void OnResetCombo() override;


	UFUNCTION(BlueprintCallable)
		virtual void OnEnableDamage(bool bIsright = true, bool bIsAll = false);

	UFUNCTION(BlueprintCallable)
		virtual void OnDisableDamage(bool bIsright = true, bool bIsAll = false);





private:

	/** The very basic combo */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Action, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* Attack_Montage;

	/** which section I am going play next in Attack_Montage*/
	UPROPERTY(BlueprintReadOnly, Category = Action, meta = (AllowPrivateAccess = "true"))
	     int CurrentComboIndex;

private:

	void DoNextAttack();





};
