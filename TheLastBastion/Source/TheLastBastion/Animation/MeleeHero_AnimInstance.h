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
		/** Animation Update tick, run every frame*/
		void OnUpdate(float _deltaTime);

	UFUNCTION(BlueprintCallable)
		void OnPostEvaluate();

public:

	/** Called When Attack button is pressed, 
	depending on current attack state, it will decide to perform combo immediately, or catch 
	it and implement at OnNextAttack Event, or ignore */
	bool OnAttack() override;


	/** Called when equip button is pressed, 
	depending on current equipment, play different equip animation */
	bool OnEquip() override;

	/** Called when combat properties requires to be reset*/
	void OnActionInterrupt() override;


	void OnFocus() override;
	bool OnDodge() override;



protected:


	UFUNCTION(BlueprintCallable)
		/** Called when before attack state enter the ReadyForNext Stage,
		*   Trigger the catched action, if the next action is not None */
		void OnNextAttack() override;


	UFUNCTION(BlueprintCallable)
		/** Called when before attack state enter the DodgePost Stage,
		*   Trigger the catched action (except for dodge), if the next action is not None */
		virtual void OnDodgePost();


	UFUNCTION(BlueprintCallable)
		void OnResetCombo() override;

	UFUNCTION(BlueprintCallable)
		virtual void OnEnableDamage(bool bIsright = true, bool bIsAll = false);

	UFUNCTION(BlueprintCallable)
		virtual void OnDisableDamage(bool bIsright = true, bool bIsAll = false);


	UFUNCTION(BlueprintCallable)
		virtual void OnDodgeFinish();


private:



	UPROPERTY(BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		/** which section I am going play next in Attack_Montage*/
	     int CurrentComboIndex;
private:


	void LaunchCombo();
	void LaunchDodge() override;




};
