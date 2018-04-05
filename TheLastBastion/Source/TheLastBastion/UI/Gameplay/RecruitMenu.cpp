// Fill out your copyright notice in the Description page of Project Settings.

#include "RecruitMenu.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "UI/Gameplay/UnitRow.h"
#include "UI/Gameplay/CrewBar_RecruitMenu.h"



bool URecruitMenu::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood = UnitList && AllMyCrew &&
		Cancel && Accept;

	if (bAllWidgetAreGood)
	{
		Accept->OnClicked.AddDynamic(this, &URecruitMenu::OnAcceptClicked);
		Cancel->OnClicked.AddDynamic(this, &URecruitMenu::OnCancelClicked);
	}
	else
		return false;


	UUnitRow* unitRow = nullptr;
	for (size_t i = 0; i < UnitList->GetChildrenCount(); i++)
	{
		unitRow = Cast<UUnitRow>(UnitList->GetChildAt(i));
		if (unitRow)
		{
			unitRow->SetEachSlotSize(96, 96);
		}
	}

	HillTribe->SetHeadingText(FText::FromString(TEXT("Hill Tribe: ")));
	HillTribe->SetEachSlotAction(HillTribe_Data);

	bIsOpened = false;
	return true;

}

void URecruitMenu::OnOpenRecruitMenu()
{
	AllMyCrew->OnOpenRecruitMenu();
	bIsOpened = true;
	
}

void URecruitMenu::OnAcceptClicked()
{
	AllMyCrew->OnAccept();


	OnCancelClicked();
}

void URecruitMenu::OnCancelClicked()
{
	this->RemoveFromParent();
	APlayerController* pc = GetOwningPlayer();
	pc->bShowMouseCursor = false;
	FInputModeGameOnly InputMode;
	pc->SetInputMode(InputMode);

	bIsOpened = false;
}

void URecruitMenu::AddWood(int _val)
{
	AllMyCrew->AddWoodValue(_val);
	CheckAcceptButtonShouldEnabled();
}

void URecruitMenu::AddFood(int _val)
{
	AllMyCrew->AddFoodValue(_val);
	CheckAcceptButtonShouldEnabled();
}

void URecruitMenu::AddMetal(int _val)
{
	AllMyCrew->AddMetalValue(_val);
	CheckAcceptButtonShouldEnabled();
}

void URecruitMenu::AddStone(int _val)
{
	AllMyCrew->AddStoneValue(_val);
	CheckAcceptButtonShouldEnabled();
}

void URecruitMenu::CheckAcceptButtonShouldEnabled()
{
	bool bIsEnabled = true;

	bIsEnabled = AllMyCrew->GetMetal_int() >= 0 && AllMyCrew->GetFood_int() >= 0 && AllMyCrew->GetWood_int() >= 0;
	Accept->SetIsEnabled(bIsEnabled);
}
