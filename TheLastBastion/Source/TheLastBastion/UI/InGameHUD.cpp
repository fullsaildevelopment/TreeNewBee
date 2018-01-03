// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameHUD.h"

#include "CustomType.h"
#include "PCs/GamePC.h"
#include "TheLastBastionHeroCharacter.h"
#include "Combat/HeroStatsComponent.h"
#include "UI/InGamePlayerRow.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/GamePlayGM.h"


bool UInGameHUD::Initialize()
{

	if (Super::Initialize() == false)
		return false;

	UE_LOG(LogTemp, Warning, TEXT("ON CLIENT !!!!!!!!!!!!!!"));


	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		PlayerRow != nullptr && TeamWindow != nullptr
		&& GoldValue != nullptr && WoodValue != nullptr && MetalValue != nullptr && RockValue != nullptr;

	if (!bAllWidgetAreGood)
	{
		UE_LOG(LogTemp, Error, TEXT("bAllWidgetAreGood is not true -- UInGameHUD::Initialize"));
		return false;
	}
	else
		return false;

	return true;
}

void UInGameHUD::AddPlayerToPlayerList(const TArray<FPlayerProfile>& _allConnectedPlayers,
	const TArray<AGamePC*>& _allControllers)
{

	APlayerController* thisPC = GetOwningPlayer();

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
}



