// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/Hero_AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RangeHero_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API URangeHero_AnimInstance : public UHero_AnimInstance
{
	GENERATED_BODY()
	


public:

	URangeHero_AnimInstance(const FObjectInitializer& _objectInitalizer);

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

	void OnAttack() override;

	/** Called when equip button is pressed  */
	bool OnEquip() override;

	void OnRightMouseButtonPressed() override;

	void OnRightMouseButtonReleased() override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fire)
	bool bTryToZoomIn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fire)
	/** FOV blend rate zoom in and zoom out*/
	float CameraZoomInRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fire)
	/** Camera blend rate between travel mode and crossbow mode*/
	float CameraShiftRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fire)
		/** Camera relative location during equip */
	FVector CameraEquipOffset; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Fire)		
		/** Camera relative location during Zoom In */
	FVector CameraZoomOffset;


private:

};
