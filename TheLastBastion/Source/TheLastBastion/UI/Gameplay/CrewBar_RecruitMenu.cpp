// Fill out your copyright notice in the Description page of Project Settings.

#include "CrewBar_RecruitMenu.h"
#include "Components/HorizontalBox.h"
#include "UI/Gameplay/CrewBlock.h"
#include "UI/Gameplay/CrewSlotUI.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/SinglePlayerGM.h"
#include "AI/AllyGroup.h"
#include "Components/TextBlock.h"




bool UCrewBar_RecruitMenu::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood = AllAllyGroups && TotalNum;

	if (bAllWidgetAreGood)
	{
	}
	else
		return false;

	int AllAllyGroupsSize = AllAllyGroups->GetChildrenCount();
	AllCrewBlock.SetNum(AllAllyGroupsSize);

	// put all in the crew block the a array
	for (int i = 0; i < AllAllyGroupsSize; i++)
	{
		UCrewBlock* crewClockTemp = Cast<UCrewBlock>(AllAllyGroups->GetChildAt(i));
		if (crewClockTemp)
		{
			AllCrewBlock[i] = crewClockTemp;
			crewClockTemp->SetAllyIndex(i);
			crewClockTemp->SetParent(this);
		}
	}

	totalAmount = 0;
	return true;
}

void UCrewBar_RecruitMenu::OnOpenRecruitMenu()
{
	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UCrewBar_RecruitMenu::OnOpenRecruitMenu"));
		return;
	}

	UCrewSlotUI *currCrewSlot = nullptr;
	AAllyGroup* currGroup = nullptr;
	totalAmount = 0;
	int currentGroupAmount = 0;

	for (int iCrew = 0; iCrew < AllCrewBlock.Num(); iCrew++)
	{

		//currCrewSlot = mCrewRow[iCrew].Crew;
		currGroup = gm->GetAllyGroupUnitAt(iCrew);
		currCrewSlot = AllCrewBlock[iCrew]->GetCrewSlot();

		AllCrewBlock[iCrew]->SetAllyIndex(iCrew);
		if (currGroup == nullptr)
		{
			AllCrewBlock[iCrew]->SetOperationEnabled(false);
			currCrewSlot->SetUnitClass(nullptr);
			currCrewSlot->SetImage(nullptr);
			AllCrewBlock[iCrew]->SetCrewNum(0);

			continue;
		}

		currCrewSlot->SetUnitClass(currGroup->GetAllyGroupClass());
		currCrewSlot->SetImage(currGroup->GetThumbNail());
		currentGroupAmount = currGroup->GetGroupSize();
		totalAmount += currentGroupAmount;
		AllCrewBlock[iCrew]->SetCrewNum(currentGroupAmount);
	}

	TotalNum->SetText(FText::AsNumber(totalAmount));
}

void UCrewBar_RecruitMenu::OnAccept()
{
	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UCrewBar_RecruitMenu::OnOpenRecruitMenu"));
		return;
	}

	UCrewSlotUI *currCrewSlot = nullptr;
	UCrewBlock * currentBlock = nullptr;
	AAllyGroup* currGroup = nullptr;
	int currentGroupAmount;
	int desiredGroupAmount;
	float deltaAmount = 0;

	for (int iCrew = 0; iCrew < AllCrewBlock.Num(); iCrew++)
	{
		currGroup = gm->GetAllyGroupUnitAt(iCrew);
		currentBlock = AllCrewBlock[iCrew];
		currCrewSlot = currentBlock->GetCrewSlot();
		if (currGroup == nullptr)
		{
			// detect this slot is no longer empty
			if (currCrewSlot->IsCrewSlotEmpty() == false)
			{
				UE_LOG(LogTemp, Warning, TEXT("Creating new Ally Group - UCrewBar_RecruitMenu::OnAccept()"));
				// Spawn Group
				gm->SpawnNewAllies(currentBlock->GetUnitClass(), 
					currentBlock->GetCrewNum(), iCrew, currCrewSlot->IsMeleeGroup());
			}
		}
		else
		{
			currentGroupAmount = currGroup->GetGroupSize();
			desiredGroupAmount = currentBlock->GetCrewNum();
			deltaAmount = desiredGroupAmount - currentGroupAmount;
			if (deltaAmount == 0)
			{
				// This group has no change, go to next group
				continue;
			}
			currGroup->OnGroupSizeChangeByNum(deltaAmount);
		}
	}
}

void UCrewBar_RecruitMenu::SetTotalAmount(int _val)
{
	totalAmount = _val;
	TotalNum->SetText(FText::AsNumber(totalAmount));
}
