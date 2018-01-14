// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyMenu.h"
#include "Engine.h"
#include "PCs/LobbyPC.h"
#include "Engine/World.h"
#include "GameMode/LobbyGM.h"
#include "GI_TheLastBastion.h"
#include "UI/LobbyPlayerRow.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "LobbyPawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TheLastBastionCharacter.h"



#define RANGER_DESCRIPTION  TEXT("Trained for close combat, and efficient with melee weapon, a survior")
#define BUILDER_DESCRIPTION TEXT("Trained for base defence, and efficient with long range weapon, a marksman and mechanic")

bool ULobbyMenu::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Check all widget
	bool bAllWidgetAreGood =
		MapName_Text && PlayerNumber_Text && ClassName_Text && Identity_Text && LobbyName_Text && StartMatch_Text
		&& StartReady && ChooseCharacter && Leave && Accept
		&& MenuSwitch && PlayerWindow 
		&& HeroDescription_Text && MapDescription_Text && IdentityDescription_Text
		&& ToggleLeft_Map && ToggleRight_Map && ToggleLeft_CharacterClass && ToggleRight_CharacterClass && ToggleLeft_Born && ToggleRight_Born
		&& VitalityValue && StaminaValue && StrengthValue && EnduranceValue && ResistenceValue && FaithValue;

	if (bAllWidgetAreGood)
	{
		Leave->OnClicked.AddDynamic(this, &ULobbyMenu::OnLeaveClicked);
		StartReady->OnClicked.AddDynamic(this, &ULobbyMenu::OnStartReadyClicked);
		ChooseCharacter->OnClicked.AddDynamic(this, &ULobbyMenu::OnChooseCharacterClicked);
		Accept->OnClicked.AddDynamic(this, &ULobbyMenu::OnAcceptClicked);
		ToggleLeft_CharacterClass->OnClicked.AddDynamic(this, &ULobbyMenu::OnToggleLeft_CharacterClassClicked);
		ToggleRight_CharacterClass->OnClicked.AddDynamic(this, &ULobbyMenu::OnToggleRight_CharacterClassClicked);
	}
	else
		return false;

	// Init Components
	if (UKismetSystemLibrary::IsServer(this))
	{
		StartMatch_Text->SetText(FText::FromString(TEXT("Start Match")));
		
		ALobbyGM* gm = Cast<ALobbyGM>(UGameplayStatics::GetGameMode(this));
		if (gm)
		{
			if (gm->GetMaxNumOfPlayers() > 1)
				StartReady->SetIsEnabled(false);
		}
	}
	else
		StartMatch_Text->SetText(FText::FromString(TEXT("Ready")));

	// Init
	ToggleLeft_Map->SetIsEnabled(false);
	ToggleRight_Map->SetIsEnabled(false);

	// Choose Ranger By Default
	OnToggleLeft_CharacterClassClicked();
	return true;
}

void ULobbyMenu::OnLeaveClicked()
{

	if (UKismetSystemLibrary::IsServer(this))
	{
		// if the server is clicked on leaving,
		// kick out all the clients, and leave


		ALobbyGM* gm = Cast<ALobbyGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (gm == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("gm is null -- ULobbyMenu::OnLeaveClicked Server call "));
			return;
		}
		TArray<APlayerController*> allPCs = gm->GetAllPlayerControllers();
		for (int iPc = 0; iPc < allPCs.Num(); iPc++)
		{
			Cast<ALobbyPC>(allPCs[iPc])->CLIENT_OnQuitLobby();
		}
	}
	else
	{
		ALobbyPC* pc = Cast<ALobbyPC>(this->GetOwningPlayer());
		if (pc == nullptr)
		{
			UE_LOG(LogTemp, Error,
				TEXT("pc is null - OnStartReadyClicked"));
			return;
		}
		// Let Server know I quit
		pc->SERVER_LeaveLobbyAndUpdateConnectedPlayer();

		// Quit
		pc->CLIENT_OnQuitLobby();
	}
}

void ULobbyMenu::OnStartReadyClicked()
{
	if (UKismetSystemLibrary::IsServer(this))
	{
		// if this is a server, we can start the match if the everyone is ready	
		ALobbyGM* gm = Cast<ALobbyGM>(UGameplayStatics::GetGameMode(this));
		gm->StartGame();
	}
	else
	{
		// toggle the Choose Character button, if the client is ready, then he should not change character
		ChooseCharacter->SetIsEnabled(!ChooseCharacter->GetIsEnabled());

		// call server to update this clients status, enable the start match button if every one is ready
		ALobbyPC* pc = Cast<ALobbyPC>(this->GetOwningPlayer());
		if (pc == nullptr)
		{
			UE_LOG(LogTemp, Error,
				TEXT("pc is null - OnStartReadyClicked"));
			return;
		}
		pc->SERVER_OnClientReadyButtonClicked();
	}
}

void ULobbyMenu::OnChooseCharacterClicked()
{
	MenuSwitch->SetActiveWidgetIndex(1);
}

void ULobbyMenu::OnAcceptClicked()
{

	ALobbyPC* pc = Cast<ALobbyPC>(GetOwningPlayer());
	if (pc)
	{
		MenuSwitch->SetActiveWidgetIndex(0);
		pc->SetCharacterClass(mCharacterClassSelection);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("pc is Null - ULobbyMenu::OnAcceptClicked"));
	}
}

void ULobbyMenu::OnToggleLeft_CharacterClassClicked()
{
	mCharacterClassSelection = ECharacterType::Ranger;
	ToggleLeft_CharacterClass->SetIsEnabled(false);
	ToggleRight_CharacterClass->SetIsEnabled(true);

	HeroDescription_Text->SetText(FText::FromString(RANGER_DESCRIPTION));
	ClassName_Text->SetText(FText::FromString(TEXT("Ranger")));

	VitalityValue->SetText(FText::AsNumber(230));
	StaminaValue->SetText(FText::AsNumber(120));
	StrengthValue->SetText(FText::AsNumber(21));
	EnduranceValue->SetText(FText::AsNumber(17));
	ResistenceValue->SetText(FText::AsNumber(25));
	FaithValue->SetText(FText::AsNumber(23));

	ALobbyPC* pc = Cast<ALobbyPC>(GetOwningPlayer());
	if (pc)
	{
		ALobbyPawn* lobbyPawn = Cast<ALobbyPawn>(pc->GetPawn());
		if (lobbyPawn)
		{
			lobbyPawn->SetIsCheckingRanger(true);
		}
	}

}

void ULobbyMenu::OnToggleRight_CharacterClassClicked()
{
	mCharacterClassSelection = ECharacterType::Builder;
	ToggleRight_CharacterClass->SetIsEnabled(false);
	ToggleLeft_CharacterClass->SetIsEnabled(true);

	HeroDescription_Text->SetText(FText::FromString(BUILDER_DESCRIPTION));
	ClassName_Text->SetText(FText::FromString(TEXT("Builder")));

	VitalityValue->SetText(FText::AsNumber(210));
	StaminaValue->SetText(FText::AsNumber(100));
	StrengthValue->SetText(FText::AsNumber(19));
	EnduranceValue->SetText(FText::AsNumber(15));
	ResistenceValue->SetText(FText::AsNumber(21));
	FaithValue->SetText(FText::AsNumber(23));


	ALobbyPC* pc = Cast<ALobbyPC>(GetOwningPlayer());
	if (pc)
	{
		ALobbyPawn* lobbyPawn = Cast<ALobbyPawn>(pc->GetPawn());
		if (lobbyPawn)
		{
			lobbyPawn->SetIsCheckingRanger(false);
		}
	}

}

void ULobbyMenu::SetLobbyName(const FText& _lobbyName)
{
	LobbyName_Text->SetText(_lobbyName);
}

void ULobbyMenu::SetCurrentPlayersNumber(int _currentNumOfPlayer, int _maxNumOfPlayers)
{
	TArray<FStringFormatArg> formatArray;
	formatArray.Add(_currentNumOfPlayer);
	formatArray.Add(_maxNumOfPlayers);

	FString players = FString::Format(TEXT("{0} / {1}"), formatArray);
	PlayerNumber_Text->SetText(FText::FromString(players));

}

void ULobbyMenu::AddPlayerToPlayerList(const TArray<struct FPlayerProfile>& _connectedPlayers)
{
	PlayerWindow->ClearChildren();

	UGI_TheLastBastion* gi = Cast<UGI_TheLastBastion>(UGameplayStatics::GetGameInstance(this));
	ULobbyPlayerRow* newRow = nullptr;
	for (int iPlayer = 0; iPlayer < _connectedPlayers.Num(); iPlayer++)
	{
		newRow
			= Cast<ULobbyPlayerRow>(CreateWidget<UUserWidget>(GetOwningPlayer(), gi->GetLobbyPlayerRow_Class()));
		newRow->FillLobbyPlayerRow(_connectedPlayers[iPlayer]);
		PlayerWindow->AddChild(newRow);
	}

}

void ULobbyMenu::SetStartMatchButtonEnabled(bool _val)
{
	StartReady->SetIsEnabled(_val);
}





