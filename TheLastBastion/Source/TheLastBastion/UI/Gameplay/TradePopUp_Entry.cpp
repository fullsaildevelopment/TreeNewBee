// Fill out your copyright notice in the Description page of Project Settings.

#include "TradePopUp_Entry.h"
#include "Components/TextBlock.h"


bool UTradePopUp_Entry::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	bool bAllWidgetAreGood = ItemName && ItemValue;

	if (bAllWidgetAreGood)
	{
		return true;
	}
	else return false;
}

void UTradePopUp_Entry::EditEntry(FText _ItemName, FText _ItemValue)
{
	ItemName->SetText(_ItemName);
	ItemValue->SetText(_ItemValue);
}

void UTradePopUp_Entry::EditColor(FLinearColor _color)
{
	ItemName->SetColorAndOpacity(_color);
	ItemValue->SetColorAndOpacity(_color);
}
