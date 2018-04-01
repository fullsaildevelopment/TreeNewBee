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
		class UTradeMenuSlot* SHWeapon;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* RangeWeapon;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* Armor;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* THWeapon;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* HeavyWeapon;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WoodValue;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* FoodValue;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* StoneValue;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MetalValue;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* GearRow;

	int Wood, Food, Stone, Metal;

public:

	void OnOpenTradeMenu(class UHeroStatsComponent * _heroStats);

	TSubclassOf<class AGear> GetGearClassAt(int _index) const;
	TSubclassOf<class AGear> GetCurrentSHWeapon() const; 
	TSubclassOf<class AGear> GetCurrentTHWeapon() const;    
	TSubclassOf<class AGear> GetCurrentHeavyWeapon() const;
	TSubclassOf<class AGear> GetCurrentRangeWeapon() const; 
	TSubclassOf<class AGear> GetCurrentArmor() const;    

	/** Load the resource from Game Mode*/
	void LoadResource();
};
