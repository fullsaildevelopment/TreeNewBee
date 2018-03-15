// Fill out your copyright notice in the Description page of Project Settings.

#include "RadarHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "GameMode/SinglePlayerGM.h"

URadarHUD::URadarHUD(const FObjectInitializer & _objectInitilizer) : Super(_objectInitilizer)
{   
	// Find the radar actors icon user widget blue print
	static ConstructorHelpers::FClassFinder<UUserWidget> AllyGroupIconBPClass(TEXT("/Game/UI/In-Game/WBP_AllyGroupIcon"));
	AllyGroupIcon_BP = AllyGroupIconBPClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> EnemyGroupIconBPClass(TEXT("/Game/UI/In-Game/WBP_EnemyGroupIcon"));
	EnemyGroupIcon_BP = EnemyGroupIconBPClass.Class;

	AllyGroupIcons.SetNum(4);

	InverseRadarScale = 0.01f;
}

bool URadarHUD::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// check Bind Delegetes to Widget components
	if (RadarOverlay == nullptr)
		return false;

	return true;

}

void URadarHUD::NativeTick(const FGeometry & MyGeometry, float InDeltaTime)
{
	float bearing = GetOwningPlayer()->GetCharacter()->GetActorRotation().Yaw;
	RadarOverlay->SetRenderAngle(-1.0F*bearing);

	ASinglePlayerGM* CurrentGameMode = Cast<ASinglePlayerGM>(GetWorld()->GetAuthGameMode());

	if (CurrentGameMode)
	{   
		// Get Player current position first
		FVector PlayerCurrentPosition = GetOwningPlayer()->GetCharacter()->GetActorLocation();

		// Update Ally Marker On Radar
		for (int i = 0; i < 4; i++)
		{
			if (AllyGroupIcons[i] != nullptr)
			{
				FVector CurrentAllyGroupPosition = CurrentGameMode->GetAllyGroupUnitAt(i)->GetActorLocation();
				FVector2D GroupMarkerRenderTranslation = CalculateGroupLocationOnRadar(PlayerCurrentPosition, CurrentAllyGroupPosition);
				AllyGroupIcons[i]->SetRenderTranslation(GroupMarkerRenderTranslation);
			}
		}

		// Update Enemy Marker On Radar
		for (int j = 0; j < EnemyGroupIcons.Num(); j++)
		{
			if (EnemyGroupIcons[j] != nullptr)
			{
				FVector CurrentEnemyGroupPosition = CurrentGameMode->GetEnemyGroupUnitAt(j)->GetActorLocation();
				FVector2D GroupMarkerRenderTranslation = CalculateGroupLocationOnRadar(PlayerCurrentPosition, CurrentEnemyGroupPosition);
				EnemyGroupIcons[j]->SetRenderTranslation(GroupMarkerRenderTranslation);
			}
		}
	}
}

void URadarHUD::AddAllyGroupIconAt(int _index)
{
	UUserWidget* AllyGroupMarker = CreateWidget<UUserWidget>(GetWorld(), AllyGroupIcon_BP);
	UOverlaySlot* AllyGroupOverlaySlot = RadarOverlay->AddChildToOverlay(AllyGroupMarker);
	AllyGroupOverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	AllyGroupOverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	AllyGroupIcons[_index] = AllyGroupMarker;
}

void URadarHUD::AddEnemyGroupIcon()
{
	UUserWidget* EnemyGroupMarker = CreateWidget<UUserWidget>(GetWorld(), EnemyGroupIcon_BP);
	UOverlaySlot* EnemyGroupOverlaySlot = RadarOverlay->AddChildToOverlay(EnemyGroupMarker);
	EnemyGroupOverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	EnemyGroupOverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	EnemyGroupIcons.Add(EnemyGroupMarker);
}

void URadarHUD::RemoveAllyIconAt(int _index)
{
	AllyGroupIcons[_index]->RemoveFromViewport();
	AllyGroupIcons[_index] = nullptr;
}

void URadarHUD::RemoveEnemyGroupAt(int _index)
{
	EnemyGroupIcons[_index]->RemoveFromViewport();
	EnemyGroupIcons.RemoveAtSwap(_index);
}

FVector2D URadarHUD::CalculateGroupLocationOnRadar(FVector PlayerWorldLocation, FVector GroupWorldLocation)
{
	FVector2D Player2DWorldPosition = FVector2D(PlayerWorldLocation.X, PlayerWorldLocation.Y);

	FVector2D Group2DWorldPosition = FVector2D(GroupWorldLocation.X, GroupWorldLocation.Y);

	FVector2D RelativePositionOnRadar = (Group2DWorldPosition - Player2DWorldPosition)*InverseRadarScale;

	// Check if the position is inside sphere
	float VectorLength = RelativePositionOnRadar.Size();

	FVector2D GroupLocationOnRadar;
	
	if (VectorLength > RadarRadius)
	{
		float ratio = RadarRadius / VectorLength;
		GroupLocationOnRadar = FVector2D(RelativePositionOnRadar.Y * ratio, RelativePositionOnRadar.X * ratio * (-1.0f));
	}
	else
		GroupLocationOnRadar = FVector2D(RelativePositionOnRadar.Y, RelativePositionOnRadar.X * (-1.0f));

	return GroupLocationOnRadar;
	
}
