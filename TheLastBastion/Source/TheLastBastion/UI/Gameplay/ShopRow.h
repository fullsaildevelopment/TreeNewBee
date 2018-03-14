// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopRow.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UShopRow : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	bool Initialize() override;

protected:

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* ItemRow;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* LongSword;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* WarAxe;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* Mace;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* CrossBow;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* Armor;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* THWeapon;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* BattleAxe;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* Hammer;

	UPROPERTY(meta = (BindWidget))
		class UTradeMenuSlot* GreatSword;


public:

	void SetEachSlotSize(float _width, float _height);

	void SetEachSlotAction(const TArray<struct FGearUI>& _data);

	//void SetEachSlotActionImage(const TArray<UTexture2D*>& _actionImages);	
};
