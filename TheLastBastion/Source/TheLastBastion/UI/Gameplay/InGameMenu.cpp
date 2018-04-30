// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameMenu.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "GI_TheLastBastion.h"

#include "GameMode/SinglePlayerGM.h"
#include "Kismet/GameplayStatics.h"
#include "Environment/Outpost.h"

#include "Combat/HeroStatsComponent.h"
#include "TheLastBastionHeroCharacter.h"
#include "CustomType.h"
#include "UI/Gameplay/SkillMenu_PopUp.h"
#include "SlateBlueprintLibrary.h"

#define MENUCOUNT 3
const FString MenuNameList[MENUCOUNT] = { TEXT("Pause Menu"), TEXT("Option Menu"), TEXT("Skill Menu") };


TSubclassOf<UUserWidget> UInGameMenu::WBP_SkillMenuPopUp = nullptr;

UInGameMenu::UInGameMenu(const FObjectInitializer & ObjectInit) : Super(ObjectInit)
{
	if (WBP_SkillMenuPopUp == nullptr)
	{
		UCustomType::FindClass<UUserWidget>(WBP_SkillMenuPopUp,
			TEXT("/Game/UI/In-Game/WBP_SkillMenu_PopUp"));
	}

}

bool UInGameMenu::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		ResumeButton && ResumeButton && ToMainMenuButton &&
		MenuName && ToggleLeft && ToggleRight &&
		HideSH && HideHV && HideTH && HideCB && Accept_Option &&
		Accept_Skill && CurrentSurvivalTrainingLevel && CurrentStaminaTraingingLevel && CurrentFarmerLevel
		&& CurrentBuilderLevel && CurrentMinerLevel && CurrentSawyerLevel && CurrentHitThemHardLevel &&
		CurrentMakeThemSufferLevel && CurrentFaithLevel && CurrentLeaderLevel && CurrentSkillPoints &&
		SurviorPlus_Btn && StaminaPlus_Btn &&
		FarmerPlus_Btn && BuilderPlus_Btn && MinerPlus_Btn && SawyerPlus_Btn &&
		HitThemHardPlus_Btn && MakeThemSufferPlus_Btn && FaithPlus_Btn && LeaderPlus_Btn;


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

		Accept_Skill->OnClicked.AddDynamic(this, &UInGameMenu::OnAcceptClicked_Skill);
		SurviorPlus_Btn       ->OnClicked.AddDynamic(this, &UInGameMenu::OnSurviorPlusClicked);
		StaminaPlus_Btn       ->OnClicked.AddDynamic(this, &UInGameMenu::OnStaminaPlusClicked);
		FarmerPlus_Btn        ->OnClicked.AddDynamic(this, &UInGameMenu::OnFarmerPlusClicked);
		BuilderPlus_Btn       ->OnClicked.AddDynamic(this, &UInGameMenu::OnBuilderPlusClicked);
		MinerPlus_Btn         ->OnClicked.AddDynamic(this, &UInGameMenu::OnMinerPlusClicked);
		SawyerPlus_Btn        ->OnClicked.AddDynamic(this, &UInGameMenu::OnSawyerPlusClicked);
		HitThemHardPlus_Btn   ->OnClicked.AddDynamic(this, &UInGameMenu::OnHitThemHardPlusClicked);
		MakeThemSufferPlus_Btn->OnClicked.AddDynamic(this, &UInGameMenu::OnMakeThemSufferPlusClicked);
		FaithPlus_Btn         ->OnClicked.AddDynamic(this, &UInGameMenu::OnFaithPlusClicked);
		LeaderPlus_Btn        ->OnClicked.AddDynamic(this, &UInGameMenu::OnLeaderPlusClicked);


		SurviorPlus_Btn->OnHovered.AddDynamic(this, &UInGameMenu::OnSurviorPlusHovered);
		StaminaPlus_Btn->OnHovered.AddDynamic(this, &UInGameMenu::OnStaminaPlusHovered);
		FarmerPlus_Btn->OnHovered.AddDynamic(this, &UInGameMenu::OnFarmerPlusHovered);
		BuilderPlus_Btn->OnHovered.AddDynamic(this, &UInGameMenu::OnBuilderPlusHovered);
		MinerPlus_Btn->OnHovered.AddDynamic(this, &UInGameMenu::OnMinerPlusHovered);
		SawyerPlus_Btn->OnHovered.AddDynamic(this, &UInGameMenu::OnSawyerPlusHovered);
		HitThemHardPlus_Btn->OnHovered.AddDynamic(this, &UInGameMenu::OnHitThemHardPlusHovered);
		MakeThemSufferPlus_Btn->OnHovered.AddDynamic(this, &UInGameMenu::OnMakeThemSufferPlusHovered);
		FaithPlus_Btn->OnHovered.AddDynamic(this, &UInGameMenu::OnFaithPlusHovered);
		LeaderPlus_Btn->OnHovered.AddDynamic(this, &UInGameMenu::OnLeaderPlusHovered);



		SurviorPlus_Btn->OnUnhovered.AddDynamic(this, &UInGameMenu::OnPlusUnHovered);
		StaminaPlus_Btn->OnUnhovered.AddDynamic(this, &UInGameMenu::OnPlusUnHovered);
		FarmerPlus_Btn->OnUnhovered.AddDynamic(this, &UInGameMenu::OnPlusUnHovered);
		BuilderPlus_Btn->OnUnhovered.AddDynamic(this, &UInGameMenu::OnPlusUnHovered);
		MinerPlus_Btn->OnUnhovered.AddDynamic(this, &UInGameMenu::OnPlusUnHovered);
		SawyerPlus_Btn->OnUnhovered.AddDynamic(this, &UInGameMenu::OnPlusUnHovered);
		HitThemHardPlus_Btn->OnUnhovered.AddDynamic(this, &UInGameMenu::OnPlusUnHovered);
		MakeThemSufferPlus_Btn->OnUnhovered.AddDynamic(this, &UInGameMenu::OnPlusUnHovered);
		FaithPlus_Btn->OnUnhovered.AddDynamic(this, &UInGameMenu::OnPlusUnHovered);
		LeaderPlus_Btn->OnUnhovered.AddDynamic(this, &UInGameMenu::OnPlusUnHovered);
	}	
	else
	{
		UE_LOG(LogTemp, Error, TEXT("bAllWidgetAreGood == false, UInGameMenu::Initialize"));
		return false;

	}

	AllSkillLevel.SetNum(SkillNum);
	AllSkillLevel[SurvivalTrainingLevel].Text = CurrentSurvivalTrainingLevel;
	AllSkillLevel[SurvivalTrainingLevel].Level = 0;
	AllSkillLevel[SurvivalTrainingLevel].Button = SurviorPlus_Btn;

	AllSkillLevel[StaminaTraingingLevel].Text = CurrentStaminaTraingingLevel;
	AllSkillLevel[StaminaTraingingLevel].Level = 0;
	AllSkillLevel[StaminaTraingingLevel].Button = StaminaPlus_Btn;

	AllSkillLevel[FarmerLevel].Text = CurrentFarmerLevel;
	AllSkillLevel[FarmerLevel].Level = 0;
	AllSkillLevel[FarmerLevel].Button = FarmerPlus_Btn;

	AllSkillLevel[BuilderLevel].Text = CurrentBuilderLevel;
	AllSkillLevel[BuilderLevel].Level = 0;
	AllSkillLevel[BuilderLevel].Button = BuilderPlus_Btn;

	AllSkillLevel[MinerLevel].Text = CurrentMinerLevel;
	AllSkillLevel[MinerLevel].Level = 0;
	AllSkillLevel[MinerLevel].Button = MinerPlus_Btn;

	AllSkillLevel[SawyerLevel].Text = CurrentSawyerLevel;
	AllSkillLevel[SawyerLevel].Level = 0;
	AllSkillLevel[SawyerLevel].Button = SawyerPlus_Btn;

	AllSkillLevel[HitThemHardLevel].Text = CurrentHitThemHardLevel;
	AllSkillLevel[HitThemHardLevel].Level = 0;
	AllSkillLevel[HitThemHardLevel].Button = HitThemHardPlus_Btn;

	AllSkillLevel[MakeThemSufferLevel].Text = CurrentMakeThemSufferLevel;
	AllSkillLevel[MakeThemSufferLevel].Level = 0;
	AllSkillLevel[MakeThemSufferLevel].Button = MakeThemSufferPlus_Btn;

	AllSkillLevel[FaithLevel].Text = CurrentFaithLevel;
	AllSkillLevel[FaithLevel].Level = 0;
	AllSkillLevel[FaithLevel].Button = FaithPlus_Btn;

	AllSkillLevel[LeaderLevel].Text = CurrentLeaderLevel;
	AllSkillLevel[LeaderLevel].Level = 0;
	AllSkillLevel[LeaderLevel].Button = LeaderPlus_Btn;

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
	// Set Name
	MenuName->SetText(FText::FromString(MenuNameList[(int)CurrentMenuIndex]));


	// Load CurrentSkillpoint from player
	LoadSkillsSetFromHero();
}

void UInGameMenu::LoadSkillsSetFromHero()
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


	CurrentSkillPoints_int = heroStats->GetSkillPoints();
	CurrentSkillPoints->SetText(FText::AsNumber(CurrentSkillPoints_int));


	int currentSkillLevel = 0;
	for (int i = 0; i < SkillNum; i++)
	{
		currentSkillLevel = heroStats->GetSkillLevelAt(i);
		if (currentSkillLevel != AllSkillLevel[i].Level)
		{
			AllSkillLevel[i].Level = currentSkillLevel;
			AllSkillLevel[i].Text->SetText(FText::AsNumber(currentSkillLevel));
		}
	}

}

void UInGameMenu::OnAcceptClicked_Skill()
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

	// read skiil points
	heroStats->SetSkillPoints(CurrentSkillPoints_int);

	for (int i = 0; i < SkillNum; i++)
	{
		heroStats->SetSkillLevelAt(i, AllSkillLevel[i].Level);
	}

	// apply changes on outpost
	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("gm == nullptr,  UInGameMenu::OnAcceptClicked_Skill"));
		return;
	}

	AOutpost* outPost_Temp = nullptr;
	EOutpostType outpostType_Temp;
	for (int iOutpost = 0; iOutpost < gm->GetAllOutpostAmount(); iOutpost++)
	{
		outPost_Temp = gm->GetOutpostAt(iOutpost);
		outpostType_Temp = outPost_Temp->GetOutpostType();

		switch (outpostType_Temp)
		{
		case EOutpostType::None:
		default:
			break;
		case EOutpostType::Food:
			outPost_Temp->SetAdditionAmount(ResourceSkillAddAmountOnEachLevel * AllSkillLevel[FarmerLevel].Level);
			break;
		case EOutpostType::Wood:
			outPost_Temp->SetAdditionAmount(ResourceSkillAddAmountOnEachLevel * AllSkillLevel[SawyerLevel].Level);
			break;
		case EOutpostType::Metal:
			outPost_Temp->SetAdditionAmount(ResourceSkillAddAmountOnEachLevel * AllSkillLevel[MinerLevel].Level);
			break;
		case EOutpostType::Stone:
			outPost_Temp->SetAdditionAmount(ResourceSkillAddAmountOnEachLevel * AllSkillLevel[BuilderLevel].Level);
			break;
		}
	}

	// apply changes on hero fitness
	heroStats->SetHeroHpRecoverDelayByLevel_Scaler(AllSkillLevel[SurvivalTrainingLevel].Level);
	heroStats->SetDpGain_Scaler(AllSkillLevel[SurvivalTrainingLevel].Level);
	heroStats->SetHeroSpConsumeRateByLevel_Scaler(AllSkillLevel[StaminaTraingingLevel].Level);

	/// apply chanage on combat skill

	// combo - hit them hard, set cool down time, set damage multiplier
	int level = AllSkillLevel[HitThemHardLevel].Level;
	heroStats->SetDamageMultiplierByLevel_Combo(level);
	heroStats->SetComboShootDamageScalerByLevel(level);
	hero->SetSkillCoolDownTimeByLevelAt(Skill__Combo, level);

	// powerHit - hit them hard, set cool down time, set damage multiplier
	level = AllSkillLevel[MakeThemSufferLevel].Level;
	heroStats->SetDamageMultiplierByLevel_PowerHit(level);
	heroStats->SetExtraProjectileAmountByLevel(level);
	heroStats->SetPowerShotBulletSpreadDistanceByLevel(level);
	hero->SetSkillCoolDownTimeByLevelAt(Skill__PowerHit, level);

	// Heal - heal amount and heal radius and CD
	level = AllSkillLevel[LeaderLevel].Level;
	heroStats->SetHealRadiusByLevel(level);
	heroStats->SetHealAmountByLevel(level);
	hero->SetSkillCoolDownTimeByLevelAt(Skill__Heal, level);

	// faith - reduce the stamina recover punishment
	level = AllSkillLevel[FaithLevel].Level;
	heroStats->SetSpRecoverRateDuringFaith(level);

}



#pragma region  On Plus Clicked
void UInGameMenu::OnPlusBtnClicked(int _index)
{
	if (CurrentSkillPoints_int <= 0 || AllSkillLevel[_index].Level >= MaxSkillLevel)
		return;

	// update current Skill points
	CurrentSkillPoints_int--;
	CurrentSkillPoints->SetText(FText::AsNumber(CurrentSkillPoints_int));

	AllSkillLevel[_index].Level++;
	AllSkillLevel[_index].Text->SetText(FText::AsNumber(AllSkillLevel[_index].Level));
}


void UInGameMenu::OnSurviorPlusClicked()
{

	OnPlusBtnClicked(SurvivalTrainingLevel);

}

void UInGameMenu::OnStaminaPlusClicked()
{
	OnPlusBtnClicked(StaminaTraingingLevel);

}

void UInGameMenu::OnFarmerPlusClicked()
{
	OnPlusBtnClicked(FarmerLevel);

}

void UInGameMenu::OnBuilderPlusClicked()
{

	OnPlusBtnClicked(BuilderLevel);
}

void UInGameMenu::OnMinerPlusClicked()
{
	OnPlusBtnClicked(MinerLevel);
}

void UInGameMenu::OnSawyerPlusClicked()
{
	OnPlusBtnClicked(SawyerLevel);
}

void UInGameMenu::OnHitThemHardPlusClicked()
{
	OnPlusBtnClicked(HitThemHardLevel);

}

void UInGameMenu::OnMakeThemSufferPlusClicked()
{
	OnPlusBtnClicked(MakeThemSufferLevel);

}

void UInGameMenu::OnFaithPlusClicked()
{
	OnPlusBtnClicked(FaithLevel);
}

void UInGameMenu::OnLeaderPlusClicked()
{
	OnPlusBtnClicked(LeaderLevel);

}

#pragma endregion







void UInGameMenu::OnSurviorPlusHovered()
{
	OnPlusHovered(SurvivalTrainingLevel);
}

void UInGameMenu::OnStaminaPlusHovered()
{
	OnPlusHovered(StaminaTraingingLevel);
}

void UInGameMenu::OnFarmerPlusHovered()
{
	OnPlusHovered(FarmerLevel);
}

void UInGameMenu::OnBuilderPlusHovered()
{
	OnPlusHovered(BuilderLevel);
}

void UInGameMenu::OnMinerPlusHovered()
{
	OnPlusHovered(MinerLevel);
}


void UInGameMenu::OnSawyerPlusHovered()
{
	OnPlusHovered(SawyerLevel);
}

void UInGameMenu::OnHitThemHardPlusHovered()
{
	OnPlusHovered(HitThemHardLevel);
}

void UInGameMenu::OnMakeThemSufferPlusHovered()
{
	OnPlusHovered(MakeThemSufferLevel);
}

void UInGameMenu::OnFaithPlusHovered()
{
	OnPlusHovered(FaithLevel);
}

void UInGameMenu::OnLeaderPlusHovered()
{
	OnPlusHovered(LeaderLevel);
}




void UInGameMenu::OnPlusHovered(int _index)
{
	if (WBP_SkillMenuPopUp)
	{
		PopUpWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), WBP_SkillMenuPopUp);

		FVector2D WidgetLocation = AllSkillLevel[_index].Button->GetCachedGeometry().GetAbsolutePosition();
		//FVector2D MouseLocation = InMouseEvent.GetScreenSpacePosition();

		FVector2D pixelPosition, viewPortPosition;

		USlateBlueprintLibrary::AbsoluteToViewport(GetWorld(),
			WidgetLocation, pixelPosition, viewPortPosition);

		PopUpWidget->SetAlignmentInViewport(FVector2D(0.0f, 0.0f));
		PopUpWidget->SetPositionInViewport(viewPortPosition);

		USkillMenu_PopUp* PopUp = Cast<USkillMenu_PopUp>(PopUpWidget);
		if (PopUp)
		{
			PopUp->SetUpSkillPopUp(_index, this);
			PopUp->AddToViewport();
		}
		else
			UE_LOG(LogTemp, Error, TEXT("PopUp == nullptr, UInGameMenu::OnPlusHovered"));
	}
	else
		UE_LOG(LogTemp, Error, TEXT("WBP_SkillMenuPopUp == nullptr, UInGameMenu::OnPlusHovered"));

}

void UInGameMenu::OnPlusUnHovered()
{
	PopUpWidget->RemoveFromParent();
	PopUpWidget = nullptr;
}
