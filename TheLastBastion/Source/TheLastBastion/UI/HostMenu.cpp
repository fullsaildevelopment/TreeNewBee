// Fill out your copyright notice in the Description page of Project Settings.

#include "HostMenu.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Kismet/KismetMathLibrary.h"
#include "GI_TheLastBastion.h"



bool UHostMenu::Initialize()
{

	if (Super::Initialize() == false)
		return false;

	// Get Ref from Game Instance
	APlayerController* const pc = GetOwningPlayer();
	if (pc)
	{
		mGameInstanceRef = Cast<UGI_TheLastBastion>(pc->GetGameInstance());
		if (mGameInstanceRef == nullptr)
			return false;
	}
	else
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		Accept != nullptr && Back != nullptr && ToggleLeft != nullptr && ToggleRight != nullptr &&
		LobbyName != nullptr;

	if (bAllWidgetAreGood)
	{
		Accept->OnClicked.AddDynamic(this, &UHostMenu::OnAcceptClick);
		Back->OnClicked.AddDynamic(this, &UHostMenu::OnBackClick);
		ToggleLeft->OnClicked.AddDynamic(this, &UHostMenu::OnToggleLeftClick);
		ToggleRight->OnClicked.AddDynamic(this, &UHostMenu::OnToggleRightClick);
		LobbyName->OnTextCommitted.AddDynamic(this, &UHostMenu::OnLobbyNameChangedWithEnterCommit);
	}
	else
		return false;

	// Init Components
	Accept->SetVisibility(ESlateVisibility::Hidden);
	numOfPlayers = 1;
	NumberOfPlayers->SetText(FText::AsNumber(numOfPlayers));
	return true;
}
void UHostMenu::OnBackClick()
{
	this->RemoveFromParent();
	mGameInstanceRef->ShowMainMenu();
}

void UHostMenu::OnAcceptClick()
{
	mGameInstanceRef->LaunchLobby(numOfPlayers);
}

void UHostMenu::OnToggleLeftClick()
{
	numOfPlayers--;
	numOfPlayers = UKismetMathLibrary::Clamp(numOfPlayers, NumOfPlayers_Min, NumOfPlayers_Max);
	NumberOfPlayers->SetText(FText::AsNumber(numOfPlayers));
}

void UHostMenu::OnToggleRightClick()
{
	numOfPlayers++;
	numOfPlayers = UKismetMathLibrary::Clamp(numOfPlayers, NumOfPlayers_Min, NumOfPlayers_Max);
	NumberOfPlayers->SetText(FText::AsNumber(numOfPlayers));
}

void UHostMenu::OnLobbyNameChangedWithEnterCommit(const FText & _text, ETextCommit::Type _commitMethod)
{
	if (_commitMethod == ETextCommit::Type::OnEnter)
	{
		if (!_text.IsEmpty())
		{
			Accept->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
