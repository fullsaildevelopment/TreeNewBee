// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/Hero_AnimInstance.h"

#include "RangeHero_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API URangeHero_AnimInstance : public UHero_AnimInstance
{
	GENERATED_BODY()
	
protected:

	UFUNCTION(BlueprintCallable)
		void OnBeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void OnInit() override;

	UFUNCTION(BlueprintCallable)
		/** Animation Update tick, run every frame*/
		void OnUpdate(float _deltaTime) override;

	UFUNCTION(BlueprintCallable)
		void OnPostEvaluate() override;



public:

	bool OnAttack() override;


};
