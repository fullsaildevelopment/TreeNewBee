// Fill out your copyright notice in the Description page of Project Settings.

#include "InGamePlayerRow.h"
#include "CustomType.h"
#include "Components/TextBlock.h"

bool UInGamePlayerRow::Initialize()
{
	if (Super::Initialize() == false)
		return false;


	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		Name != nullptr && Level != nullptr &&
		Class != nullptr && HpBar != nullptr && Hp != nullptr &&
		SpBar != nullptr && Sp != nullptr;

	if (!bAllWidgetAreGood)
	{
		UE_LOG(LogTemp, Error, TEXT("bAllWidgetAreGood is not true -- UInGamePlayerRow::Initialize"));
		return false;
	}
	return true;
}

void UInGamePlayerRow::InitRowHeader(const FPlayerProfile& _profile)
{
	HeroAvatarImage = _profile.mCharacterImage;
	Name->SetText(_profile.mPlayerName);
	
	

}
