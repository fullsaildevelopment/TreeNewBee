// Fill out your copyright notice in the Description page of Project Settings.

#include "AllyGroupHUD.h"

#include "Components/TextBlock.h"


bool UAllyGroupHUD::Initialize()
{
	if (Super::Initialize() == false)
	{
		return false;
	}

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood = GroupIndexText != nullptr;

	if (bAllWidgetAreGood)
	{

	}
	else
		return false;

	return true;
}


void UAllyGroupHUD::SetGroupIndexText(int _groupIndex)
{
	GroupIndexText->SetText(FText::AsNumber(_groupIndex));
}
