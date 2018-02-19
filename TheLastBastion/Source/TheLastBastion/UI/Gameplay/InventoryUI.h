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
		class UTradeMenuSlot* Additive_0;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* Additive_1;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* Additive_2;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* Additive_3;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* Additive_4;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* Additive_5;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* GearRow;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* AdditiveRow;


public:

	void OnOpenTradeMenu(class UHeroStatsComponent * _heroStats);



	TSubclassOf<class AGear> GetGearClassAt(int _index) const;
	TSubclassOf<class AGear> GetCurrentSHWeapon() const; 
	TSubclassOf<class AGear> GetCurrentTHWeapon() const;    
	TSubclassOf<class AGear> GetCurrentHeavyWeapon() const;
	TSubclassOf<class AGear> GetCurrentRangeWeapon() const; 
	TSubclassOf<class AGear> GetCurrentArmor() const;    
	
};
