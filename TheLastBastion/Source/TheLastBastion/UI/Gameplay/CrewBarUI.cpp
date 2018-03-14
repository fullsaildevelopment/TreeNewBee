// Fill out your copyright notice in the Description page of Project Settings.

#include "CrewBarUI.h"
#include "UI/Gameplay/CrewSlotUI.h"
#include "Components/TextBlock.h"

bool UCrewBarUI::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		Crew_1 && Crew_2 && Crew_3 && Crew_4 && 
		Crew_1_TEXT && Crew_2_TEXT && Crew_3_TEXT && Crew_4_TEXT;

	if (bAllWidgetAreGood)
	{
		mCrewBar.SetNum(4);
		mCrewBar[0].Crew = Crew_1;
		mCrewBar[0].Crew_TEXT = Crew_1_TEXT;
		mCrewBar[1].Crew = Crew_2;
		mCrewBar[1].Crew_TEXT = Crew_2_TEXT;
		mCrewBar[2].Crew = Crew_3;
		mCrewBar[2].Crew_TEXT = Crew_3_TEXT;
		mCrewBar[3].Crew = Crew_4;
		mCrewBar[3].Crew_TEXT = Crew_4_TEXT;

	}
	else
		return false;

	// init
	mCurrentIndex = -1;
	return true;
}

void UCrewBarUI::OnSelectedCrewAt(int _index)
{
	// reset previous
	if (mCurrentIndex >= 0)
	{
		mCrewBar[mCurrentIndex].Crew_TEXT->SetColorAndOpacity(FLinearColor(FColor::White));
	}
	mCurrentIndex = _index;
	mCrewBar[mCurrentIndex].Crew_TEXT->SetColorAndOpacity(FLinearColor(FColor::Green));
}

void UCrewBarUI::UpdateCrewSizeAt(int _index, int _groupSize)
{
	mCrewBar[_index].Crew->SetUnitNumber(_groupSize);
}

void UCrewBarUI::UpdateCrewInfoAt(int _index, int _groupSize, UTexture2D * _thumbNail)
{
	mCrewBar[_index].Crew->SetUnitNumber(_groupSize);
	mCrewBar[_index].Crew->SetImage(_thumbNail);


}
