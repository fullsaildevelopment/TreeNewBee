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

	//UMeleeHero_AnimInstance(const FObjectInitializer& _objectInitalizer);


protected:
	
	//UFUNCTION(BlueprintCallable)
	//	void OnBeginPlay() override;

	//UFUNCTION(BlueprintCallable)
	//	void OnInit();

	//UFUNCTION(BlueprintCallable)
	//	/** Animation Update tick, run every frame*/
	//	void OnUpdate(float _deltaTime);

	//UFUNCTION(BlueprintCallable)
	//	void OnPostEvaluate();

public:

	///** Called When Attack button is pressed, 
	//depending on current attack state, it will decide to perform combo immediately, or catch 
	//it and implement at OnNextAttack Event, or ignore */
	//void OnAttack() override;

	///** Called when equip button is pressed, 
	//depending on current equipment, play different equip animation */
	//bool OnEquip() override;


	///** Called when player try to enter focus mode*/
	//void OnMiddleMouseButtonPressed() override;

	///** Called when player click middle mouse button, or disable the focus by heroComp*/
	//void OnFocus();

	///** Called when melee player try to dodge*/
	//void OnCorLAltPressed() override;

	///** Called when combat properties requires to be reset*/
	//void ResetOnBeingHit() override;


protected:




	//UFUNCTION()
	//	void OnMontageStartHandle(class UAnimMontage* _animMontage) override;


	//UFUNCTION()
	//	void OnMontageBlendOutStartHandle(class UAnimMontage* _animMontage, bool _bInterruptted) override;



private:



protected:



	//UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Combat)
	//	/** Angle between Acceleration and Forward Direction must less than this angle to trigger dodge*/
	//	float DodgeMinTurnThreshold;







private:













	public:
		//FORCEINLINE bool GetIsFocus() const { return bIsFocused; }
		//FORCEINLINE bool GetFocusPendingEnter() const { return bIsFocusEnterPending; }
		//FORCEINLINE bool GetFocusPendingExit() const { return bIsFocusExitPending; }

};
