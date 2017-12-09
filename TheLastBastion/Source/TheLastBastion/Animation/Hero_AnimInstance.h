// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Hero_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UHero_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadOnly)
		class UCharacterMovementComponent* mMovementComp;

	UFUNCTION(BlueprintCallable)
		virtual void OnBeginPlay();

	UFUNCTION(BlueprintCallable)
		virtual void OnInit();

	UFUNCTION(BlueprintCallable)
		virtual void OnUpdate(float _deltaTime);

	UFUNCTION(BlueprintCallable)
		virtual void OnPostEvaluate();	
};
