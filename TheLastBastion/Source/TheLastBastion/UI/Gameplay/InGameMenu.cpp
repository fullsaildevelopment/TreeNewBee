// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameMenu.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "GI_TheLastBastion.h"
#include "Combat/HeroStatsComponent.h"
#include "TheLastBastionHeroCharacter.h"


#define MENUCOUNT 3
const FString MenuNameList[MENUCOUNT] = { TEXT("Pause Menu"), TEXT("Option Menu"), TEXT("Skill Menu") };

bool UInGameMenu::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		ResumeButton && ResumeButton && ToMainMenuButton && 
		MenuName && ToggleLeft && ToggleRight &&
		HideSH && HideHV && HideTH && HideCB && Accept_Option;

	if (bAllWidgetAreGood)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UInGameMenu::OnResumeButtonClicked);
		QuitButton->OnClicked.AddDynamic(this, &UInGameMenu::OnQuitButtonClicked);
		ToMainMenuButton->OnClicked.AddDynamic(this, &UInGameMenu::OnToMainMenuButtonClicked);

		ToggleLeft->OnClicked.AddDynamic(this, &UInGameMenu::OnToggleLeftClicked);
		ToggleRight->OnClicked.AddDynamic(this, &UInGameMenu::OnToggleRightClicked);

		Accept_Option->OnClicked.AddDynamic(this, &UInGameMenu::OnAcceptClicked_Option);
		HideSH->OnCheckStateChanged.AddDynamic(this, &UInGameMenu::OnHideSHChanged);
		HideHV->OnCheckStateChanged.AddDynamic(this, &UInGameMenu::OnHideHVChanged);
		HideTH->OnCheckStateChanged.AddDynamic(this, &UInGameMenu::OnHideTHChanged);
		HideCB->OnCheckStateChanged.AddDynamic(this, &UInGameMenu::OnHideCBChanged);

	}
	else
		return false;

	return true;
}


void UInGameMenu::OnQuitButtonClicked()
{
	APlayerController* TargetPC = GetOwningPlayer();
	if (TargetPC)
		TargetPC->ConsoleCommand("quit");
}

void UInGameMenu::OnResumeButtonClicked()
{
	APlayerController* pc = GetOwningPlayer();
	this->RemoveFromParent();
	if (pc)
	{
		pc->bShowMouseCursor = false;
		FInputModeGameOnly inputMode;
		pc->SetPause(false);
		pc->SetInputMode(inputMode);
	}

}

void UInGameMenu::OnToMainMenuButtonClicked()
{
	APlayerController* pc = GetOwningPlayer();
	if (pc)
	{
		UGI_TheLastBastion* gi = Cast<UGI_TheLastBastion>(pc->GetGameInstance());
		if (gi)
		{
			gi->DisplayLoadingScreen();
			gi->BackToMainMenu();
		}
	}

}

void UInGameMenu::OnToggleLeftClicked()
{
	SwitchMenu(-1);
}

void UInGameMenu::OnToggleRightClicked()
{
	SwitchMenu(1);
}

void UInGameMenu::OnAcceptClicked_Option()
{
	APlayerController* pc = GetOwningPlayer();
	if (pc == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("pc is null - UInGameMenu::OnOpenOptionMenu"));
		return;
	}
	ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(pc->GetCharacter());
	if (hero == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("hero is null - UInGameMenu::OnOpenOptionMenu"));
		return;
	}
	UHeroStatsComponent* heroStats = hero->GetHeroStatsComp();
	if (heroStats == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("heroStats is null - UInGameMenu::OnOpenOptionMenu"));
		return;
	}


	if (HideSH->GetCheckedState() == ECheckBoxState::Checked)
		heroStats->SetWeaponEquipVisibility(0, true);
	else
		heroStats->SetWeaponEquipVisibility(0, false);

	if (HideTH->GetCheckedState() == ECheckBoxState::Checked)
		heroStats->SetWeaponEquipVisibility(1, true);
	else
		heroStats->SetWeaponEquipVisibility(1, false);

	if (HideCB->GetCheckedState() == ECheckBoxState::Checked)
		heroStats->SetWeaponEquipVisibility(2, true);
	else
		heroStats->SetWeaponEquipVisibility(2, false);

	if (HideHV->GetCheckedState() == ECheckBoxState::Checked)
		heroStats->SetWeaponEquipVisibility(3, true);
	else
		heroStats->SetWeaponEquipVisibility(3, false);

	//heroStats->SetWeaponEquipVisibility(0, (bool)HideSH->GetCheckedState());
	//heroStats->SetWeaponEquipVisibility(1, (bool)HideTH->GetCheckedState());
	//heroStats->SetWeaponEquipVisibility(2, (bool)HideCB->GetCheckedState());
	//heroStats->SetWeaponEquipVisibility(3, (bool)HideHV->GetCheckedState());
	heroStats->UpdateEquipHideOption();
}

void UInGameMenu::OnHideSHChanged(bool _isChecked)
{
}

void UInGameMenu::OnHideCBChanged(bool _isChecked)
{
}

void UInGameMenu::OnHideTHChanged(bool _isChecked)
{
}

void UInGameMenu::OnHideHVChanged(bool _isChecked)
{
}

void UInGameMenu::OpenPauseMenu()
{
	CurrentMenuIndex = MenuType::PauseMenu;
	OpenCurrentMenu();
}

void UInGameMenu::SwitchMenu(int _val)
{
	int index = (int)(CurrentMenuIndex) + _val;
	if (index < 0)
		CurrentMenuIndex = MenuType::SkillTree;
	else if (index >= MENUCOUNT)
		CurrentMenuIndex = MenuType::PauseMenu;
	else
		CurrentMenuIndex = (MenuType)index;

	OpenCurrentMenu();
}

void UInGameMenu::OpenCurrentMenu()
{
	MenuSwitcher->SetActiveWidgetIndex((int)CurrentMenuIndex);

	switch (CurrentMenuIndex)
	{
	case UInGameMenu::PauseMenu:
		OnOpenPauseMenu();
		break;
	case UInGameMenu::OptionMenu:
		OnOpenOptionMenu();
		break;
	case UInGameMenu::SkillTree:
		OnOpenSkillMenu();
		break;
	}

}

void UInGameMenu::OnOpenPauseMenu()
{
	MenuName->SetText(FText::FromString(MenuNameList[(int)CurrentMenuIndex]));
}

void UInGameMenu::OnOpenOptionMenu()
{
	MenuName->SetText(FText::FromString(MenuNameList[(int)CurrentMenuIndex]));
	// Update UI to current option settings
	APlayerController* pc = GetOwningPlayer();
	if (pc == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("pc is null - UInGameMenu::OnOpenOptionMenu"));
		return;
	}
	ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(pc->GetCharacter());
	if (hero == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("hero is null - UInGameMenu::OnOpenOptionMenu"));
		return;
	}
	UHeroStatsComponent* heroStats = hero->GetHeroStatsComp();
	if (heroStats == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("heroStats is null - UInGameMenu::OnOpenOptionMenu"));
		return;
	}

	HideSH->SetCheckedState((ECheckBoxState)(heroStats->GetWeaponSlotAt(0).bHideWhenEquip));
	HideTH->SetCheckedState((ECheckBoxState)(heroStats->GetWeaponSlotAt(1).bHideWhenEquip));
	HideCB->SetCheckedState((ECheckBoxState)(heroStats->GetWeaponSlotAt(2).bHideWhenEquip));
	HideHV->SetCheckedState((ECheckBoxState)(heroStats->GetWeaponSlotAt(3).bHideWhenEquip));
}

void UInGameMenu::OnOpenSkillMenu()
{
	MenuName->SetText(FText::FromString(MenuNameList[(int)CurrentMenuIndex]));

}
