// Fill out your copyright notice in the Description page of Project Settings.

#include "CrewSlotUI.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"



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
	UE_LOG(LogTemp, Log, TEXT("UCrewSlotUI::NativeOnDragDetected"));
}

bool UCrewSlotUI::NativeOnDrop(const FGeometry & InGeometry, 
	const FDragDropEvent & InDragDropEvent, 
	UDragDropOperation * InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	UE_LOG(LogTemp, Log, TEXT("UCrewSlotUI::NativeOnDrop"));
	return false;
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

void UCrewSlotUI::OnCrewSelected()
{
}

