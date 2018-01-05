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
		Class != nullptr && HpBar != nullptr && Hp != nullptr &&
		SpBar != nullptr && Sp != nullptr;


	//APlayerController* pc = GetOwningPlayer();
	//if (pc != nullptr)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("pc is: %s"), *pc->GetName());
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("pc is null"));

	//}

	if (!bAllWidgetAreGood)
	{
		UE_LOG(LogTemp, Error, TEXT("bAllWidgetAreGood is not true -- UInGamePlayerRow::Initialize"));
		return false;
	}
	return true;
}

void UInGamePlayerRow::SetPlayerName(const FPlayerProfile& _profile)
{
	HeroAvatarImage = _profile.mCharacterImage;
	Name->SetText(_profile.mPlayerName);

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
	HpBar->SetPercent(_currentSp * _divByMax);

}

void UInGamePlayerRow::SetLevel(int _Level)
{
	FString LevelText = FString::Printf(TEXT("Lv. %d"), _Level);
	Level->SetText(FText::FromString(LevelText));
}
