// Fill out your copyright notice in the Description page of Project Settings.

#include "RadarHUD.h"

URadarHUD::URadarHUD(const FObjectInitializer & _objectInitilizer) : Super(_objectInitilizer)
{
	AllyIcons.SetNum(4);
}

bool URadarHUD::Initialize()
{

	if (Super::Initialize() == false)
		return false;

	UE_LOG(LogTemp, Warning, TEXT("ON CLIENT !!!!!!!!!!!!!!"));


	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood = true;

	if (!bAllWidgetAreGood)
	{
		UE_LOG(LogTemp, Error, TEXT("bAllWidgetAreGood is not true -- UInGameHUD::Initialize"));
		return false;
	}


	// Widget Init

	return true;

}

void URadarHUD::NativeTick(const FGeometry & MyGeometry, float InDeltaTime)
{
	float bearing = GetOwningPlayer()->GetCharacter()->GetActorRotation().Yaw;
}

void URadarHUD::AddAllyGroupIconAt(int _index)
{

}

void URadarHUD::AddEnemyGroupIcon()
{

}

void URadarHUD::RemoveAllyIconAt(int _index)
{
}

void URadarHUD::RemoveEnemyGroupAt(int _index)
{
}
