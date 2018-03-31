// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionSlot.h"
#include "Components/SizeBox.h"
#include "Components/Button.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/Gameplay/InventoryUI.h"
#include "UI/ItemDrag.h"
#include "UI/Gameplay/DraggedItem.h"
#include "CustomType.h"



TSubclassOf<UUserWidget> UActionSlot::WBP_DraggedItem = nullptr;

TSubclassOf<UUserWidget> UActionSlot::WBP_PopUpWidget = nullptr;


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
		SlotSize != nullptr;// && ActionButton;

	if (bAllWidgetAreGood)
	{
		//ActionButton->OnClicked.AddDynamic(this, &UActionSlot::OnButtonClick);
	}
	else
		return false;

	//ActionButton->IsFocusable = false;
	//ActionButton->SetClickMethod(EButtonClickMethod::PreciseClick);
	//ActionButton->SetTouchMethod(EButtonTouchMethod::PreciseTap);

	DragDropMode = EDragDropMode::EDragAndDrop;
	SetSize(DefaultActionSlot_Width, DefaultActionSlot_Height);

	return true;
}

FReply UActionSlot::NativeOnMouseButtonDown(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, FKey(TEXT("LeftMouseButton"))).NativeReply;
}

void UActionSlot::NativeOnMouseLeave(const FPointerEvent & InMouseEvent)
{

	UE_LOG(LogTemp, Warning, TEXT("UActionSlot::NativeOnMouseLeave"));

	if (PopUpWidget)
	{
		PopUpWidget->RemoveFromParent();
	}
}

//void UActionSlot::SetIsButton(bool _val)
//{
//	if (_val)
//	{
//		//ActionButton->SetVisibility(ESlateVisibility::Visible);
//		DragDropMode = EDragDropMode::EDisable;
//	}
//	//else
//	//{
//	//	ActionButton->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
//	//}
//}

void UActionSlot::SetDragDropMode(EDragDropMode _mode)
{
	//switch (_mode)
	//{
	//case EDragDropMode::EDragAndDrop:
	//case EDragDropMode::EDragOnly:
	//	SetIsButton(false);
	//	break;
	//case EDragDropMode::EDropOnly:
	//case EDragDropMode::EDisable:
	//default:
	//	break;
	//}
	DragDropMode = _mode;
}

void UActionSlot::SetSize(float _width, float _height)
{
	SlotSize->SetWidthOverride(true);
	SlotSize->SetHeightOverride(true);

	SlotSize->WidthOverride = _width;
	SlotSize->HeightOverride = _height;
}

