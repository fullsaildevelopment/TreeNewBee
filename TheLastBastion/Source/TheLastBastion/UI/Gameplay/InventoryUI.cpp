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
		{
			slot->SetSize(150.0f, 150.0f);
			slot->OnInventoryInit(i);
		}
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
	
	for (int i = 0; i <  _heroStats->GetMaxNumOfWeaponSlot(); i++)
	{
		TSubclassOf<AGear> weaponClass = _heroStats->GetWeaponSlotAt(i).WeaponClass;
		weapon = _heroStats->GetWeaponSlotAt(i).RightHand;
		if (weapon)
		{
			switch (weapon->GetGearType())
			{
				case EGearType::WarAxe:
				case EGearType::LongSword:
				case EGearType::Mace:
					SHWeapon->SetGearBp(weaponClass);
					SHWeapon->SetActionImage(weapon->GetThumbNailImage());
					break;
				case EGearType::BattleAxe:
				case EGearType::Hammer:
				case EGearType::GreatSword:
					HeavyWeapon->SetGearBp(weaponClass);
					HeavyWeapon->SetActionImage(weapon->GetThumbNailImage());
					break;
				case EGearType::CrossBow:
					RangeWeapon->SetGearBp(weaponClass);
					RangeWeapon->SetActionImage(weapon->GetThumbNailImage());
					break;
				case EGearType::DoubleHandWeapon:
					THWeapon->SetGearBp(weaponClass);
					THWeapon->SetActionImage(weapon->GetThumbNailImage());
					break;
			}
		}
	}

}

TSubclassOf<class AGear> UInventoryUI::GetGearClassAt(int _index) const
{
	UActionSlot* slot  = Cast<UActionSlot>(GearRow->GetChildAt(_index));
	if (slot)
		return slot->GetGearClass();
	return nullptr;
}

TSubclassOf<class AGear> UInventoryUI::GetCurrentSHWeapon() const
{
	return SHWeapon->GetGearClass();
}

TSubclassOf<class AGear> UInventoryUI::GetCurrentTHWeapon() const
{
	return THWeapon->GetGearClass();
}

TSubclassOf<class AGear> UInventoryUI::GetCurrentHeavyWeapon() const
{
	return HeavyWeapon->GetGearClass();
}

TSubclassOf<class AGear> UInventoryUI::GetCurrentRangeWeapon() const
{
	return RangeWeapon->GetGearClass();
}

TSubclassOf<class AGear> UInventoryUI::GetCurrentArmor() const
{
	return Armor->GetGearClass();
}
