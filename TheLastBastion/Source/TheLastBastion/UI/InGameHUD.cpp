// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameHUD.h"

#include "CustomType.h"
#include "PCs/GamePC.h"
#include "TheLastBastionHeroCharacter.h"
#include "Combat/HeroStatsComponent.h"
#include "UI/InGamePlayerRow.h"
#include "UI/InGameTeamRow.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"


static TSubclassOf<UUserWidget> InGameTeamRow_WBPClass;

UInGameHUD::UInGameHUD(const FObjectInitializer& objInit) : Super(objInit)
{
	if (!InGameTeamRow_WBPClass)
	{
		UCustomType::FindClass<UUserWidget>(InGameTeamRow_WBPClass, TEXT("/Game/UI/In-Game/WBP_InGameTeamRow"));
	}
}

bool UInGameHUD::Initialize()
{

	if (Super::Initialize() == false)
		return false;

	UE_LOG(LogTemp, Warning, TEXT("ON CLIENT !!!!!!!!!!!!!!"));


	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		PlayerRow != nullptr && TeamWindow != nullptr
		&& GoldValue != nullptr && WoodValue != nullptr && MetalValue != nullptr && RockValue != nullptr && CrossHair != nullptr;

	if (!bAllWidgetAreGood)
	{
		UE_LOG(LogTemp, Error, TEXT("bAllWidgetAreGood is not true -- UInGameHUD::Initialize"));
		return false;
	}
	else
		return false;

	return true;
}

void UInGameHUD::AddTeamMember(const FPlayerProfile & _newTeamMember)
{
	if (!InGameTeamRow_WBPClass)
	{
		UE_LOG(LogTemp, Error, TEXT("InGameTeamRow_WBPClass is NULL -  UInGameHUD::AddTeamMember"));
		return;
	}
	UInGameTeamRow* newTeamRow = Cast<UInGameTeamRow>(CreateWidget<UUserWidget>(GetOwningPlayer(), InGameTeamRow_WBPClass));	
	if (newTeamRow)
	{
		newTeamRow->InitHeader(_newTeamMember);
		TeamWindow->AddChild(newTeamRow);
	}
}

void UInGameHUD::SetPlayerName(const FPlayerProfile & _profile)
{
	PlayerRow->SetPlayerName(_profile);
}

void UInGameHUD::InitStats(const UHeroStatsComponent * _heroStats)
{
	PlayerRow->SetHpValue(_heroStats->GetHpCurrent(), _heroStats->GetHpMax());
	PlayerRow->SetSpValue(_heroStats->GetStaminaCurrent(), _heroStats->GetStaminaMax());
	PlayerRow->SetLevel(_heroStats->GetLevel());


	CrossHair->SetVisibility(ESlateVisibility::Hidden);

}

void UInGameHUD::SetHpOnHealthChange(const UPawnStatsComponent * _pawnStats)
{
	float currentHp = FMath::Clamp(_pawnStats->GetHpCurrent(), 0.0f, MAX_FLT);

	PlayerRow->SetHpValue(currentHp, _pawnStats->GetHpMax());
	PlayerRow->SetHpPercentage(currentHp, _pawnStats->GetDivByHpMax());
}



