// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameTeamRow.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "GI_TheLastBastion.h"
#include "GameMode/GamePlayGM.h"
#include "Combat/PawnStatsComponent.h"
#include "TheLastBastionHeroCharacter.h"


bool UInGameTeamRow::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		Name != nullptr && Level != nullptr &&
		HeroClass != nullptr && HpBar != nullptr && Hp != nullptr;

	if (!bAllWidgetAreGood)
	{
		UE_LOG(LogTemp, Error, TEXT("bAllWidgetAreGood is not true -- UInGameTeamRow::Initialize"));
		return false;
	}
	return true;
}

void UInGameTeamRow::InitHeader(const FMatchPlayer & _member)
{	
	Name->SetText(_member.profile.mPlayerName);
	HeroClass->SetText((_member.profile.bIsRangerClass) ?
		FText::FromString(TEXT("Ranger")) : FText::FromString(TEXT("Builder")));

	const UPawnStatsComponent* pawnStats = _member.character->GetPawnStatsComp_Const();
	SetHpValue(pawnStats);
	SetLevel(pawnStats->GetCharacterLevel());
} 

void UInGameTeamRow::SetHpValue(const UPawnStatsComponent* _pawnComp)
{
	FString HpStatsText = FString::Printf(TEXT("%d / %d"), (int)_pawnComp->GetHpCurrent(), 
		(int)(int)_pawnComp->GetHpMax());
	Hp->SetText(FText::FromString(HpStatsText));
	HpBar->SetPercent(_pawnComp->GetHpCurrent() * _pawnComp->GetDivByHpMax());
}

void UInGameTeamRow::SetLevel(int _Level)
{
	FString LevelText = FString::Printf(TEXT("Lv. %d"), _Level);
	Level->SetText(FText::FromString(LevelText));
}
