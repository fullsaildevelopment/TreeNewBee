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

	return true;

}

void URecruitMenu::OnOpenRecruitMenu()
{
	AllMyCrew->OnOpenRecruitMenu();
}

void URecruitMenu::OnAcceptClicked()
{

	AllMyCrew->OnAccept();

	this->RemoveFromParent();
	APlayerController* pc = GetOwningPlayer();
	pc->bShowMouseCursor = false;
	FInputModeGameOnly InputMode;
	pc->SetInputMode(InputMode);

}

void URecruitMenu::OnCancelClicked()
{
	this->RemoveFromParent();
	APlayerController* pc = GetOwningPlayer();
	pc->bShowMouseCursor = false;
	FInputModeGameOnly InputMode;
	pc->SetInputMode(InputMode);
}
