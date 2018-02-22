// Fill out your copyright notice in the Description page of Project Settings.

#include "CrewSlotUI.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/SinglePlayerGM.h"
#include "AI/AllyGroup.h"
#include "UI/UnitDrag.h"
#include "UI/Gameplay/DraggedItem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"



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


void UCrewSlotUI::SetPrice(int _price)
{
	NumericValue->SetText(FText::AsNumber(_price));	
}

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


