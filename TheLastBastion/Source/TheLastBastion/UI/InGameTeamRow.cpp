// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameTeamRow.h"

bool UInGameTeamRow::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		Name != nullptr && Level != nullptr &&
		Class != nullptr && HpBar != nullptr && Hp != nullptr;

	if (!bAllWidgetAreGood)
	{
		UE_LOG(LogTemp, Error, TEXT("bAllWidgetAreGood is not true -- UInGameTeamRow::Initialize"));
		return false;
	}
	return true;
}

