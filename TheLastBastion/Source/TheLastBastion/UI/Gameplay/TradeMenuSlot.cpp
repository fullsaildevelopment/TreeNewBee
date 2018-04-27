// Fill out your copyright notice in the Description page of Project Settings.

#include "TradeMenuSlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "UI/Gameplay/InventoryUI.h"
#include "UI/ItemDrag.h"
#include "UI/Gameplay/DraggedItem.h"
#include "UI/Gameplay/TradeMenu_PopUp.h"

#include "PCs/SinglePlayerPC.h"

#include "Combat/Gear.h"
#include "CustomType.h"
#include "SlateBlueprintLibrary.h"


//#include <Runtime/Engine/Classes/Engine/Engine.h>




UTradeMenuSlot::UTradeMenuSlot(const FObjectInitializer& _objInit) : Super(_objInit)
{
	if (WBP_TradePopUpWidget == nullptr)
	{
		UCustomType::FindClass<UUserWidget>(WBP_TradePopUpWidget, TEXT("/Game/UI/In-Game/TradeMenu/WBP_TradePopUp"));
	}
}

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

	// if drop on something dont accept drop operation
	if (IsDropEnabled() == false)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Drop is not available for Shop Action Slot - UActionSlot::NativeOnDragDetected"));
		return false;
	}

	UItemDrag* itemDragOp = Cast<UItemDrag>(InOperation);
	if (itemDragOp)
	{
		// if this is the same gear class, we will skip 
		if (itemDragOp->GearUI.Gear_Bp == GearUI.Gear_Bp)
			return false;

		bool success = false;

		// if drop on the correct gear type, the drop will success
		switch (itemDragOp->UpgradeGearType)
		{
		case EUpgradeGearType::Armor:
		{
			if (InventoryGearType == EInventoryGearType::Armor)
				success = true;
			break;
		}
		case EUpgradeGearType::LongSword:
		case EUpgradeGearType::WarAxe:
		case EUpgradeGearType::Mace:
		{
			if (InventoryGearType == EInventoryGearType::SHWeapon)
				success = true;
			break;
		}
		case EUpgradeGearType::Katana:
		{
			if (InventoryGearType == EInventoryGearType::THWeapon)
				success = true;
			break;
		}
		case EUpgradeGearType::CrossBow:
		{
			if (InventoryGearType == EInventoryGearType::RangeWeapon)
				success = true;
			break;
		}
		case EUpgradeGearType::BattleAxe:
		case EUpgradeGearType::Hammer:
		case EUpgradeGearType::GreatSword:
		{
			if (InventoryGearType == EInventoryGearType::HeavyWeapon)
				success = true;
			break;
		}
		default:
			break;
		}

		// if drop on the wrong gear type, the drop operation will return fail
		if (success == false)
			return false;
		else
		{
			// if we have enough resouce to buy
			success = CostCheck(itemDragOp->GearUI.Gear_Bp);
			if (success)
				GearUI = itemDragOp->GearUI;
			return success;
		}
	}
	else
	{
		return false;
	}
}

void UTradeMenuSlot::NativeOnMouseEnter(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (WBP_TradePopUpWidget && GearUI.Gear_Bp != nullptr)
	{
		PopUpWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), WBP_TradePopUpWidget);

		FVector2D WidgetLocation = InGeometry.GetAbsolutePosition();
		FVector2D MouseLocation = InMouseEvent.GetScreenSpacePosition();

		FVector2D pixelPosition, viewPortPosition;

		USlateBlueprintLibrary::AbsoluteToViewport(GetWorld(), 
			MouseLocation, pixelPosition, viewPortPosition);

		PopUpWidget->SetAlignmentInViewport(FVector2D(1.0f, 0.0f));
		PopUpWidget->SetPositionInViewport(viewPortPosition);

		UTradeMenu_PopUp* PopUp = Cast<UTradeMenu_PopUp>(PopUpWidget);
		if (PopUp)
		{
			PopUp->OnPopUp(GearUI.Gear_Bp);
			PopUp->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PopUp == nullptr, UTradeMenuSlot::NativeOnMouseEnter"));

		}

		//PopUpWidget->SetRenderTranslation(InMouseEvent.GetScreenSpacePosition());
	}

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

bool UTradeMenuSlot::CostCheck(TSubclassOf<class AGear> _gear) const
{
	bool bAccept = true;

	ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(GetOwningPlayer());
	if (pc == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("pc == nullptr,  UTradeMenuSlot::CostCheck"));
		bAccept = false;
	}

	UInventoryUI* inventoryUI = pc->GetInventoryUI();
	if (inventoryUI == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("inventoryUI == nullptr,  UTradeMenuSlot::CostCheck"));
		bAccept = false;
	}

	// check if player has the resource for the cost
	AGear* dragGear = _gear.GetDefaultObject();
	TArray<int> costs = dragGear->GetResourceCost();

	int requiredMetal = costs[0];
	if (requiredMetal != 0)
	{
		if (requiredMetal > inventoryUI->GetMetal_int())
			bAccept = false;
		else
			inventoryUI->AddMetalValue(-requiredMetal);
	}

	int requiredWood = costs[1];
	if (requiredWood != 0)
	{
		if (requiredWood > inventoryUI->GetWood_int())
			bAccept = false;
		else
			inventoryUI->AddWoodValue(-requiredWood);
	}

	return bAccept;
}






