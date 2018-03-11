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

	UPROPERTY(meta = (BindWidget))
		class UWeaponSlotsUI* WeaponSlots;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* GroupCommandList;

	UPROPERTY(meta = (BindWidget))
		class UCrewBarUI* CrewBar;

	UPROPERTY()
		bool bIsRanger;

public:

	void AddTeamMember(const struct FMatchPlayer& _newTeamMember);

	/** Initialize the HUD during creation by player profile*/
	void LoadPlayerProfile(const struct FPlayerProfile& _profile);

	void LoadPlayerProfile_Sp(const struct FPlayerProfile& _profile);

	void InitStats(const class UHeroStatsComponent* _heroStats);

    /** Called On ReEquip, Level Up*/
	void ResetStats(const class UHeroStatsComponent* _heroStats);

	void SetHpOnHealthChange(const class UPawnStatsComponent * _pawnStats);

	void SetSpOnStaminaChange(const class UPawnStatsComponent * _pawnStats);

	void SetDpOnDpChange(const class UPawnStatsComponent * _pawnStats);

	void ToggleFireMode(bool _val);

	void ToggleCommandList(bool _val);

	void SetCurrentWeaponImage(const class AGear* _gear);

	void OnSelectedCrewAt(int _index);

};
