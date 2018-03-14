// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadarHUD.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API URadarHUD : public UUserWidget
{
	GENERATED_BODY()
		
public:

	URadarHUD(const FObjectInitializer & _objectInitilizer);

protected:

	bool Initialize() override;

	/** Update Radar rotation */
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:

	TArray<UUserWidget*> AllyIcons;

	TArray<UUserWidget*> EnemyIcons;

public:

	void AddAllyGroupIconAt(int _index);

	void AddEnemyGroupIcon();

	// Remove ally icon, preserve array order
	void RemoveAllyIconAt(int _index);

	// Remove enemy icon, dont preserve array order();
	void RemoveEnemyGroupAt(int _index);

	





};
