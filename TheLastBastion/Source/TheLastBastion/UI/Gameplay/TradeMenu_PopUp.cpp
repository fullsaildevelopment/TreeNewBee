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
	if (defaultGear && WBP_TradePopUpEntry)
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

		PopUpCostWeapon(defaultGear);
	}
}

void UTradeMenu_PopUp::PopUpArmor(const AGear*  const _Armor)
{

}

void UTradeMenu_PopUp::PopUpSharpWeapon(const AGear* const  _SharpWeapon)
{
	//float physicalDamage;
	float physicalDamage = _SharpWeapon->GetPhysicalDamage();
	AddPopUpEntry(TEXT("Physical Damage: "), physicalDamage);

	float CriticalChance = _SharpWeapon->GetCriticalChance();
	AddPopUpEntryWithColor(CriticalHit_FTColor, TEXT("Critical Chance(%): "), CriticalChance);

}

void UTradeMenu_PopUp::PopUpStunWeapon(const AGear* const  _StunWeapon)
{
	PopUpSharpWeapon(_StunWeapon);
	float stunChance = _StunWeapon->GetStunChance();
	AddPopUpEntryWithColor(StunHit_FTColor, TEXT("Stun Chance(%): "), stunChance);
}

void UTradeMenu_PopUp::PopUpCostWeapon(const AGear* const  _gear)
{
	TArray<int> Cost = _gear->GetResourceCost();

	// Metal 

	int MetalCost = Cost[0];
	if (MetalCost > 0)
		AddPopUpEntryWithColor(MetalUI_Color, TEXT("Metal Require:"), MetalCost);

	// Wood 
	int WoodCost = Cost[1];
	if (WoodCost > 0)
		AddPopUpEntryWithColor(WoodUI_Color, TEXT("Wood Require:"), WoodCost);

}

void UTradeMenu_PopUp::AddPopUpEntry(FString _text, int _val)
{

	UTradePopUp_Entry*	popUpEntry =
		Cast<UTradePopUp_Entry>(CreateWidget<UUserWidget>(GetOwningPlayer(), WBP_TradePopUpEntry));

	if (popUpEntry)
	{
		popUpEntry->EditEntry(FText::FromString(_text), FText::AsNumber(_val));
	}
	
	DataList->AddChildToVerticalBox(popUpEntry);
}

void UTradeMenu_PopUp::AddPopUpEntryWithColor(FLinearColor _color, FString _text, int _val)
{
	UTradePopUp_Entry*	popUpEntry =
		Cast<UTradePopUp_Entry>(CreateWidget<UUserWidget>(GetOwningPlayer(), WBP_TradePopUpEntry));

	if (popUpEntry)
	{
		popUpEntry->EditEntry(FText::FromString(_text), FText::AsNumber(_val));
		popUpEntry->EditColor(_color);
	}

	DataList->AddChildToVerticalBox(popUpEntry);

}
