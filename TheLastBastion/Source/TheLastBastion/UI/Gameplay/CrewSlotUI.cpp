// Fill out your copyright notice in the Description page of Project Settings.

#include "CrewSlotUI.h"




bool UCrewSlotUI::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood = CrewSize != nullptr;

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
