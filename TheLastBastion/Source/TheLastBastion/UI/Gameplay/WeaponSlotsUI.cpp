// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponSlotsUI.h"
#include "Combat/HeroStatsComponent.h"
#include "Combat/Gear.h"

bool UWeaponSlotsUI::Initialize()
{
	if (Super::Initialize() == false)
	{
		return false;
	}
	return true;
}

void UWeaponSlotsUI::OnInitStats(const UHeroStatsComponent * _heroStats)
{
	AGear* gear = nullptr;
	for (int iGear = 0; iGear < _heroStats->GetMaxNumOfWeaponSlot(); iGear++)
	{
		gear = _heroStats->GetWeaponSlotAt(iGear).RightHand;
		if (gear == nullptr)
			continue;
		switch (gear->GetGearType())
		{
		case EGearType::CrossBow:
		case EGearType::Bow:
			RangeWeapon = gear->GetThumbNailImage();
			break;
		case EGearType::LongSword:
		case EGearType::Mace:
		case EGearType::WarAxe:
			SingleHandWeapon = gear->GetThumbNailImage();
			break;
		case EGearType::GreatSword:
		case EGearType::BattleAxe:
		case EGearType::Hammer:
			HeavyWeapon = gear->GetThumbNailImage();
			break; 
		case EGearType::DoubleHandWeapon:
			Katana = gear->GetThumbNailImage();
			break;
		default:
			break;
		}
	}

	gear = _heroStats->GetCurrentRightHandWeapon();
	if (gear)
		CurrentWeapon = gear->GetThumbNailImage();
}

void UWeaponSlotsUI::SetCurrentWeaponImage(const AGear * _gear)
{
	if (_gear != nullptr)
	{
		CurrentWeapon = _gear->GetThumbNailImage();
	}
}

