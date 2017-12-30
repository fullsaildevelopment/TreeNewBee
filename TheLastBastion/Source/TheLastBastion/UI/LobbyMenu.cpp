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
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GI_TheLastBastion.h"

bool ULobbyMenu::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Check all widget
	bool bAllWidgetAreGood =
		ToggleLeft && ToggleRight && MapName_Text && MapDescription_Text && PlayerNumber_Text &&
		LobbyName_Text && StartMatch_Text && StartReady && ChooseCharacter && Leave && Back &&
		MenuSwitch && PlayerWindow && HeroStats && HeroDescription_Text && ChatText;

	if (bAllWidgetAreGood)
	{
		Leave->OnClicked.AddDynamic(this, &ULobbyMenu::OnLeaveClicked);
		StartReady->OnClicked.AddDynamic(this, &ULobbyMenu::OnStartReadyClicked);
		ChooseCharacter->OnClicked.AddDynamic(this, &ULobbyMenu::OnChooseCharacterClicked);
		Back->OnClicked.AddDynamic(this, &ULobbyMenu::OnBackClicked);
	}
	else
		return false;



	// Init Components

	if (UKismetSystemLibrary::IsServer(this))
	{
		StartMatch_Text->SetText(FText::FromString(TEXT("Start Match")));
		StartReady->SetIsEnabled(false);
	}
	else
		StartMatch_Text->SetText(FText::FromString(TEXT("Ready")));

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

void ULobbyMenu::OnBackClicked()
{
	MenuSwitch->SetActiveWidgetIndex(0);
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





