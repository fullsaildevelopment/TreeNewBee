// Fill out your copyright notice in the Description page of Project Settings.

#include "ShopRow.h"
#include "UI/Gameplay/TradeMenuSlot.h"
#include "Components/HorizontalBox.h"


bool UShopRow::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		LongSword && WarAxe && Mace && CrossBow  && Armor && THWeapon
		&& BattleAxe && Hammer && GreatSword && ItemRow;

		//SHWeapon_Long && SHWeapon_Short && RangeWeapon && Armor && THWeapon 
		//&& HeavyWeapon_Axe && HeavyWeapon_Hammer && ItemRow;

	if (bAllWidgetAreGood)
	{

	}
	else
		return false;

	return true;

}


void UShopRow::SetEachSlotSize(float _width, float _height)
{
	UTradeMenuSlot* slot = nullptr;
	for (int i = 0; i < ItemRow->GetChildrenCount(); i++)
	{
		slot = Cast<UTradeMenuSlot>(ItemRow->GetChildAt(i));
		if (slot)
		{
			slot->SetSize(_width, _height);
			slot->OnShopRowInit(i);
		}
	}
}

void UShopRow::SetEachSlotAction(const TArray<struct FGearUI>& _data)
{
	UTradeMenuSlot* slot = nullptr;
	for (size_t i = 0; i < ItemRow->GetChildrenCount(); i++)
	{
		slot = Cast<UTradeMenuSlot>(ItemRow->GetChildAt(i));
		if (slot)
			slot->SetAction(_data[i]);
	}

}

//void UShopRow::SetEachSlotActionImage(const TArray<UTexture2D*>& _actionImages)
//{
//	UTradeMenuSlot* slot = nullptr;
//	for (size_t i = 0; i < ItemRow->GetChildrenCount(); i++)
//	{
//		slot = Cast<UTradeMenuSlot>(ItemRow->GetChildAt(i));
//		if (slot)
//			slot->SetImage(_actionImages[i]);
//	}
//}
