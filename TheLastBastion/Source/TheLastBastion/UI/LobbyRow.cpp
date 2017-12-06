// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyRow.h"
#include "GI_TheLastBastion.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"



bool ULobbyRow::Initialize()
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

	bool bAllWidgetsAreGood = Ping && Players && ServerName && Lobby;
	if (bAllWidgetsAreGood == false)
		return false;

	Lobby->OnClicked.AddDynamic(this, &ULobbyRow::OnLobbyClick);

	return true;
}

void ULobbyRow::MakeThisAHeader()
{
	Lobby->SetIsEnabled(false);
}

void ULobbyRow::SetRowProperty(const FText & _ServerName, const FText & _Players, const FText & _ping, int _index)
{
	ServerName->SetText(_ServerName);
	Players->SetText(_Players);
	Ping->SetText(_ping);
	index = _index;
}

void ULobbyRow::OnLobbyClick()
{
	UE_LOG(LogTemp, Warning, TEXT("%d, Lobby Row Pick"), index);
	mGameInstanceRef->JoinServer(index);
}

