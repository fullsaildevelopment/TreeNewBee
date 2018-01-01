// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameHUD.h"

#include "CustomType.h"
#include "PCs/GamePC.h"

#include "UI/InGamePlayerRow.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


bool UInGameHUD::Initialize()
{

	if (Super::Initialize() == false)
		return false;


	// Get Ref from Game Instance
	APlayerController* const pc = GetOwningPlayer();
	if (pc)
	{
		mGameInstanceRef = Cast<UGI_TheLastBastion>(pc->GetGameInstance());
	}
	else
		return false;

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


//void UInGameHUD::SetHpStats(float current, float max)
//{
//	TArray<FStringFormatArg> formatArray;
//	formatArray.Add((int)current);
//	formatArray.Add((int)max);
//	FString HpStatsText = FString::Format(TEXT("{0} / {1}"), formatArray);
//	HpStats->SetText(FText::FromString(HpStatsText));
//
//	float ratio = current / max;
//	ratio = FMath::Clamp(ratio, 0.0f, 1.0f);
//	HealthBar->SetPercent(ratio);
//}
//
//void UInGameHUD::SetSpStats(float current, float max)
//{
//	TArray<FStringFormatArg> formatArray;
//	formatArray.Add((int)current);
//	formatArray.Add((int)max);
//	FString SpStatsText = FString::Format(TEXT("{0} / {1}"), formatArray);
//	SpStats->SetText(FText::FromString(SpStatsText));
//
//	float ratio = current / max;
//	ratio = FMath::Clamp(ratio, 0.0f, 1.0f);
//	StaminaBar->SetPercent(ratio);
//}

void UInGameHUD::AddPlayerToPlayerList(const TArray<FPlayerProfile>& _allConnectedPlayers,
	const TArray<AGamePC*>& _allControllers)
{

	APlayerController* thisPC = GetOwningPlayer();

	//for (int iPC = 0; i < length; i++)
	//{

	//}
}

void UInGameHUD::InitPlayerRow(const FPlayerProfile & _profile)
{
	PlayerRow->InitRowHeader(_profile);
}


