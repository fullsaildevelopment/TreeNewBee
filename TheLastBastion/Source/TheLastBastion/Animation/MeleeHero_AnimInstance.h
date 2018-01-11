// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	void OnAttack() override;

	/** Called when equip button is pressed, 
	depending on current equipment, play different equip animation */
	bool OnEquip() override;


	/** Called when player try to enter focus mode*/
	void OnMiddleMouseButtonPressed() override;

	/** Called when player click middle mouse button, or disable the focus by heroComp*/
	void OnFocus();

	/** Called when melee player try to dodge*/
	void OnCorLAltPressed() override;

	/** Called when combat properties requires to be reset*/
	void OnActionInterrupt() override;


protected:


	UFUNCTION(BlueprintCallable)
		/** Called when before attack state enter the ReadyForNext Stage,
		*   Trigger the catched action, if the next action is not None */
		void OnNextAttack();



	UFUNCTION(BlueprintCallable)
		void OnEnableDamage(bool bIsright = true, bool bIsAll = false);

	UFUNCTION(BlueprintCallable)
		void OnDisableDamage(bool bIsright = true, bool bIsAll = false);

	UFUNCTION(BlueprintCallable)
		/** Called when before attack state enter the DodgePost Stage,
		*   Trigger the catched action (except for dodge), if the next action is not None */
		virtual void OnDodgePost();


	UFUNCTION(BlueprintCallable)
		virtual void OnDodgeFinish();


	UFUNCTION()
		virtual void OnMontageStartHandle(class UAnimMontage* _animMontage);

	UFUNCTION()
		virtual void OnMontageBlendOutStartHandle(class UAnimMontage* _animMontage, bool _bInterruptted);



private:

	UPROPERTY(BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		/** which section I am going play next in Attack_Montage*/
	     int CurrentComboIndex;

protected:



	UPROPERTY(BlueprintReadOnly, Category = Combat)
		/** Flag to check if our character is in strafe mode,
		*  Set by Focus Button, Reset by Equip button and focus button
		*  if player is in travel mode, focus button will automatically implement equip*/
		bool bIsFocused;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	/** Due to I dont want to mess up my dodge movement property,
	*   if someone try to enter focus mode  during dodge, toggle this boolean to true, and wait
	*   until the dodge finish, enter the focus mode, reset this boolean*/
		bool bIsFocusEnterPending;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	/** Due to I dont want to mess up my dodge movement property,
	*   if someone try to exit focus mode during dodge, toggle this boolean to true, and wait
	*   until the dodge finish, exit the focus mode, reset this boolean */
		bool bIsFocusExitPending;




	UPROPERTY(BlueprintReadOnly, Category = Combat)
		/** spine angle to blend alpha*/
		float spineAngleOverrideAlpha;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Combat)
		/** Angle between Acceleration and Forward Direction must less than this angle to trigger dodge*/
		float DodgeMinTurnThreshold;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
		/** spine angle to blend */
		FRotator spineAngleRotator;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
		/** Must be assigned for before focused dodge*/
		EFocusDodgeDirection FocusDodgeDirection;




private:


	void LaunchCombo();

	/** Reset Combo when attack montage gets blended out without any interrupt*/
	void ResetCombo();




	/** Called when player press LAlt or C*/
	void OnDodge();

	void LaunchDodge();

	FVector GetFocusDodgeDirection() const;

	/** Toggle Focus mode on and off*/
	void ToggleFocusMode(bool _IsOn);




	public:
		FORCEINLINE bool GetIsFocus() const { return bIsFocused; }
		FORCEINLINE bool GetFocusPendingEnter() const { return bIsFocusEnterPending; }
		FORCEINLINE bool GetFocusPendingExit() const { return bIsFocusExitPending; }


};
