// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AIBase_AnimInstance.h"
#include "AIRange_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UAIRange_AnimInstance : public UAIBase_AnimInstance
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

	UFUNCTION()
		void OnMontageStartHandle(class UAnimMontage* _animMontage) override;

	UFUNCTION()
		void OnMontageBlendOutStartHandle(class UAnimMontage* _animMontage, bool _bInterruptted) override;



public:
#pragma region Anim Notification

	UFUNCTION(BlueprintCallable)
		void Fire() override;

	//UFUNCTION(BlueprintCallable)
	//	/** Called at the end of attack frame, and let BT know attack is finished*/
	//	void FinishAttack() override;

	//UFUNCTION(BlueprintCallable)
	//	void InitFire();


protected:



};
