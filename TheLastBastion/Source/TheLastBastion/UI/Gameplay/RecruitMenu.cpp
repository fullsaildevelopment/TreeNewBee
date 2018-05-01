// Fill out your copyright notice in the Description page of Project Settings.

#include "RecruitMenu.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "UI/Gameplay/UnitRow.h"
#include "UI/Gameplay/CrewBar_RecruitMenu.h"
#include "TheLastBastionHeroCharacter.h"
#include "Combat/HeroStatsComponent.h"
#include "PCs/SinglePlayerPC.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/SinglePlayerGM.h"
#include "Environment/EnemyGroupSpawner.h"


#define Wolf_Unlock 2
#define Nord_Unlock 4
#define Dwarven_Unlock 7
#define Ebony_Unlock 9

bool URecruitMenu::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood = UnitList && AllMyCrew &&
		Cancel && Accept;

	if (bAllWidgetAreGood)
	{
		Accept->OnClicked.AddDynamic(this, &URecruitMenu::OnAcceptClicked);
		Cancel->OnClicked.AddDynamic(this, &URecruitMenu::OnCancelClicked);
	}
	else
		return false;


	UUnitRow* unitRow = nullptr;
	for (size_t i = 0; i < UnitList->GetChildrenCount(); i++)
	{
		unitRow = Cast<UUnitRow>(UnitList->GetChildAt(i));
		if (unitRow)
		{
			unitRow->SetEachSlotSize(164, 164);
		}
	}

	HillTribe->SetHeadingText(FText::FromString(TEXT("Hill Tribe: ")));
	HillTribe->SetEachSlotAction(HillTribe_Data);

	Wolf->SetHeadingText(FText::FromString(TEXT("Wolf: ")));
	Wolf->SetEachSlotAction(Wolf_Data);

	Nord->SetHeadingText(FText::FromString(TEXT("North: ")));
	Nord->SetEachSlotAction(Nord_Data);

	Dwarven->SetHeadingText(FText::FromString(TEXT("Dwarven: ")));
	Dwarven->SetEachSlotAction(Dwarven_Data);

	Ebony->SetHeadingText(FText::FromString(TEXT("Ebony: ")));
	Ebony->SetEachSlotAction(Ebony_Data);


	bIsOpened = false;
	return true;

}

void URecruitMenu::OnOpenRecruitMenu()
{   
	//// Display available ally types to player based on Player's level
	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("gm == nullptr, URecruitMenu::OnOpenRecruitMenu"));
		return;
	}

	AEnemyGroupSpawner* enemySpawner = gm->GetEnemyGroupSpawner();
	if (enemySpawner == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("enemySpawner == nullptr, URecruitMenu::OnOpenRecruitMenu"));
		return;
	}

	int currentWave = enemySpawner->GetCurrentWaveIndex();

	Wolf->SetIsEnabled(false);
	Nord->SetIsEnabled(false);
	Dwarven->SetIsEnabled(false);
	Ebony->SetIsEnabled(false);

	if (currentWave >= Wolf_Unlock)
	{
		Wolf->SetIsEnabled(true);
		if (currentWave >= Nord_Unlock)
		{
			Nord->SetIsEnabled(true);
			if (currentWave >= Dwarven_Unlock)
			{
				Dwarven->SetIsEnabled(true);
				if (currentWave >= Ebony_Unlock)
					Ebony->SetIsEnabled(true);
			}
		}
	}

	AllMyCrew->OnOpenRecruitMenu();
	bIsOpened = true;	
}

void URecruitMenu::OnAcceptClicked()
{
	AllMyCrew->OnAccept();


	OnCancelClicked();
}

void URecruitMenu::OnCancelClicked()
{   
	this->RemoveFromParent();
	//APlayerController* pc = GetOwningPlayer();
	ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(GetOwningPlayer());
	if (pc)
	{
		pc->GetInGameHUD()->SetPopUpNotificationVisibility(true);
		pc->bShowMouseCursor = false;
		FInputModeGameOnly InputMode;
		pc->SetInputMode(InputMode);
	}
	bIsOpened = false;
}

void URecruitMenu::AddWood(int _val)
{
	AllMyCrew->AddWoodValue(_val);
	CheckAcceptButtonShouldEnabled();
}

void URecruitMenu::AddFood(int _val)
{
	AllMyCrew->AddFoodValue(_val);
	CheckAcceptButtonShouldEnabled();
}

void URecruitMenu::AddMetal(int _val)
{
	AllMyCrew->AddMetalValue(_val);
	CheckAcceptButtonShouldEnabled();
}

void URecruitMenu::AddStone(int _val)
{
	AllMyCrew->AddStoneValue(_val);
	CheckAcceptButtonShouldEnabled();
}

void URecruitMenu::CheckAcceptButtonShouldEnabled()
{
	bool bIsEnabled = true;

	bIsEnabled = AllMyCrew->GetMetal_int() >= 0 && AllMyCrew->GetFood_int() >= 0 && AllMyCrew->GetWood_int() >= 0;
	Accept->SetIsEnabled(bIsEnabled);
}
