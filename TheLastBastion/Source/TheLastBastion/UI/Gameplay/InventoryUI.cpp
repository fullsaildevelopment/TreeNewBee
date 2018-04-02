// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryUI.h"
#include "UI/Gameplay/TradeMenuSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

#include "Combat/HeroStatsComponent.h"
#include "Combat/Gear.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameMode/SinglePlayerGM.h"
#include "Kismet/GameplayStatics.h"

bool UInventoryUI::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood = GearRow &&
		SHWeapon && RangeWeapon && Armor && THWeapon && HeavyWeapon &&
		WoodValue && FoodValue && StoneValue && MetalValue;

	if (bAllWidgetAreGood)
	{

	}
	else
		return false;

	UTradeMenuSlot* slot = nullptr;
	for (int i = 0; i < GearRow->GetChildrenCount(); i++)
	{
		slot = Cast<UTradeMenuSlot>(GearRow->GetChildAt(i));
		if (slot)
		{
			slot->SetSize(150.0f, 150.0f);
			slot->OnInventoryInit(i);
		}
	}

	Wood = 0; Food = 0; Stone = 0; Metal = 0;
	return true;
}

void UInventoryUI::OnOpenTradeMenu(UHeroStatsComponent * _heroStats)
{
	// Check Armor
	AGear* currentArmor = _heroStats->GetCurrentArmor();
	if (currentArmor)
	{
		Armor->SetImage(currentArmor->GetThumbNailImage());
		Armor->SetGearBp(currentArmor->GetClass());
	}

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
					SHWeapon->SetImage(weapon->GetThumbNailImage());
					break;
				case EGearType::BattleAxe:
				case EGearType::Hammer:
				case EGearType::GreatSword:
					HeavyWeapon->SetGearBp(weaponClass);
					HeavyWeapon->SetImage(weapon->GetThumbNailImage());
					break;
				case EGearType::CrossBow:
					RangeWeapon->SetGearBp(weaponClass);
					RangeWeapon->SetImage(weapon->GetThumbNailImage());
					break;
				case EGearType::DoubleHandWeapon:
					THWeapon->SetGearBp(weaponClass);
					THWeapon->SetImage(weapon->GetThumbNailImage());
					break;
			}
		}
	}

	// Load Resource
	
	LoadResource();

}

void UInventoryUI::OnTradeMenuAccept() const
{
	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("gm == nullptr,  UInventoryUI::OnTradeMenuAccept"));
		return;
	}

	gm->OnTradeMenuAccept(Metal, Wood);

}

TSubclassOf<class AGear> UInventoryUI::GetGearClassAt(int _index) const
{
	UTradeMenuSlot* slot  = Cast<UTradeMenuSlot>(GearRow->GetChildAt(_index));
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

void UInventoryUI::LoadResource()
{
	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gm == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("gm == nullptr, UInventoryUI::LoadResource"));
		return;
	}

	Wood = gm->GetWoodTotal();
	WoodValue->SetText(FText::AsNumber(Wood));
	Food = gm->GetFoodTotal();
	FoodValue->SetText(FText::AsNumber(Food));
	Stone = gm->GetStoneTotal();
	StoneValue->SetText(FText::AsNumber(Stone));
	Metal = gm->GetMetalTotal();
	MetalValue->SetText(FText::AsNumber(Metal));
}

void UInventoryUI::AddWoodValue(int _val)
{
	Wood += _val;
	WoodValue->SetText(FText::AsNumber(Wood));
}

void UInventoryUI::AddFoodValue(int _val)
{
	Food += _val;
	FoodValue->SetText(FText::AsNumber(Food));
}

void UInventoryUI::AddMetalValue(int _val)
{
	Metal += _val;
	MetalValue->SetText(FText::AsNumber(Metal));
}

void UInventoryUI::AddStoneValue(int _val)
{
	Stone += _val;
	StoneValue->SetText(FText::AsNumber(Stone));
}
