// Fill out your copyright notice in the Description page of Project Settings.

#include "PopUpBase.h"
#include "CustomType.h"

#include "UI/Gameplay/TradePopUp_Entry.h"
#include "Components/VerticalBox.h"


TSubclassOf<UUserWidget> UPopUpBase::WBP_TradePopUpEntry = nullptr;

UPopUpBase::UPopUpBase(const FObjectInitializer & ObjectInit) : Super(ObjectInit)
{
	if (WBP_TradePopUpEntry == nullptr)
	{
		UCustomType::FindClass<UUserWidget>(WBP_TradePopUpEntry, 
			TEXT("/Game/UI/In-Game/TradeMenu/WBP_TradePopUpEntry"));
	}
}

void UPopUpBase::AddPopUpEntry(
	FString _text, int _val, UVerticalBox * const _list)
{
	UTradePopUp_Entry*	popUpEntry =
		Cast<UTradePopUp_Entry>(CreateWidget<UUserWidget>(GetOwningPlayer(), WBP_TradePopUpEntry));

	if (popUpEntry)
	{
		popUpEntry->EditEntry(FText::FromString(_text), FText::AsNumber(_val));
	}

	_list->AddChildToVerticalBox(popUpEntry);
}

void UPopUpBase::AddPopUpEntryWithColor(FLinearColor _color, 
	FString _text, int _val, UVerticalBox * const _list)
{
	UTradePopUp_Entry*	popUpEntry =
		Cast<UTradePopUp_Entry>(CreateWidget<UUserWidget>(GetOwningPlayer(), WBP_TradePopUpEntry));

	if (popUpEntry)
	{
		popUpEntry->EditEntry(FText::FromString(_text), FText::AsNumber(_val));
		popUpEntry->EditColor(_color);
	}

	_list->AddChildToVerticalBox(popUpEntry);
}
