// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameHUD.h"




#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"
#include "GI_TheLastBastion.h"
#include "TheLastBastionHeroCharacter.h"
#include "Combat/HeroStatsComponent.h"


bool UInGameHUD::Initialize()
{

	if (Super::Initialize() == false)
		return false;

	// Get Ref from Game Instance
	APlayerController* const pc = GetOwningPlayer();
	if (pc)
	{
		mGameInstanceRef = Cast<UGI_TheLastBastion>(pc->GetGameInstance());
		mHeroCharacter = Cast<ATheLastBastionHeroCharacter>(pc->GetCharacter());
		if (mGameInstanceRef == nullptr || mHeroCharacter == nullptr)
		{

			UE_LOG(LogTemp, Error, TEXT("Cast to ATheLastBastionHeroCharacter Failed - UInGameHUD"));
			return false;
		}
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

	//UE_LOG(LogTemp, Warning, TEXT("Fill Hp Bar"));
	//SetHpStats(mHeroCharacter->GetHeroStatsComp()->GetHpCurrent(),
	//	mHeroCharacter->GetHeroStatsComp()->GetHpMax());
	//SetSpStats(mHeroCharacter->GetHeroStatsComp()->GetStaminaCurrent(), 
	//	mHeroCharacter->GetHeroStatsComp()->GetStaminaMax());
	//UE_LOG(LogTemp, Warning, TEXT("Fill Hp Bar Done"));

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
