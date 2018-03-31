// Fill out your copyright notice in the Description page of Project Settings.

#include "TradeMenuSlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/Gameplay/InventoryUI.h"
#include "UI/ItemDrag.h"
#include "UI/Gameplay/DraggedItem.h"
#include "Combat/Gear.h"

bool UTradeMenuSlot::Initialize()
{

	if (Super::Initialize() == false)
		return false;

	return true;
}

void UTradeMenuSlot::NativeOnDragDetected(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent, UDragDropOperation *& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (IsDragEnabled() == false)
	{
		OutOperation = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Drag is not available for inventory Action Slot - UActionSlot::NativeOnDragDetected"));
		return;
	}

	if (GearUI.Gear_Bp && GearUI.Gear_Image)
	{
		
		AGear* defaults = GearUI.Gear_Bp.GetDefaultObject(); 
		if (defaults)
		{
			UE_LOG(LogTemp, Warning, TEXT("%f, physical damage"), defaults->GetPhysicalDamage());
		}

		UDraggedItem* dragVisual = CreateWidget<UDraggedItem>(GetOwningPlayer(), WBP_DraggedItem);
		if (dragVisual)
		{
			dragVisual->SetItemSize(SlotSize->WidthOverride, SlotSize->HeightOverride);
			dragVisual->SetImage(GearUI.Gear_Image);

			UItemDrag* dragOp = Cast<UItemDrag>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemDrag::StaticClass()));
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
		//UE_LOG(LogTemp, Warning, TEXT("Action Slot has null action - UActionSlot::NativeOnDragDetected"));
		OutOperation = nullptr;
	}

}

bool UTradeMenuSlot::NativeOnDrop(const FGeometry & InGeometry, const FDragDropEvent & InDragDropEvent, UDragDropOperation * InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	// if drop on shop row
	if (IsDropEnabled() == false)
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
		case EUpgradeGearType::LongSword:
		case EUpgradeGearType::WarAxe:
		case EUpgradeGearType::Mace:
		{
			if (InventoryGearType == EInventoryGearType::SHWeapon)
			{
				GearUI = itemDragOp->GearUI;
				return true;
			}
			break;
		}
		case EUpgradeGearType::Katana:
		{
			if (InventoryGearType == EInventoryGearType::THWeapon)
			{
				GearUI = itemDragOp->GearUI;
				return true;
			}
			break;
		}
		case EUpgradeGearType::CrossBow:
		{
			if (InventoryGearType == EInventoryGearType::RangeWeapon)
			{
				GearUI = itemDragOp->GearUI;
				return true;
			}
			break;
		}
		case EUpgradeGearType::BattleAxe:
		case EUpgradeGearType::Hammer:
		case EUpgradeGearType::GreatSword:
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

void UTradeMenuSlot::SetImage(UTexture2D * _image)
{
	GearUI.Gear_Image = _image;
}

void UTradeMenuSlot::OnInventoryInit(int _index)
{
	InventoryGearType = (EInventoryGearType)(_index);
	SetDragDropMode(EDragDropMode::EDropOnly);
}

void UTradeMenuSlot::OnShopRowInit(int _index)
{
	UpgradeGearType = (EUpgradeGearType)(_index);
	SetDragDropMode(EDragDropMode::EDragOnly);
}






