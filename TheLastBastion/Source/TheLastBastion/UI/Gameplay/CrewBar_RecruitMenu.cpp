// Fill out your copyright notice in the Description page of Project Settings.

#include "CrewBar_RecruitMenu.h"
#include "UI/Gameplay/CrewSlotUI.h"




bool UCrewBar_RecruitMenu::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood
		= Crew_1 && Crew_2 && Crew_3 && Crew_4 &&
		Crew_1_Num && Crew_2_Num && Crew_3_Num && Crew_4_Num && TotalNum;

	if (bAllWidgetAreGood)
	{
	}
	else
		return false;

	mCrewRow.SetNum(4);
	mCrewRow[0].Crew = Crew_1;
	mCrewRow[0].Crew_Num = Crew_1_Num;
	mCrewRow[1].Crew = Crew_2;
	mCrewRow[1].Crew_Num = Crew_2_Num;
	mCrewRow[2].Crew = Crew_3;
	mCrewRow[2].Crew_Num = Crew_3_Num;
	mCrewRow[3].Crew = Crew_4;
	mCrewRow[3].Crew_Num = Crew_4_Num;

	for (int i = 0; i < mCrewRow.Num(); i++)
	{
		mCrewRow[i].Crew->SetSize(96, 96);
	}

	return true;
}

void UCrewBar_RecruitMenu::OnOpenCrewMenu()
{
}
