// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TradePopUp_Entry.generated.h"

/**
 * 
 */


UCLASS()
class THELASTBASTION_API UTradePopUp_Entry : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ItemName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ItemValue;


protected:

	bool Initialize() override;
public:

	void EditEntry(FText _ItemName, FText _ItemValue);
	void EditColor(FLinearColor _color);
};
