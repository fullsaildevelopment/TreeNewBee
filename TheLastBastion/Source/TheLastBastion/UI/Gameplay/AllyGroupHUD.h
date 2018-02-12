// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Gameplay/GroupHUD.h"
#include "AllyGroupHUD.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UAllyGroupHUD : public UGroupHUD
{
	GENERATED_BODY()
	
protected:

	bool Initialize() override;

protected:


	UPROPERTY()
		class UTextBlock* GroupIndexText;

	
public:
	
	void SetGroupIndexText(int _groupIndex);

	
};
