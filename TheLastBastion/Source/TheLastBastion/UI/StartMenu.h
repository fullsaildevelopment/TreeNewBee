// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartMenu.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UStartMenu : public UUserWidget
{

	GENERATED_BODY()

	enum ListToSwitch { MainList = 0,  ConnectionSelectionList = 1, HostJoinSelectionList= 2};
	
protected:

	virtual bool Initialize();

public:

	void UseFirstSlide();

private:

	UPROPERTY()
		class UGI_TheLastBastion* mGameInstanceRef;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* ListSwitcher;

	UPROPERTY(meta = (BindWidget))
		class UButton* BackButton;

#pragma region Main List
	UPROPERTY(meta = (BindWidget))
		class UButton* PlayButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* CustomizeButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* OptionsButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* CreditsButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* QuitButton;

#pragma endregion

#pragma region Host Join Selection List
	UPROPERTY(meta = (BindWidget))
		class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* JoinButton;

#pragma endregion

#pragma region Connection Type Selection List

	UPROPERTY(meta = (BindWidget))
		class UButton* SinglePlayerButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* SteamButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* LanButton;

#pragma endregion


private:


#pragma region Widget Binding Funcs
	UFUNCTION()
		void OnPlayButtonClick();

	UFUNCTION()
		void OnCustomizeButtonClick();

	UFUNCTION()
		void OnOptionButtonClick();

	UFUNCTION()
		void OnCreditsButtonClick();

	UFUNCTION()
		void OnQuitButtonClick();


	UFUNCTION()
		void OnHostButtonClick();

	UFUNCTION()
		void OnJoinButtonClick();


	UFUNCTION()
		void OnSinglePlayerButtonClick();

	UFUNCTION()
		void OnSteamButtonClick();

	UFUNCTION()
		void OnLanButtonClick();

	UFUNCTION()
		void OnBackButtonClick();


#pragma endregion

};
