// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameHUD.generated.h"

/**
*
*/
UCLASS()
class THELASTBASTION_API UInGameHUD : public UUserWidget
{
	GENERATED_BODY()


public:
	UInGameHUD(const FObjectInitializer& objInit);

protected:

	bool Initialize() override;


	UPROPERTY(meta = (BindWidget))
		class UInGamePlayerRow* PlayerRow;

	UPROPERTY(meta = (BindWidget))
		class UVerticalBox* TeamWindow;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* GoldValue;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WoodValue;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MetalValue;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* RockValue;

	UPROPERTY(meta = (BindWidget))
		class UWidget* CrossHair;

	UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
		class UTexture2D* WeaponThumbNail;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ProjectileCount_Text;


	UPROPERTY()
		bool bIsRanger;




public:

	void AddTeamMember(const FPlayerProfile& _newTeamMember);

	/** Initialize the HUD during creation by player profile*/
	void LoadPlayerProfile(const FPlayerProfile& _profile);

	void InitStats(const class UHeroStatsComponent* _heroStats);

	void SetHpOnHealthChange(const UPawnStatsComponent * _pawnStats);
};
