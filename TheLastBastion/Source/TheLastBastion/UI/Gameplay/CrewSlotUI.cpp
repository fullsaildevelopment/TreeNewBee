// Fill out your copyright notice in the Description page of Project Settings.

#include "CrewSlotUI.h"
#include "Components/SizeBox.h"



bool UCrewSlotUI::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood = NumericValue != nullptr;

	if (bAllWidgetAreGood)
	{

	}
	else
		return false;

	return true;
}

void UCrewSlotUI::OnCrewMemberDead()
{
}

void UCrewSlotUI::OnCrewSelected()
{
}

