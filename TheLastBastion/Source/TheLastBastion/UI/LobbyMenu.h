// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyMenu.generated.h"

/**
*
*/
UCLASS()
class THELASTBASTION_API ULobbyMenu : public UUserWidget
{
	GENERATED_BODY()


private:


	TSubclassOf<class UUserWidget> LobbyPlayerRow_Class;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* LobbyName_Text;
	UPROPERTY(meta = (BindWidget))
		/** How many player this lobby currently have */
		class UTextBlock* PlayerNumber_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MapName_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* StartMatch_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MapDescription_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* HeroDescription_Text;
	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* ChatText;

	UPROPERTY(meta = (BindWidget))
		class UButton* ToggleLeft;
	UPROPERTY(meta = (BindWidget))
		class UButton* ToggleRight;
	UPROPERTY(meta = (BindWidget))
		class UButton* StartReady;
	UPROPERTY(meta = (BindWidget))
		class UButton* ChooseCharacter;
	UPROPERTY(meta = (BindWidget))
		class UButton* Leave;
	UPROPERTY(meta = (BindWidget))
		class UButton* Back;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* MenuSwitch;
	UPROPERTY(meta = (BindWidget))
		class UVerticalBox* PlayerWindow;
	UPROPERTY(meta = (BindWidget))
		class UVerticalBox* HeroStats;


protected:

	bool Initialize() override;

private:

	UFUNCTION()
		void OnLeaveClicked();

	UFUNCTION()
		void OnStartReadyClicked();

	UFUNCTION()
		void OnChooseCharacterClicked();

	UFUNCTION()
		void OnBackClicked();


public:

	void SetLobbyName(const FText& _lobbyName);

	void SetCurrentPlayersNumber(int _currentNumOfPlayer, int _maxNumOfPlayers);

	void AddPlayerToPlayerList(const TArray<struct FPlayerProfile>& _connectedPlayers);

	void SetStartMatchButtonEnabled(bool _val);

};
