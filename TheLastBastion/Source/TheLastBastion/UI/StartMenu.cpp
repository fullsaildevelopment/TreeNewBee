// Fill out your copyright notice in the Description page of Project Settings.

#include "StartMenu.h"


#include "StartMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/KismetMathLibrary.h"
#include "GI_TheLastBastion.h"


bool UStartMenu::Initialize()
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
	bool bWidgetComponents = 
		(PlayButton && CustomizeButton && OptionsButton && CreditsButton && QuitButton) &&
		(HostButton && JoinButton) && (SteamButton && LanButton) && BackButton && ListSwitcher;

	if (bWidgetComponents == false)
		return false;

	// Assign delegates
	PlayButton->OnClicked.AddDynamic(this, &UStartMenu::OnPlayButtonClick);
	CustomizeButton->OnClicked.AddDynamic(this, &UStartMenu::OnCustomizeButtonClick);
	OptionsButton->OnClicked.AddDynamic(this, &UStartMenu::OnOptionButtonClick);
	CreditsButton->OnClicked.AddDynamic(this, &UStartMenu::OnCreditsButtonClick);
	QuitButton->OnClicked.AddDynamic(this, &UStartMenu::OnQuitButtonClick);

	HostButton->OnClicked.AddDynamic(this, &UStartMenu::OnHostButtonClick);
	JoinButton->OnClicked.AddDynamic(this, &UStartMenu::OnJoinButtonClick);

	SteamButton->OnClicked.AddDynamic(this, &UStartMenu::OnSteamButtonClick);
	LanButton  ->OnClicked.AddDynamic(this, &UStartMenu::OnLanButtonClick);

	BackButton->OnClicked.AddDynamic(this, &UStartMenu::OnBackButtonClick);

	// Init Button
	ListSwitcher->SetActiveWidgetIndex(0);
	BackButton->SetVisibility(ESlateVisibility::Hidden);
	return true;
}

void UStartMenu::OnHostButtonClick()
{
	this->RemoveFromParent();
	mGameInstanceRef->ShowHostMenu();
}

void UStartMenu::OnJoinButtonClick()
{
	this->RemoveFromParent();
	mGameInstanceRef->ShowJoinMenu();

}

void UStartMenu::OnSteamButtonClick()
{	
	mGameInstanceRef->SetIsLan(false);
	ListSwitcher->SetActiveWidgetIndex(HostJoinSelectionList);
}

void UStartMenu::OnLanButtonClick()
{
	
	mGameInstanceRef->SetIsLan(true);
    ListSwitcher->SetActiveWidgetIndex(HostJoinSelectionList);
}

void UStartMenu::OnBackButtonClick()
{
	int widgetIndex = ListSwitcher->GetActiveWidgetIndex();
	widgetIndex--;
	if (widgetIndex <= 0)
	{
		widgetIndex = 0;
		BackButton->SetVisibility(ESlateVisibility::Hidden);
	}
	ListSwitcher->SetActiveWidgetIndex(widgetIndex);
}


#pragma region Main List Func
void UStartMenu::OnPlayButtonClick()
{
	ListSwitcher->SetActiveWidgetIndex(ConnectionSelectionList);
	BackButton->SetVisibility(ESlateVisibility::Visible);
}

void UStartMenu::OnCustomizeButtonClick()
{
	this->RemoveFromParent();
	mGameInstanceRef->ShowCustomizeMenu();
}

void UStartMenu::OnOptionButtonClick()
{
	//this->RemoveFromParent();
	//mGameInstanceRef->ShowOptionMenu();
}

void UStartMenu::OnCreditsButtonClick()
{
}

void UStartMenu::OnQuitButtonClick()
{
	APlayerController* TargetPC = GetOwningPlayer();
	if (TargetPC)
		TargetPC->ConsoleCommand("quit");
}

#pragma endregion



