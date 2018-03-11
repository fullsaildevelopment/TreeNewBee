// Fill out your copyright notice in the Description page of Project Settings.

#include "InGamePlayerRow.h"
#include "CustomType.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

bool UInGamePlayerRow::Initialize()
{
	if (Super::Initialize() == false)
		return false;


	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		Name != nullptr && Level != nullptr &&
		Class != nullptr && HpBar != nullptr && Hp != nullptr && DpBar != nullptr &&
		SpBar != nullptr && Sp != nullptr;


	if (!bAllWidgetAreGood)
	{
		UE_LOG(LogTemp, Error, TEXT("bAllWidgetAreGood is not true -- UInGamePlayerRow::Initialize"));
		return false;
	}
	return true;
}

void UInGamePlayerRow::InitByPlayerProfile(const FPlayerProfile& _profile)
{
	HeroAvatarImage = _profile.mCharacterImage;
	Name->SetText(_profile.mPlayerName);
	if (_profile.bIsRangerClass)
		Class->SetText(FText::FromString("Ranger"));
	else		
		Class->SetText(FText::FromString("Builder"));
}


void UInGamePlayerRow::SetHpValue(float _currentHp, float _maxHp)
{
	FString HpStatsText = FString::Printf(TEXT("%d / %d"), (int) _currentHp, (int) _maxHp);
	Hp->SetText(FText::FromString(HpStatsText));
}

void UInGamePlayerRow::SetHpPercentage(float _currentHp, float _divByMax)
{
	HpBar->SetPercent(_currentHp * _divByMax);
}

void UInGamePlayerRow::SetSpValue(float _currentSp, float _maxSp)
{
	FString SpStatsText = FString::Printf(TEXT("%d / %d"), (int)_currentSp, (int)_maxSp);
	Sp->SetText(FText::FromString(SpStatsText));

}

void UInGamePlayerRow::SetSpPercentage(float _currentSp, float _divByMax)
{
	SpBar->SetPercent(_currentSp * _divByMax);

}

void UInGamePlayerRow::SetDpPercentage(float _currentDp, float _divByMax)
{
	DpBar->SetPercent(_currentDp * _divByMax);
}

void UInGamePlayerRow::SetLevel(int _Level)
{
	FString LevelText = FString::Printf(TEXT("Lv. %d"), _Level);
	Level->SetText(FText::FromString(LevelText));
}

void UInGamePlayerRow::SetHp(float _currentHp, float _maxHp, float _divByMax)
{
	SetHpValue(_currentHp, _maxHp);
	SetHpPercentage(_currentHp, _divByMax);
}

void UInGamePlayerRow::SetSp(float _currentSp, float _maxSp, float _divByMax)
{
	SetSpValue(_currentSp, _maxSp);
	SetSpPercentage(_currentSp, _divByMax);
}

