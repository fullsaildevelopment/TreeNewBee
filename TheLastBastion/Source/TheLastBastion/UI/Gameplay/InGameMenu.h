// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenu.generated.h"

/**
 * 
 */
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


private:

	MenuType CurrentMenuIndex;

	
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

};
