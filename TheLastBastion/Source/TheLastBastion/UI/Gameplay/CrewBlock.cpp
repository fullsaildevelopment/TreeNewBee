// Fill out your copyright notice in the Description page of Project Settings.

#include "CrewBlock.h"
#include "UI/Gameplay/CrewSlotUI.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UI/Gameplay/CrewBar_RecruitMenu.h"

bool UCrewBlock::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		CrewSlot && AllyIndex_Text && CrewNum_Text 
		&& Plus && Minus && Dismiss && FastRecruit;

	if (bAllWidgetAreGood)
	{
		Dismiss->OnClicked.AddDynamic(this, &UCrewBlock::OnDismissClick);
		Plus->OnClicked.AddDynamic(this, &UCrewBlock::OnPlusClick);
		Minus->OnClicked.AddDynamic(this, &UCrewBlock::OnMinusClick);
		FastRecruit->OnClicked.AddDynamic(this, &UCrewBlock::OnFastRecruitClick);
		CrewSlot->SetIsButton(true);
		CrewSlot->SetSize(96, 96);
	}
	else
		return false;

	// init
	AllyIndex = -1;
	return true;
}

void UCrewBlock::OnPlusClick()
{
	int crewNumTemp = CrewNum;
	CrewNum++;
	int TotalAmount = mCrewBar->GetTotalAmount();

	int maxCrewNum = MaxAlliesNum - TotalAmount + crewNumTemp;
	maxCrewNum = (maxCrewNum >= 20) ? 20 : maxCrewNum;
	//UE_LOG(LogTemp, Log, TEXT("%d - UCrewBlock::OnPlusClick"), maxCrewNum)

	CrewNum = FMath::Clamp(CrewNum, 0, maxCrewNum);
	if (crewNumTemp == CrewNum)
		return;

	TotalAmount++;
	mCrewBar->SetTotalAmount(TotalAmount);
	CrewNum_Text->SetText(FText::AsNumber(CrewNum));
}

void UCrewBlock::OnMinusClick()
{

	int TotalAmount = mCrewBar->GetTotalAmount();
	TotalAmount -= CrewNum;

	CrewNum--;
	if (CrewNum < 0)
		CrewNum = 0;

	TotalAmount += CrewNum;

	mCrewBar->SetTotalAmount(TotalAmount);
	CrewNum_Text->SetText(FText::AsNumber(CrewNum));
}

void UCrewBlock::OnFastRecruitClick()
{
	int crewNumTemp = CrewNum;

	int TotalAmount = mCrewBar->GetTotalAmount();
	int TotalRemain = TotalAmount - crewNumTemp;

	int maxCrewNum = MaxAlliesNum - TotalRemain;
	maxCrewNum = (maxCrewNum >= 20) ? 20 : maxCrewNum;

	CrewNum = maxCrewNum;
	TotalAmount = TotalRemain + maxCrewNum;

	mCrewBar->SetTotalAmount(TotalAmount);
	CrewNum_Text->SetText(FText::AsNumber(CrewNum));

}

void UCrewBlock::OnDismissClick()
{
	if (CrewNum == 0)
		return;

	int TotalAmount = mCrewBar->GetTotalAmount();
	TotalAmount -= CrewNum;
	CrewNum = 0;

	mCrewBar->SetTotalAmount(TotalAmount);
	CrewNum_Text->SetText(FText::AsNumber(CrewNum));
}

void UCrewBlock::SetCrewNum(int _groupSize)
{
	CrewNum = _groupSize;
	CrewNum_Text->SetText(FText::AsNumber(_groupSize));
}
