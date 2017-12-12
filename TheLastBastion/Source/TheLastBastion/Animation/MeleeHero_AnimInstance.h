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


private:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Action, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* attack;


};
