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
		class UActionSlot* SHWeapon_Long;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* SHWeapon_Short;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* RangeWeapon;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* Armor;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* THWeapon;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* HeavyWeapon_Axe;

	UPROPERTY(meta = (BindWidget))
		class UActionSlot* HeavyWeapon_Hammer;


public:

	void SetEachSlotSize(float _width, float _height);

	void SetEachSlotActionImage(const TArray<UTexture2D*>& _actionImages);
	
};
