// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameAIHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Combat/PawnStatsComponent.h"


bool UInGameAIHUD::Initialize()
{

	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		AIName != nullptr && AILevel != nullptr && HpBar != nullptr;


	if (!bAllWidgetAreGood)
	{
		UE_LOG(LogTemp, Error, TEXT("bAllWidgetAreGood is not true -- UInGameHUD::Initialize"));
		return false;
	}
	else
		return false;

	AITier = 1;
	HpBar->SetPercent(1.0f);
	return true;
}

void UInGameAIHUD::InitRowHeader(const FAIHUDInitializer & _init)
{
	AIName->SetText(_init.AIName);
	FString LevelText = FString::Printf(TEXT("Lv. %d"), _init.AILevel);
	AILevel->SetText(FText::FromString(LevelText));

	if (_init.bIsEnemy)
	{
		AIName->SetColorAndOpacity(FLinearColor::Red);
		AILevel->SetColorAndOpacity(FLinearColor::Red);
	}


	AITier = _init.AITier;
}

void UInGameAIHUD::UpdateHealthBar(const UPawnStatsComponent * _pawnStatsComp)
{
	HpBar->SetPercent(_pawnStatsComp->GetHpCurrent() * _pawnStatsComp->GetDivByHpMax());
}

void UInGameAIHUD::ToggleUI(bool _turnOn, bool _playAnim)
{

	if (OpaAnim == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("OpaAnim is nullptr,  UInGameAIHUD::ToggleUI"));
	}

	if (_turnOn)
	{
		if (_playAnim)
			PlayAnimation(OpaAnim);
		else
		{
			if (IsAnimationPlaying(OpaAnim))
				StopAnimation(OpaAnim);
			SetColorAndOpacity(FLinearColor(FColor::White));
		}
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{		
		if (IsAnimationPlaying(OpaAnim))
			StopAnimation(OpaAnim);
		SetColorAndOpacity(FLinearColor(FColor::White));
		SetVisibility(ESlateVisibility::Hidden);
	}
}

