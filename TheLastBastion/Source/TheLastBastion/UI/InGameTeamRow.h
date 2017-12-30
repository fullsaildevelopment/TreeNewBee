// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameTeamRow.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UInGameTeamRow : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	bool Initialize() override;

protected:

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* Name;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* Level;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Class;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Hp;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HpBar;

};
