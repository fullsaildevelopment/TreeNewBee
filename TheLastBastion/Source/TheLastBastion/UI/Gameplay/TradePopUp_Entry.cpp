// Fill out your copyright notice in the Description page of Project Settings.

#include "TradePopUp_Entry.h"


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

void UTradePopUp_Entry::AddEntry(FText _ItemName, FText _ItemValue)
{

}
