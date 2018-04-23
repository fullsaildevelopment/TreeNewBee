// Fill out your copyright notice in the Description page of Project Settings.

#include "UnitRow.h"
#include "UI/Gameplay/CrewSlotUI.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"


bool UUnitRow::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		UnitsClassRow && UnitClassName &&
		Slot_0 && Slot_1 && Slot_2;

	if (bAllWidgetAreGood)
	{
		Slot_0->SetDragDropMode(EDragDropMode::EDragOnly);
		Slot_1->SetDragDropMode(EDragDropMode::EDragOnly);
		Slot_2->SetDragDropMode(EDragDropMode::EDragOnly);
	}
	else
		return false;
	return true;
}

void UUnitRow::SetHeadingText(FText _headingText)
{
	UnitClassName->SetText(_headingText);
}

void UUnitRow::SetEachSlotSize(float _width, float _height)
{
	UCrewSlotUI* slot = nullptr;
	for (int i = 0; i < UnitsClassRow->GetChildrenCount(); i++)
	{
		slot = Cast<UCrewSlotUI>(UnitsClassRow->GetChildAt(i));
		if (slot)
		{
			slot->SetSize(_width, _height);
		}
	}
}

void UUnitRow::SetEachSlotAction(const TArray<TSubclassOf<class ATheLastBastionAIBase>>& _unitClass)
{
	UCrewSlotUI* slot = nullptr;
	for (size_t i = 0; i < UnitsClassRow->GetChildrenCount(); i++)
	{
		slot = Cast<UCrewSlotUI>(UnitsClassRow->GetChildAt(i));
		if (slot && _unitClass.IsValidIndex(i))
		{
			slot->SetUnitData(_unitClass[i]);
		}
	}

}

