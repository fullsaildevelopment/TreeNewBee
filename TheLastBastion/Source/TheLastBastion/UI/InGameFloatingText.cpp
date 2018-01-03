// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameFloatingText.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"


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
	else
		return false;

	

}

void UInGameFloatingText::SetInGameFTProperty(FText _val)
{
	InGameFT->SetText(_val);
}


