// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionSlot.h"
#include "Components/SizeBox.h"
#include "Components/Button.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#define DefaultWidth 64
#define DefaultHeight 64

bool UActionSlot::Initialize()
{

	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		SlotSize && ActionButton;

	if (bAllWidgetAreGood)
	{

	}
	else
		return false;

	ActionButton->IsFocusable = false;
	ActionButton->SetClickMethod(EButtonClickMethod::PreciseClick);
	ActionButton->SetTouchMethod(EButtonTouchMethod::PreciseTap);

	SetSize(DefaultWidth, DefaultHeight);

	return true;
}


void UActionSlot::NativeOnDragDetected(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent, UDragDropOperation *& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool UActionSlot::NativeOnDrop(const FGeometry & InGeometry, const FDragDropEvent & InDragDropEvent, UDragDropOperation * InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	return false;
}

FReply UActionSlot::NativeOnMouseButtonDown(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, FKey(TEXT("LeftMouseButton"))).NativeReply;
}

void UActionSlot::SetSize(float _width, float _height)
{
	SlotSize->SetWidthOverride(true);
	SlotSize->SetHeightOverride(true);

	SlotSize->WidthOverride = _width;
	SlotSize->HeightOverride = _height;

}


//void UActionSlot::NativeOnDragEnter(const FGeometry & InGeometry, const FDragDropEvent & InDragDropEvent, UDragDropOperation * InOperation)
//{
//	UE_LOG(LogTemp, Log, TEXT("NativeOnDragEnter"));
//
//}
//
//void UActionSlot::NativeOnDragLeave(const FDragDropEvent & InDragDropEvent, UDragDropOperation * InOperation)
//{
//	UE_LOG(LogTemp, Log, TEXT("NativeOnDragLeave"));
//}

