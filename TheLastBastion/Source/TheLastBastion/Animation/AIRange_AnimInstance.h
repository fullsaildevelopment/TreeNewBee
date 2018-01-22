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

	UFUNCTION(BlueprintCallable)
		void Fire(const AActor * _target) override;

	UFUNCTION(BlueprintCallable)
		void StopFire() override;

	void OnBeingHit(float _damage, FName boneName,
		const FVector& _shotFromDirection, const FVector& _hitLocation, const class UPawnStatsComponent* _pawnStats) override;


	//UFUNCTION(BlueprintCallable)
	//	/** Called at the end of attack frame, and let BT know attack is finished*/
	//	void FinishAttack() override;

	//UFUNCTION(BlueprintCallable)
	//	void InitFire();


protected:



};
