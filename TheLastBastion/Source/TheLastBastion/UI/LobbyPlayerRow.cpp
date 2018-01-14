// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyPlayerRow.h"
#include "Components/TextBlock.h"
#include "CustomType.h"


bool ULobbyPlayerRow::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		PlayerStatus != nullptr && PlayerName != nullptr;

	if (!bAllWidgetAreGood)
	{
		UE_LOG(LogTemp, Error, TEXT("bAllWidgetAreGood is null"));
		return false;
	}
	return true;
}


void ULobbyPlayerRow::FillLobbyPlayerRow(const FPlayerProfile& _incomingPlayer)
{
	PlayerStatus->SetText(_incomingPlayer.mPlayerStatus);
	PlayerName->SetText(_incomingPlayer.mPlayerName);
	mPlayerAvatar = _incomingPlayer.mAvatarImage;
	mCharacterAvatar = _incomingPlayer.mCharacterImage;
}
