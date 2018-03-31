// Fill out your copyright notice in the Description page of Project Settings.

#include "TradeMenu_PopUp.h"
#include "CustomType.h"
#include "Combat/Gear.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "UI/Gameplay/TradePopUp_Entry.h"


TSubclassOf<UUserWidget> UTradeMenu_PopUp::WBP_TradePopUpEntry = nullptr;


UTradeMenu_PopUp::UTradeMenu_PopUp(const FObjectInitializer & ObjectInit) : Super(ObjectInit)
{
	if (WBP_TradePopUpEntry == nullptr)
	{
		UCustomType::FindClass<UUserWidget>(WBP_TradePopUpEntry, TEXT("/Game/UI/In-Game/TradeMenu/WBP_TradePopUpEntry"));
	}
}

bool UTradeMenu_PopUp::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	bool bAllWidgetAreGood = DataList && Description && GearName;

	if (bAllWidgetAreGood == false)
		return false;

	return true;
}

void UTradeMenu_PopUp::OnPopUp(TSubclassOf<class AGear> _GearClass)
{
	AGear* defaultGear = _GearClass.GetDefaultObject();
	if (defaultGear)
	{
		GearName->SetText(defaultGear->GetGearName());
		Description->SetText(defaultGear->GetGearDescription());
		
		EGearType gearType = defaultGear->GetGearType();

		switch (gearType)
		{
		case EGearType::Armor:
		case EGearType::Shield:
			PopUpArmor(defaultGear);
			break;

		case EGearType::LongSword:
		case EGearType::DoubleHandWeapon:
		case EGearType::CrossBow:
		case EGearType::GreatSword:
		case EGearType::WarAxe:
		case EGearType::BattleAxe:
			PopUpSharpWeapon(defaultGear);
			break;

		case EGearType::Mace:
		case EGearType::Hammer:
			PopUpStunWeapon(defaultGear);
			break;
		default:
			break;
		}



	}
}

void UTradeMenu_PopUp::PopUpArmor(const AGear*  const _Armor)
{
}

void UTradeMenu_PopUp::PopUpSharpWeapon(const AGear* const  _SharpWeapon)
{
	//float physicalDamage;

}

void UTradeMenu_PopUp::PopUpStunWeapon(const AGear* const  _StunWeapon)
{
}

void UTradeMenu_PopUp::PopUpCostWeapon(const AGear* const  _gear)
{
	TArray<int> Cost = _gear->GetCost();
	
	// 

}
