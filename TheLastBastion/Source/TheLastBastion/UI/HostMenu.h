// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HostMenu.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UHostMenu : public UUserWidget
{
	GENERATED_BODY()

		const int NumOfPlayers_Min = 1, NumOfPlayers_Max = 4;
	
protected:
	virtual bool Initialize();

	UPROPERTY(meta = (BindWidget))
		class UButton* Accept;
	UPROPERTY(meta = (BindWidget))
		class UButton* Back;
	UPROPERTY(meta = (BindWidget))
		class UButton* ToggleLeft;
	UPROPERTY(meta = (BindWidget))
		class UButton* ToggleRight;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* NumberOfPlayers;
	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* LobbyName;

private:
	UPROPERTY()
		class UGI_TheLastBastion* mGameInstanceRef;

	int numOfPlayers;

private:
	UFUNCTION()
		void OnBackClick();

	UFUNCTION()
		void OnAcceptClick();

	UFUNCTION()
		void OnToggleLeftClick();

	UFUNCTION()
		void OnToggleRightClick();

	UFUNCTION()
		void OnLobbyNameChange(const FText& _text);


	
};
