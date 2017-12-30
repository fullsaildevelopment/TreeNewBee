// Fill out your copyright notice in the Description page of Project Settings.

#include "JoinMenu.h"
#include "GI_TheLastBastion.h"
#include "CustomType.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ScrollBox.h"
#include "OnlineSessionSettings.h"


#include "LobbyRow.h"

bool UJoinMenu::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Get Reference for game instance
	APlayerController* const pc = GetOwningPlayer();
	if (pc)
	{
		mGameInstanceRef = Cast<UGI_TheLastBastion>(pc->GetGameInstance());
		if (mGameInstanceRef == nullptr)
			return false;
	}
	else
		return false;

	// Bind Delegate for each widget components
	bool bWidgetComponents = Back && LobbyList && ListSwitcher; //Refresh;

	if (bWidgetComponents == false)
		return false;

	Back->OnClicked.AddDynamic(this, &UJoinMenu::OnBackClick);
	Refresh->OnClicked.AddDynamic(this, &UJoinMenu::OnRefreshClick);

	// Refresh the List During Init
	OnRefreshClick();

	return true;
}

void UJoinMenu::OnBackClick()
{
	this->RemoveFromParent();
	mGameInstanceRef->ShowMainMenu();
}

void UJoinMenu::OnRefreshClick()
{
	ListSwitcher->SetActiveWidgetIndex(Searching);
	mGameInstanceRef->FindLobby();
}

void UJoinMenu::PopLobbyList(const TSharedRef<class FOnlineSessionSearch>& _searchSettings)
{

	if (_searchSettings->SearchResults.Num() < 1)
	{
		// if no result found
		ListSwitcher->SetActiveWidgetIndex(NoLobby);
		UE_LOG(LogTemp, Warning, TEXT("not Find Session"));

	}
	else
	{
		LobbyList->ClearChildren();
		ULobbyRow* lobbyRow = nullptr;
		int currentPlayer = 0, lobbyPlayers = 0;
		for (int i = 0; i < _searchSettings->SearchResults.Num(); i++)
		{
			// create lobby row
			lobbyRow = CreateWidget<ULobbyRow>(GetOwningPlayer(), mGameInstanceRef->GetLobbyRow_Class());
			// fill lobby row info
			TArray<FStringFormatArg> formatArray;
			lobbyPlayers = _searchSettings->SearchResults[i].Session.SessionSettings.NumPublicConnections;
			currentPlayer = lobbyPlayers - _searchSettings->SearchResults[i].Session.NumOpenPublicConnections;
			formatArray.Add(currentPlayer);
			formatArray.Add(lobbyPlayers);

			FString players = FString::Format(TEXT("{0} / {1}"), formatArray);

			FString data;
			FString LobbyName;
			FString HostName;

			if (_searchSettings->SearchResults[i].Session.SessionSettings.Get(TEXT("LobbyName"), data))
				LobbyName = data;
			else
				LobbyName = TEXT("NewLobby");

			data.Empty();

			if (_searchSettings->SearchResults[i].Session.SessionSettings.Get(TEXT("UserName"), data))
				HostName = data;
			else
				HostName = TEXT("NewPlayer");


			lobbyRow->SetRowProperty(
				FText::FromString(LobbyName),
				FText::FromString(HostName),
				FText::FromString(players),
				FText::AsNumber(_searchSettings->SearchResults[i].PingInMs), i
			);

			// Add row to list
			LobbyList->AddChild(lobbyRow);
		}
		UE_LOG(LogTemp, Warning, TEXT(" Find Session"));
		ListSwitcher->SetActiveWidgetIndex(ShowLobby);
	}

	UE_LOG(LogTemp, Warning, TEXT(" Find Session Finished"));
}

