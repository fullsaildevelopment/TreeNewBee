// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionSlot.h"
#include "Components/SizeBox.h"
#include "Components/Button.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/Gameplay/InventoryUI.h"
#include "UI/ItemDrag.h"
#include "UI/Gameplay/DraggedItem.h"
#include "CustomType.h"


#define DefaultWidth 64
#define DefaultHeight 64

TSubclassOf<UUserWidget> UActionSlot::WBP_DraggedItem = nullptr;


UActionSlot::UActionSlot(const FObjectInitializer& _objInit) :Super(_objInit)
{
	if (WBP_DraggedItem == nullptr)
	{
		UCustomType::FindClass<UUserWidget>(WBP_DraggedItem, TEXT("/Game/UI/In-Game/WBP_DraggedItem"));
	}	
}

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

	if (bFromUpgrade == false)
	{
		OutOperation = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Drag is not available for inventory Action Slot - UActionSlot::NativeOnDragDetected"));
		return;
	}

	if (GearUI.Gear_Bp && GearUI.Gear_Image)
	{

		UDraggedItem* dragVisual = CreateWidget<UDraggedItem>(GetOwningPlayer(), WBP_DraggedItem);
		if (dragVisual)
		{
			dragVisual->SetItemSize(SlotSize->WidthOverride, SlotSize->HeightOverride);
			dragVisual->SetImage(GearUI.Gear_Image);

			UItemDrag* dragOp =Cast<UItemDrag>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemDrag::StaticClass()));
			if (dragOp)
			{
				dragOp->DefaultDragVisual = dragVisual;
				dragOp->GearUI = GearUI;
				dragOp->UpgradeGearType = UpgradeGearType;
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
		UE_LOG(LogTemp, Warning, TEXT("Action Slot has null action - UActionSlot::NativeOnDragDetected"));
		OutOperation = nullptr;
	}

}

bool UActionSlot::NativeOnDrop(const FGeometry & InGeometry, const FDragDropEvent & InDragDropEvent, UDragDropOperation * InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	// if drop on shop row
	if (bFromUpgrade)
	{
		UE_LOG(LogTemp, Warning, 
			TEXT("Drop is not available for Shop Action Slot - UActionSlot::NativeOnDragDetected"));
		return false;
	}

	UItemDrag* itemDragOp = Cast<UItemDrag>(InOperation);
	if (itemDragOp)
	{
		switch (itemDragOp->UpgradeGearType)
		{
		case EUpgradeGearType::Armor:
		{
			if (InventoryGearType == EInventoryGearType::Armor)
			{
				GearUI = itemDragOp->GearUI;
				return true;
			}
			break;
		}
		case EUpgradeGearType::SHWeapon_Long:
		case EUpgradeGearType::SHWeapon_Short:
		{
			if (InventoryGearType == EInventoryGearType::SHWeapon)
			{
				GearUI = itemDragOp->GearUI;
				return true;
			}
			break;
		}
		case EUpgradeGearType::THWeapon:
		{
			if (InventoryGearType == EInventoryGearType::THWeapon)
			{
				GearUI = itemDragOp->GearUI;
				return true;
			}
			break;
		}
		case EUpgradeGearType::RangeWeapon:
		{
			if (InventoryGearType == EInventoryGearType::RangeWeapon)
			{
				GearUI = itemDragOp->GearUI;
				return true;
			}
			break;
		}
		case EUpgradeGearType::HeavyWeapon_Axe:
		case EUpgradeGearType::HeavyWeapon_Hammer:
		{
			if (InventoryGearType == EInventoryGearType::HeavyWeapon)
			{
				GearUI = itemDragOp->GearUI;
				return true;
			}
			break;
		}
		}
		return false;
	}
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

void UActionSlot::OnInventoryInit(int _index)
{
	InventoryGearType = (EInventoryGearType)(_index);
	bFromUpgrade = false;
}

void UActionSlot::OnShopRowInit(int _index)
{
	UpgradeGearType = (EUpgradeGearType)(_index);
	bFromUpgrade = true;
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

