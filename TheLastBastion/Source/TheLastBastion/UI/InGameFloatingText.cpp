// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameFloatingText.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "CustomType.h"

UInGameFloatingText::UInGameFloatingText(const FObjectInitializer & ObjectInit) : Super(ObjectInit)
{

}

bool UInGameFloatingText::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	if (InGameFT == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("InGameFT is NULL --  UInGameFloatingText::Initialize"));
		return false;
	}

	InGameFT->Font.Size = 20;
	return true;

}

void UInGameFloatingText::SetInGameFTProperty(FText _val)
{
	InGameFT->SetText(_val);
	
	//InGameFT->
	//InGameFT->SetColorAndOpacity()
}

void UInGameFloatingText::SetStyle(EFloatingTextStyle _val)
{
	switch (_val)
	{
	case EFloatingTextStyle::Default:
	default:
		break;
	case EFloatingTextStyle::Enemy:
		InGameFT->SetColorAndOpacity(EnemyHit_FTColor);
		break;
	case EFloatingTextStyle::Fire:
		InGameFT->SetColorAndOpacity(FireHit_FTColor);
		break;
	case EFloatingTextStyle::Ice:
		InGameFT->SetColorAndOpacity(IceHit_FTColor);
		break;
	case EFloatingTextStyle::Critical:
		InGameFT->SetColorAndOpacity(CriticalHit_FTColor);
		break;
	case EFloatingTextStyle::Stun:
		InGameFT->SetColorAndOpacity(StunHit_FTColor);
		break;
	}
}

void UInGameFloatingText::SetFontSize(float _val)
{
	FSlateFontInfo font = InGameFT->Font;
	font.Size = _val;

	InGameFT->SetFont(font);
}


