// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameFloatingText.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UInGameFloatingText : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UInGameFloatingText(const FObjectInitializer & ObjectInit);

protected:

	bool Initialize() override;


protected:

	UPROPERTY(BlueprintReadwrite, meta = (BindWidget))
		class UTextBlock* InGameFT;

public:

	UFUNCTION(BlueprintCallable)
	void SetInGameFTProperty(FText _val);

};
