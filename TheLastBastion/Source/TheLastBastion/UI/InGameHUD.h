// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameHUD.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UInGameHUD : public UUserWidget
{
	GENERATED_BODY()
	
	
protected:

	virtual bool Initialize();

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* GoldValue;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WoodValue;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MetalValue;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* RockValue;


	UPROPERTY(meta = (BindWidget))
		class UTextBlock* HpStats;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* SpStats;


private:

	UPROPERTY()
		class UGI_TheLastBastion* mGameInstanceRef;

	UPROPERTY()
		class ATheLastBastionHeroCharacter* mHeroCharacter;


public:

	void SetHpStats(float current, float max);

	void SetSpStats(float current, float max);
	
};
