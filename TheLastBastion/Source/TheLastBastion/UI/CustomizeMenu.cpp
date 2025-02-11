// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomizeMenu.h"

#include "GI_TheLastBastion.h"
#include "SaveGame/SaveGame_TheLastBastion.h"
#include "CustomType.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"




UCustomizeMenu::UCustomizeMenu(const FObjectInitializer & _objectInitailizer) : Super(_objectInitailizer)
{


}

bool UCustomizeMenu::Initialize()
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
		PlayerName != nullptr && WelcomeMessage != nullptr;

	if (bAllWidgetAreGood)
	{
		Accept->OnClicked.AddDynamic(this, &UCustomizeMenu::OnAcceptClick);
		Back->OnClicked.AddDynamic(this, &UCustomizeMenu::OnBackClick);
		ToggleLeft->OnClicked.AddDynamic(this, &UCustomizeMenu::OnToggleLeftClick);
		ToggleRight->OnClicked.AddDynamic(this, &UCustomizeMenu::OnToggleRightClick);
		PlayerName->OnTextChanged.AddDynamic(this, &UCustomizeMenu::OnPlayerNameChange);
	}
	else
		return false;

	PlayerProfileSaveCheck();
	return true;

}

void UCustomizeMenu::OnBackClick()
{
	this->RemoveFromParent();
	mGameInstanceRef->ShowMainMenu();
}

void UCustomizeMenu::OnAcceptClick()
{
	if (mSaveGame == nullptr)
		mSaveGame = Cast<USaveGame_TheLastBastion>(UGameplayStatics::CreateSaveGameObject(USaveGame_TheLastBastion::StaticClass()));

	if (mSaveGame)
	{
		mSaveGame->UseDefaultProfile();
		mSaveGame->mPlayerProfile.mAvatarImage = mCurrentAvatar;
		mSaveGame->mPlayerProfile.mPlayerName = PlayerName->GetText();
		UGameplayStatics::SaveGameToSlot(mSaveGame, mGameInstanceRef->GetPlayerSettingsSaveFString(), 0);
		WelcomeMessage->SetVisibility(ESlateVisibility::Hidden);
		Back->SetVisibility(ESlateVisibility::Visible);
		this->RemoveFromParent();
		mGameInstanceRef->ShowMainMenu();
	}
}

void UCustomizeMenu::OnToggleLeftClick()
{
	mCurrentAvatarSelectionIndex--;
	mCurrentAvatarSelectionIndex = UKismetMathLibrary::Clamp(mCurrentAvatarSelectionIndex, 0, allAvatars.Num() - 1);
	mCurrentAvatar = allAvatars[mCurrentAvatarSelectionIndex];

}

void UCustomizeMenu::OnToggleRightClick()
{
	mCurrentAvatarSelectionIndex++;
	mCurrentAvatarSelectionIndex = UKismetMathLibrary::Clamp(mCurrentAvatarSelectionIndex, 0, allAvatars.Num() - 1);
	mCurrentAvatar = allAvatars[mCurrentAvatarSelectionIndex];
}

void UCustomizeMenu::OnPlayerNameChange(const FText& _text)
{
     if (!_text.IsEmpty())
     {
     	Accept->SetIsEnabled(true);
     }
     else
     {
     	Accept->SetIsEnabled(false);
     }
}

void UCustomizeMenu::PlayerProfileSaveCheck()
{
	mSaveGame = mGameInstanceRef->LoadSaveGame();
	mCurrentAvatarSelectionIndex = 0;
	if (mSaveGame)
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Save Game, Load Save Game"));
		PlayerName->SetText(mSaveGame->mPlayerProfile.mPlayerName);
		mCurrentAvatar = mSaveGame->mPlayerProfile.mAvatarImage;

		WelcomeMessage->SetVisibility(ESlateVisibility::Hidden);
		Accept->SetIsEnabled(true);

	}
	else
	{
		// Set instruction message to visible, and hide back button 
		// force player to finish complete player profile
		UE_LOG(LogTemp, Warning, TEXT("Did not find save game"));
		mCurrentAvatar = allAvatars[mCurrentAvatarSelectionIndex];
		WelcomeMessage->SetVisibility(ESlateVisibility::Visible);
		Back->SetVisibility(ESlateVisibility::Hidden);
	}
}


