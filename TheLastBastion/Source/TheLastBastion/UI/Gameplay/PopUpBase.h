// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopUpBase.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UPopUpBase : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPopUpBase(const FObjectInitializer & ObjectInit);

protected:

	static TSubclassOf<UUserWidget> WBP_TradePopUpEntry;


protected:

	void AddPopUpEntry(FString _text, 
		int _val, class UVerticalBox* const _list);

	void AddPopUpEntryWithColor(FLinearColor _color, FString _text, 
		int _val, class UVerticalBox* const _list);
};
