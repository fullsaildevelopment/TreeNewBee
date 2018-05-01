// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameHUD.h"

#include "CustomType.h"
//#include "PCs/GamePC.h"
#include "TheLastBastionHeroCharacter.h"
#include "Combat/HeroStatsComponent.h"
#include "UI/InGamePlayerRow.h"
//#include "UI/InGameTeamRow.h"
#include "UI/Gameplay/WeaponSlotsUI.h"
#include "UI/Gameplay/CrewBarUI.h"
#include "UI/Gameplay/SkillBarUI.h"
#include "UI/Gameplay/RadarHUD.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"


const FString BattleDescription[15] =
{
	TEXT("BattleDescription 1"),
	TEXT("BattleDescription 2"),
	TEXT("BattleDescription 3"),
	TEXT("BattleDescription 4"),
	TEXT("BattleDescription 5"),
	TEXT("BattleDescription 6"),
	TEXT("BattleDescription 7"),
	TEXT("BattleDescription 8"),
	TEXT("BattleDescription 9"),
	TEXT("BattleDescription 10"),
	TEXT("BattleDescription 11"),
	TEXT("BattleDescription 12"),
	TEXT("BattleDescription 13"),
	TEXT("BattleDescription 14"),
	TEXT("BattleDescription 15")
};

#define CastleDestoryNotificationColor      FLinearColor::Red
#define VictoryNotificationColor   FLinearColor(0.607313f,0.015308f,1.000000f,1.000000f)
#define WaitNotificationColor   FLinearColor(0.261106f,1.000000f,0.249277f,1.000000f)
#define BattleNotificationColor FLinearColor(0.045752f,0.143075f,1.000000f,1.000000f)

UInGameHUD::UInGameHUD(const FObjectInitializer& objInit) : Super(objInit)
{
	//if (!InGameTeamRow_WBPClass)
	//{
	//	UCustomType::FindClass<UUserWidget>(InGameTeamRow_WBPClass, TEXT("/Game/UI/In-Game/WBP_InGameTeamRow"));
	//}
}

bool UInGameHUD::Initialize()
{
	bIsRanger = true;

	if (Super::Initialize() == false)
		return false;

	//UE_LOG(LogTemp, Warning, TEXT("ON CLIENT !!!!!!!!!!!!!!"));


	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		PlayerRow != nullptr && TeamWindow != nullptr
		&& FoodValue != nullptr && WoodValue != nullptr && MetalValue != nullptr && StoneValue != nullptr && PopUpNotification != nullptr && WaveNotification != nullptr
		&& CrossHair != nullptr && WeaponSlots && GroupCommandList && CrewBar && RadarHUD;

	if (!bAllWidgetAreGood)
	{
		UE_LOG(LogTemp, Error, TEXT("bAllWidgetAreGood is not true -- UInGameHUD::Initialize"));
		return false;
	}
	GroupCommandList->SetVisibility(ESlateVisibility::Hidden);
	PopUpNotification->SetVisibility(ESlateVisibility::Hidden);
	WaveNotification->SetVisibility(ESlateVisibility::Visible);

	SetWaveNotificationOnWait();

	return true;
}

//void UInGameHUD::AddTeamMember(const FMatchPlayer& _newTeamMember)
//{
//	if (!InGameTeamRow_WBPClass)
//	{
//		UE_LOG(LogTemp, Error, TEXT("InGameTeamRow_WBPClass is NULL -  UInGameHUD::AddTeamMember"));
//		return;
//	}
//	UInGameTeamRow* newTeamRow = Cast<UInGameTeamRow>(CreateWidget<UUserWidget>(GetOwningPlayer(), InGameTeamRow_WBPClass));	
//	if (newTeamRow)
//	{
//		newTeamRow->InitHeader(_newTeamMember);
//		TeamWindow->AddChild(newTeamRow);
//	}
//}

void UInGameHUD::LoadPlayerProfile(const FPlayerProfile & _profile)
{
	PlayerRow->InitByPlayerProfile(_profile);
	bIsRanger = _profile.bIsRangerClass;
	if (bIsRanger)
	{		
		CrossHair->SetVisibility(ESlateVisibility::Hidden);
		//ProjectileCount_Text->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInGameHUD::LoadPlayerProfile_Sp(const FPlayerProfile & _profile)
{
	PlayerRow->InitByPlayerProfile(_profile);

	CrossHair->SetVisibility(ESlateVisibility::Hidden);
	//ProjectileCount_Text->SetVisibility(ESlateVisibility::Hidden);
}

void UInGameHUD::InitStats(const UHeroStatsComponent * _heroStats)
{
	PlayerRow->SetHpValue(_heroStats->GetHpCurrent(), _heroStats->GetHpMax());
	PlayerRow->SetSpValue(_heroStats->GetStaminaCurrent(), _heroStats->GetStaminaMax());
	PlayerRow->SetLevel(_heroStats->GetCharacterLevel());

	WeaponSlots->OnInitStats(_heroStats);
}

void UInGameHUD::ResetStats(const UHeroStatsComponent * _heroStats)
{
	PlayerRow->SetHp(_heroStats->GetHpCurrent(), _heroStats->GetHpMax(), _heroStats->GetDivByHpMax());
	PlayerRow->SetSp(_heroStats->GetStaminaCurrent(), _heroStats->GetStaminaMax(), _heroStats->GetDivBySpMax());
	PlayerRow->SetLevel(_heroStats->GetCharacterLevel());

	WeaponSlots->OnInitStats(_heroStats);

}

void UInGameHUD::SetHpOnHealthChange(const UPawnStatsComponent * _pawnStats)
{
    //float currentHp = FMath::Clamp(_pawnStats->GetHpCurrent(), 0.0f, MAX_FLT);
	float currentHp = _pawnStats->GetHpCurrent();
	PlayerRow->SetHpValue(currentHp, _pawnStats->GetHpMax());
	PlayerRow->SetHpPercentage(currentHp, _pawnStats->GetDivByHpMax());
}

void UInGameHUD::SetSpOnStaminaChange(const UPawnStatsComponent * _pawnStats)
{
	//float currentSp = FMath::Clamp(_pawnStats->GetStaminaCurrent(), 0.0f, MAX_FLT);
	float currentSp = _pawnStats->GetStaminaCurrent();
	PlayerRow->SetSp(currentSp, _pawnStats->GetStaminaMax(), _pawnStats->GetDivBySpMax());
}

void UInGameHUD::SetDpOnDpChange(const UPawnStatsComponent * _pawnStats)
{

	//float currentDp = FMath::Clamp(_pawnStats->GetDpCurrent(), 0.0f, MAX_FLT);
	float currentDp = _pawnStats->GetDpCurrent();
	PlayerRow->SetDpPercentage(currentDp, _pawnStats->GetDivByHpMax());
}

void UInGameHUD::SetExpBarValue(float _val)
{
	PlayerRow->SetExpBar(_val);
}

void UInGameHUD::SetCastleBarValue(float _val)
{
	PlayerRow->SetCastleHealth(_val);
}

void UInGameHUD::SetCastleBarColor(FLinearColor _color)
{
	PlayerRow->SetCastleHpBarColor(_color);
}

void UInGameHUD::SetPlayerRowOnLevelUp(const UHeroStatsComponent * _heroStats)
{
	PlayerRow->SetHp(_heroStats->GetHpCurrent(), 
		_heroStats->GetHpMax(), 
		_heroStats->GetDivByHpMax());
	PlayerRow->SetSp(_heroStats->GetStaminaCurrent(), 
		_heroStats->GetStaminaMax(), 
		_heroStats->GetDivBySpMax());
	PlayerRow->SetLevel(_heroStats->GetCharacterLevel());
}



void UInGameHUD::ToggleFireMode(bool _val)
{
	if (_val == true)
	{
		CrossHair->SetVisibility(ESlateVisibility::Visible);
		//ProjectileCount_Text->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CrossHair->SetVisibility(ESlateVisibility::Hidden);
		//ProjectileCount_Text->SetVisibility(ESlateVisibility::Hidden);

	}
}

void UInGameHUD::ToggleCommandList(bool _val)
{
	if (_val)
	{
		GroupCommandList->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		GroupCommandList->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInGameHUD::SetCurrentWeaponImage(const AGear * _gear)
{
	WeaponSlots->SetCurrentWeaponImage(_gear);
}

void UInGameHUD::OnSelectedCrewAt(int _index)
{
	CrewBar->OnSelectedCrewAt(_index);
}

void UInGameHUD::OnLaunchSkillAt(int _skillIndex, float _cdDuration)
{
	SkillBar->OnLaunchSkillAt(_skillIndex, _cdDuration);
}

bool UInGameHUD::IsSkilledCooledDown(int _skillIndex) const
{
	return 	SkillBar->IsCooledDownAt(_skillIndex);
}

void UInGameHUD::AddAllyGroupIconAt(int _index)
{
	RadarHUD->AddAllyGroupIconAt(_index);
}

void UInGameHUD::AddEnemyGroupIcon()
{
	RadarHUD->AddEnemyGroupIcon();
}

void UInGameHUD::RemoveAllyIconAt(int _index)
{
	RadarHUD->RemoveAllyIconAt(_index);
}

void UInGameHUD::RemoveEnemyGroupAt(int _index)
{
	RadarHUD->RemoveEnemyGroupAt(_index);
}

void UInGameHUD::UpdateCrewSizeAt(int _index, int _groupSize)
{
	CrewBar->UpdateCrewSizeAt(_index, _groupSize);
}

void UInGameHUD::UpdateCrewInfoAt(int _index, int _groupSize, UTexture2D * _thumbNail)
{
	CrewBar->UpdateCrewInfoAt(_index, _groupSize, _thumbNail);
}

void UInGameHUD::SetWoodValue(int _val)
{
	WoodValue->SetText(FText::AsNumber(_val));
}

void UInGameHUD::SetStoneValue(int _val)
{
	StoneValue->SetText(FText::AsNumber(_val));
}

void UInGameHUD::SetMetalValue(int _val)
{
	MetalValue->SetText(FText::AsNumber(_val));
}

void UInGameHUD::SetFoodValue(int _val)
{
	FoodValue->SetText(FText::AsNumber(_val));
}

void UInGameHUD::SetPopUpNotificationVisibility(bool visiblity)
{
	if (visiblity == true)
		PopUpNotification->SetVisibility(ESlateVisibility::Visible);
	else
		PopUpNotification->SetVisibility(ESlateVisibility::Hidden);
}

void UInGameHUD::SetWaveNotificationOnWave(int _index)
{
	int wave = _index + 1;

	FString NotificationOnWait = FString::Printf(TEXT("Wave %d: %s"), wave, *BattleDescription[_index]);
	WaveNotification->SetText(FText::FromString(NotificationOnWait));
	WaveNotification->SetColorAndOpacity(BattleNotificationColor);

}

void UInGameHUD::SetWaveNotificationOnWait()
{
	FString NotificationOnWait = TEXT("When you are ready, Press [Enter] to start next battle");
	WaveNotification->SetText(FText::FromString(NotificationOnWait));
	WaveNotification->SetColorAndOpacity(WaitNotificationColor);

}

void UInGameHUD::SetCastleDestoryNotification()
{
	FString NotificationOnWait = TEXT("The Castle has lost, clear the remaining enemies and repair the battle");
	WaveNotification->SetText(FText::FromString(NotificationOnWait));
	WaveNotification->SetColorAndOpacity(CastleDestoryNotificationColor);
}

void UInGameHUD::SetVictoryNotification()
{
	FString NotificationOnWait = TEXT("You beat the death, and the livings survive because of you");
	WaveNotification->SetText(FText::FromString(NotificationOnWait));
	WaveNotification->SetColorAndOpacity(VictoryNotificationColor);
}

//void UInGameHUD::SetStartWaveNotificationVisibility(bool visiblity)
//{
//	if (visiblity == true)
//		WaveNotification->SetVisibility(ESlateVisibility::Visible);
//	else
//		WaveNotification->SetVisibility(ESlateVisibility::Hidden);
//}





