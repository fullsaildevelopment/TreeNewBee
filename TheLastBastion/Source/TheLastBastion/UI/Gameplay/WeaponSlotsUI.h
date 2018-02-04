// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponSlotsUI.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UWeaponSlotsUI : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		UTexture2D* CurrentWeapon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		UTexture2D* Katana;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		UTexture2D* RangeWeapon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		UTexture2D* SingleHandWeapon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		UTexture2D* HeavyWeapon;

public:

	void OnInitStats(const class UHeroStatsComponent * _heroStats);

	void SetCurrentWeaponImage(const class AGear* _gear);
};
