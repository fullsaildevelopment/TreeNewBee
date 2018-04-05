// Fill out your copyright notice in the Description page of Project Settings.

#include "CrewBlock.h"
#include "UI/Gameplay/CrewSlotUI.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UI/Gameplay/CrewBar_RecruitMenu.h"
#include "UI/Gameplay/RecruitMenu.h"
#include "UI/UnitDrag.h"
#include "GameMode/SinglePlayerGM.h" 
#include "Kismet/GameplayStatics.h"

#include "AICharacters/TheLastBastionAIBase.h"
#include "PCs/SinglePlayerPC.h"

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
		CrewSlot->SetDragDropMode(EDragDropMode::EDropOnly);
		CrewSlot->SetSize(96, 96);
	}
	else
		return false;

	// init
	AllyIndex = -1;
	return true;
}

bool UCrewBlock::NativeOnDrop(const FGeometry & InGeometry, const FDragDropEvent & InDragDropEvent, UDragDropOperation * InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (CrewSlot->IsDropEnabled() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("CrewSlot->IsDropEnabled() == false - UCrewBlock::NativeOnDrop"));
		return false;
	}

	UUnitDrag* unitDragOp = Cast<UUnitDrag>(InOperation);

	if (unitDragOp && CrewSlot -> IsCrewSlotEmpty())
	{
		OnAddingNewUnit(unitDragOp);
	}

	return false;
}

void UCrewBlock::OnPlusClick()
{
	int crewNumTemp = CrewNum;
	UpdateResourcePreview(1);
	CrewNum++;
	if (CrewNum > 3)
		Minus->SetIsEnabled(true);

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

	UpdateResourcePreview(1);
}

void UCrewBlock::OnMinusClick()
{


	int TotalAmount = mCrewBar->GetTotalAmount();
	TotalAmount -= CrewNum;
	UpdateResourcePreview(-1);

	CrewNum--;
	if (CrewNum <= 3)
	{
		CrewNum = 3;
		Minus->SetIsEnabled(false);
	}

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

	// pay
	UpdateResourcePreview(maxCrewNum - crewNumTemp);


	CrewNum = maxCrewNum;
	TotalAmount = TotalRemain + maxCrewNum;

	mCrewBar->SetTotalAmount(TotalAmount);
	CrewNum_Text->SetText(FText::AsNumber(CrewNum));

}

void UCrewBlock::OnDismissClick()
{
	if (CrewNum == 0)
		return;


	// update total amount on crew bar
	int TotalAmount = mCrewBar->GetTotalAmount();
	TotalAmount -= CrewNum;
	mCrewBar->SetTotalAmount(TotalAmount);

	// Refund 
	UpdateResourcePreview(-CrewNum);

	// Clear crew slot
	CrewNum = 0;
	CrewSlot->SetImage(nullptr);
	CrewSlot->SetUnitClass(nullptr);
	CrewNum_Text->SetText(FText::AsNumber(CrewNum));


	// Kill the group
	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm)
	{
		gm->DestroyAllyGroupAt(AllyIndex);
	}
	SetOperationEnabled(false);

}

void UCrewBlock::OnAddingNewUnit(const UUnitDrag * unitDragOp)
{
	int TotalAmount = mCrewBar->GetTotalAmount();

	// no room for our new unit
	if (TotalAmount >= MaxAlliesNum)
	{
		UE_LOG(LogTemp, Warning, TEXT("no room for our new unit - UCrewBlock::OnAddingNewUnit"));
		return;
	}

	int remainRoom = MaxAlliesNum - TotalAmount;

	int numCanJoin = (remainRoom < StartingNum_EachCrew)
		? remainRoom : StartingNum_EachCrew;

	SetCrewNum(numCanJoin);

	TotalAmount += CrewNum;

	mCrewBar->SetTotalAmount(TotalAmount);
	CrewSlot->SetUnitData(unitDragOp->unitData);

	SetOperationEnabled(true);

	if (CrewNum <= MinUnitsNum_EachCrew)
		Minus->SetIsEnabled(false);


	UpdateResourcePreview(numCanJoin);
}

void UCrewBlock::SetAllyIndex(int _index)
{
	AllyIndex = _index;
	AllyIndex_Text->SetText(FText::AsNumber(_index));
}

TSubclassOf<class ATheLastBastionAIBase> UCrewBlock::GetUnitClass() const
{
	return CrewSlot->GetUnitClass();
}

void UCrewBlock::SetCrewNum(int _groupSize)
{
	CrewNum = _groupSize;
	CrewNum_Text->SetText(FText::AsNumber(_groupSize));
}

void UCrewBlock::SetOperationEnabled(int _enabled)
{
	Plus->SetIsEnabled(_enabled); 
	Minus->SetIsEnabled(_enabled);
	Dismiss->SetIsEnabled(_enabled);
	FastRecruit->SetIsEnabled(_enabled);
}

void UCrewBlock::UpdateResourcePreview(int _num)//, TSubclassOf<ATheLastBastionAIBase> _unitClass)
{
	
	ATheLastBastionAIBase* defaultAI = GetUnitClass().GetDefaultObject();
	if (defaultAI)
	{
		TArray<int> cost = defaultAI->GetResourceCost();
		// check food
		int requiredFood = cost[0] * _num;
		int requiredMetal = cost[1] * _num;
		int requiredWood = cost[2] * _num;

		mCrewBar->AddFoodValue(-requiredFood);
		mCrewBar->AddMetalValue(-requiredMetal);
		mCrewBar->AddWoodValue(-requiredWood);

		// Check apply button availity
		ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(GetOwningPlayer());
		if (pc == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("pc == nullptr, UCrewBlock::UpdateResourcePreview"));
			return;
		}
		else
		{
			URecruitMenu* recruitMenu = pc->GetRecruitMenu();
			if (recruitMenu == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("recruitMenu == nullptr, UCrewBlock::UpdateResourcePreview"));
				return;
			}
			else
			{
				recruitMenu->CheckAcceptButtonShouldEnabled();
			}
		}
	
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("defaultAI == nullptr, UCrewBlock::HasEnoughResource"));
	}

}
