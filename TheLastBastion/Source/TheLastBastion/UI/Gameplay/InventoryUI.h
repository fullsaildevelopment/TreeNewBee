// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryUI.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UInventoryUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	bool Initialize() override;

protected:

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* SHWeapon;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* RangeWeapon;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* Armor;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* THWeapon;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* HeavyWeapon;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* Additive_0;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* Additive_1;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* Additive_2;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* Additive_3;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* Additive_4;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* Additive_5;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* GearRow;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* AdditiveRow;


public:

	void OnOpenTradeMenu(class UHeroStatsComponent * _heroStats);

	
	
};
