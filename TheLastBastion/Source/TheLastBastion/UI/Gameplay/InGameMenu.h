// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenu.generated.h"

/**
 * 
 */


#define MaxSkillLevel 5
#define ResourceSkillAddAmountOnEachLevel 0.2

USTRUCT()
struct FSkillLevel
{
	GENERATED_BODY()

	UPROPERTY()
		class UTextBlock* Text;
	UPROPERTY()
		int Level;
};


UCLASS()
class THELASTBASTION_API UInGameMenu : public UUserWidget
{
	GENERATED_BODY()
	
	enum MenuType 
	{
		PauseMenu = 0, 
		OptionMenu = 1,
		SkillTree = 2
	};

protected:
	
	bool Initialize() override;

protected:

	UPROPERTY(meta = (BindWidget))
		class UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* ToMainMenuButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* ToggleLeft;

	UPROPERTY(meta = (BindWidget))
		class UButton* ToggleRight;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MenuName;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
		class UCheckBox* HideSH;

	UPROPERTY(meta = (BindWidget))
		class UCheckBox* HideHV;

	UPROPERTY(meta = (BindWidget))
		class UCheckBox* HideCB;

	UPROPERTY(meta = (BindWidget))
		class UCheckBox* HideTH;

	UPROPERTY(meta = (BindWidget))
		class UButton* Accept_Option;


	// skill menu
	UPROPERTY(meta = (BindWidget))
		class UButton* Accept_Skill;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentSurvivalTrainingLevel;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentStaminaTraingingLevel;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentFarmerLevel;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentBuilderLevel;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentMinerLevel;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentSawyerLevel;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentHitThemHardLevel;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentMakeThemSufferLevel;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentFaithLevel;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentLeaderLevel;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentSkillPoints;

	// plus button

	UPROPERTY(meta = (BindWidget))
		class UButton* SurviorPlus_Btn;

	UPROPERTY(meta = (BindWidget))
		class UButton* StaminaPlus_Btn;

	UPROPERTY(meta = (BindWidget))
		class UButton* FarmerPlus_Btn;

	UPROPERTY(meta = (BindWidget))
		class UButton* BuilderPlus_Btn;

	UPROPERTY(meta = (BindWidget))
		class UButton* MinerPlus_Btn;

	UPROPERTY(meta = (BindWidget))
		class UButton* SawyerPlus_Btn;

	UPROPERTY(meta = (BindWidget))
		class UButton* HitThemHardPlus_Btn;

	UPROPERTY(meta = (BindWidget))
		class UButton* MakeThemSufferPlus_Btn;

	UPROPERTY(meta = (BindWidget))
		class UButton* FaithPlus_Btn;

	UPROPERTY(meta = (BindWidget))
		class UButton* LeaderPlus_Btn;

	int CurrentSkillPoints_int;

private:

	MenuType CurrentMenuIndex;

	UPROPERTY()
		TArray<FSkillLevel> AllSkillLevel;
	
protected:

	UFUNCTION()
		void OnQuitButtonClicked();

	UFUNCTION()
		void OnResumeButtonClicked();

	UFUNCTION()
		void OnToMainMenuButtonClicked();

	UFUNCTION()
		void OnToggleLeftClicked();

	UFUNCTION()
		void OnToggleRightClicked();

	UFUNCTION()
		void OnAcceptClicked_Option();

	UFUNCTION()
		void OnHideSHChanged(bool _isChecked);

	UFUNCTION()
		void OnHideCBChanged(bool _isChecked);

	UFUNCTION()
		void OnHideTHChanged(bool _isChecked);

	UFUNCTION()
		void OnHideHVChanged(bool _isChecked);


public:

	void OpenPauseMenu();

private:

	void SwitchMenu(int _val);

	
	void OpenCurrentMenu();

	void OnOpenPauseMenu();

	void OnOpenOptionMenu();

	void OnOpenSkillMenu();
	void LoadSkillsSetFromHero();

	UFUNCTION()
		void OnSurviorPlusClicked();

	UFUNCTION()
		void OnStaminaPlusClicked();

	UFUNCTION()
		void OnFarmerPlusClicked();

	UFUNCTION()
		void OnBuilderPlusClicked();

	UFUNCTION()
		void OnMinerPlusClicked();

	UFUNCTION()
		void OnSawyerPlusClicked();

	UFUNCTION()
		void OnHitThemHardPlusClicked();

	UFUNCTION()
		void OnMakeThemSufferPlusClicked();

	UFUNCTION()
		void OnFaithPlusClicked();

	UFUNCTION()
		void OnLeaderPlusClicked();

	UFUNCTION()
		void OnAcceptClicked_Skill();


	void OnPlusBtnClicked(int _index);


};
