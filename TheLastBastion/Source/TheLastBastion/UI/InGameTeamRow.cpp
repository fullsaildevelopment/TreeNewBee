// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameTeamRow.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "GI_TheLastBastion.h"


bool UInGameTeamRow::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		Name != nullptr && Level != nullptr &&
		HeroClass != nullptr && HpBar != nullptr && Hp != nullptr;

	if (!bAllWidgetAreGood)
	{
		UE_LOG(LogTemp, Error, TEXT("bAllWidgetAreGood is not true -- UInGameTeamRow::Initialize"));
		return false;
	}
	return true;
}

void UInGameTeamRow::InitHeader(const FPlayerProfile & _memberProfile)
{	
	Name->SetText(_memberProfile.mPlayerName);
	HeroClass->SetText((_memberProfile.bIsRangerClass) ? FText::FromString(TEXT("Ranger")) : FText::FromString(TEXT("Builder")));
}
