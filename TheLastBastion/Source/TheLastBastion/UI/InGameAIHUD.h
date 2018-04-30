// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameAIHUD.generated.h"

/**
 * 
 */

USTRUCT()
struct FAIHUDInitializer
{
	GENERATED_BODY();

	UPROPERTY()
	   FText AIName;

	int   AILevel = 1;
	int   AITier = 1;
	float AIHp = 100.0f;
	bool  bIsEnemy;
};


UCLASS()
class THELASTBASTION_API UInGameAIHUD : public UUserWidget
{
	GENERATED_BODY()
	
	
protected:

	bool Initialize() override;

protected:

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AIName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AILevel;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HpBar;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		class UWidgetAnimation* OpaAnim;

	int AITier;

public:

	void InitRowHeader(const FAIHUDInitializer& _init);

	/** Update health bar when the owner of this UI get hit*/
	void UpdateHealthBar(const class UPawnStatsComponent * _pawnStatsComp);

	/** Called when character level up with hero*/
	void OnCharacterLevelUp(const class UPawnStatsComponent * _pawnStatsComp);

	/** Play transparency animation when the AI get hit without being focused on*/
	void ToggleUI(bool _turnOn, bool _playAnim);

};
