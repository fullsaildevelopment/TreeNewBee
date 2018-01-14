// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyMenu.generated.h"

/**
*
*/
enum class ECharacterType : uint8;

UCLASS()
class THELASTBASTION_API ULobbyMenu : public UUserWidget
{
	GENERATED_BODY()


private:

	UPROPERTY()
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
		class UTextBlock* ClassName_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Identity_Text;



	/** Toggles */
	UPROPERTY(meta = (BindWidget))
		class UButton* ToggleLeft_Map;
	UPROPERTY(meta = (BindWidget))
		class UButton* ToggleRight_Map;
	UPROPERTY(meta = (BindWidget))
		class UButton* ToggleLeft_CharacterClass;
	UPROPERTY(meta = (BindWidget))
		class UButton* ToggleRight_CharacterClass;
	UPROPERTY(meta = (BindWidget))
		class UButton* ToggleLeft_Born;
	UPROPERTY(meta = (BindWidget))
		class UButton* ToggleRight_Born;

	/** Buttons*/
	UPROPERTY(meta = (BindWidget))
		class UButton* StartReady;
	UPROPERTY(meta = (BindWidget))
		class UButton* ChooseCharacter;
	UPROPERTY(meta = (BindWidget))
		class UButton* Leave;
	//UPROPERTY(meta = (BindWidget))
	//	class UButton* Back;
	UPROPERTY(meta = (BindWidget))
		class UButton* Accept;

	/** Description */
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MapDescription_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* HeroDescription_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* IdentityDescription_Text;


	/** Stats Value*/

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* VitalityValue;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* StaminaValue;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* StrengthValue;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* EnduranceValue;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ResistenceValue;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* FaithValue;



	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* MenuSwitch;
	UPROPERTY(meta = (BindWidget))
		class UVerticalBox* PlayerWindow;


	




protected:

	bool Initialize() override;

private:

	UFUNCTION()
		void OnLeaveClicked();

	UFUNCTION()
		void OnStartReadyClicked();

	UFUNCTION()
		void OnChooseCharacterClicked();

	//UFUNCTION()
	//	void OnBackClicked();
	UFUNCTION()
		void OnAcceptClicked();


	UFUNCTION()
		void OnToggleLeft_CharacterClassClicked();

	UFUNCTION()
		void OnToggleRight_CharacterClassClicked();

private:
	
	UPROPERTY()
		ECharacterType mCharacterClassSelection;

	

public:

	void SetLobbyName(const FText& _lobbyName);

	void SetCurrentPlayersNumber(int _currentNumOfPlayer, int _maxNumOfPlayers);

	void AddPlayerToPlayerList(const TArray<struct FPlayerProfile>& _connectedPlayers);

	void SetStartMatchButtonEnabled(bool _val);



};
