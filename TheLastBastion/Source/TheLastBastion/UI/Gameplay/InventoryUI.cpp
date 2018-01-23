// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryUI.h"
#include "UI/ActionSlot.h"
#include "Components/HorizontalBox.h"
#include "Combat/HeroStatsComponent.h"
#include "Combat/Gear.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

bool UInventoryUI::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood = GearRow && AdditiveRow &&
		SHWeapon && RangeWeapon && Armor && THWeapon && HeavyWeapon &&
		Additive_0 && Additive_1 && Additive_2 && Additive_3 && Additive_4 && Additive_5;

	if (bAllWidgetAreGood)
	{

	}
	else
		return false;

	UActionSlot* slot = nullptr;
	for (int i = 0; i < GearRow->GetChildrenCount(); i++)
	{
		slot = Cast<UActionSlot>(GearRow->GetChildAt(i));
		if (slot)
			slot->SetSize(150.0f, 150.0f);
	}

	return true;
}

void UInventoryUI::OnOpenTradeMenu(UHeroStatsComponent * _heroStats)
{
	// Check Armor
	AGear* currentArmor = _heroStats->GetCurrentArmor();
	if (currentArmor)
		Armor->SetActionImage(currentArmor->GetThumbNailImage());

	// Check WeaponSlot
	AGear* weapon = nullptr;
	
	//int numOfSlot = _heroStats->GetMaxNumOfWeaponSlot();

	// Use constant 2 for now
	for (int i = 0; i < 2; i++)
	{
		weapon = _heroStats->GetWeaponSlotAt(i).RightHand;
		if (weapon)
		{
			switch (weapon->GetGearType())
			{
				case EGearType::SingleHandWeapon:
					SHWeapon->SetActionImage(weapon->GetThumbNailImage());
					break;
				case EGearType::CrossBow:
					THWeapon->SetActionImage(weapon->GetThumbNailImage());
					break;
				case EGearType::DoubleHandWeapon:
					RangeWeapon->SetActionImage(weapon->GetThumbNailImage());
					break;
			}

		}
	}

}
