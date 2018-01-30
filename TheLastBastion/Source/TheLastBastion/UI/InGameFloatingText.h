// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameFloatingText.generated.h"

/**
 * 
 */
UENUM()
enum class EFloatingTextStyle : uint8
{
	Default,
	Enemy,
	Fire,
	Ice, 
	Critical,
	Stun
};

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

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FSlateColor Enemy;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FSlateColor Critical;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FSlateColor Fire;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FSlateColor Ice;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FSlateColor Stun;


public:

	UFUNCTION(BlueprintCallable)
	void SetInGameFTProperty(FText _val);

	UFUNCTION()
		void SetStyle(EFloatingTextStyle _val);

	void SetFontSize(float _val);

};
