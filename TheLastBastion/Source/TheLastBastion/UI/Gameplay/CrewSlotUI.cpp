// Fill out your copyright notice in the Description page of Project Settings.

#include "CrewSlotUI.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/SinglePlayerGM.h"
#include "AI/AllyGroup.h"

#include "UI/UnitDrag.h"
#include "UI/Gameplay/DraggedItem.h"
#include "UI/Gameplay/RecruitMenu_PopUp.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

#include "AICharacters/TheLastBastionAIBase.h"
#include "CustomType.h"
#include "SlateBlueprintLibrary.h"


UCrewSlotUI::UCrewSlotUI(const FObjectInitializer& _objInit) : Super(_objInit)
{
	if (WBP_RecruitPopUpWidget == nullptr)
	{
		UCustomType::FindClass<UUserWidget>(WBP_RecruitPopUpWidget, TEXT("/Game/UI/In-Game/Recruit/WBP_CrewSlotPopUp"));
	}
}


bool UCrewSlotUI::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood = NumericValue != nullptr;

	if (bAllWidgetAreGood)
	{

	}
	else
		return false;

	return true;
}

void UCrewSlotUI::NativeOnDragDetected(const FGeometry & InGeometry, 
	const FPointerEvent & InMouseEvent, 
	UDragDropOperation *& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (IsDragEnabled() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("IsDragEnabled() == false - UCrewSlotUI::NativeOnDragDetected"));
		return;
	}

	if (Unit_Data.Unit_Bp && Unit_Data.Unit_Image)
	{
		UDraggedItem* dragVisual = CreateWidget<UDraggedItem>(GetOwningPlayer(), WBP_DraggedItem);
		if (dragVisual)
		{
			dragVisual->SetItemSize(SlotSize->WidthOverride, SlotSize->HeightOverride);
			dragVisual->SetImage(Unit_Data.Unit_Image);

			UUnitDrag* dragOp = Cast<UUnitDrag>(UWidgetBlueprintLibrary::CreateDragDropOperation(UUnitDrag::StaticClass()));
			if (dragOp)
			{
				dragOp->DefaultDragVisual = dragVisual;
				dragOp->unitData = Unit_Data;
				OutOperation = dragOp;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Drag and Drop fail to create - UActionSlot::NativeOnDragDetected"));
				OutOperation = nullptr;
			}
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Action Slot has null action - UActionSlot::NativeOnDragDetected"));
		OutOperation = nullptr;
	}

}

void UCrewSlotUI::NativeOnMouseEnter(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (WBP_RecruitPopUpWidget && Unit_Data.Unit_Bp != nullptr)
	{
		PopUpWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), WBP_RecruitPopUpWidget);

		FVector2D WidgetLocation = InGeometry.GetAbsolutePosition();
		FVector2D MouseLocation = InMouseEvent.GetScreenSpacePosition();

		FVector2D pixelPosition, viewPortPosition;

		USlateBlueprintLibrary::AbsoluteToViewport(GetWorld(),
			MouseLocation, pixelPosition, viewPortPosition);

		PopUpWidget->SetAlignmentInViewport(FVector2D(1.0f, 0.0));
		PopUpWidget->SetPositionInViewport(viewPortPosition);

		URecruitMenu_PopUp* PopUp = Cast<URecruitMenu_PopUp>(PopUpWidget);
		if (PopUp)
		{
			PopUp->OnPopUp(Unit_Data.Unit_Bp);
			PopUp->AddToViewport();
		}

	}

}


void UCrewSlotUI::SetUnitData(const FUnitData & _data)//, bool _bShowPrice)
{
	Unit_Data = _data;
}

void UCrewSlotUI::SetUnitData(TSubclassOf<class ATheLastBastionAIBase> _aiClassBp)
{
	if (_aiClassBp)
	{
		Unit_Data.Unit_Bp = _aiClassBp;

		ATheLastBastionAIBase* defaultAI = _aiClassBp.GetDefaultObject();

		if (defaultAI)
			Unit_Data.Unit_Image = defaultAI->GetThumbNailImage();
	}
}


bool UCrewSlotUI::IsMeleeGroup() const
{
	if (Unit_Data.Unit_Bp)
	{
		ATheLastBastionAIBase* defaultAIBase = Unit_Data.Unit_Bp.GetDefaultObject();

		if (defaultAIBase)
			return !defaultAIBase->IsRangeUnit();
		else
		{
			UE_LOG(LogTemp, Error, TEXT("defaultAIBase == nullptr, UCrewSlotUI::IsMeleeGroup()"));
			return true;
		}
	}

	return true;
}

void UCrewSlotUI::ClearNumericValue()
{
	NumericValue->SetText(FText::GetEmpty());
}

//void UCrewSlotUI::SetPrice(int _price)
//{
//	NumericValue->SetText(FText::AsNumber(_price));	
//	NumericValue->SetColorAndOpacity(FLinearColor::Yellow);
//}


void UCrewSlotUI::SetUnitNumber(int _number)
{

	NumericValue->SetText(FText::AsNumber(_number));
}

void UCrewSlotUI::OnCrewMemberDead()
{
}

void UCrewSlotUI::OnCrewSelected() {}

bool UCrewSlotUI::IsCrewSlotEmpty() const
{
	return Unit_Data.Unit_Bp == nullptr; 
}


