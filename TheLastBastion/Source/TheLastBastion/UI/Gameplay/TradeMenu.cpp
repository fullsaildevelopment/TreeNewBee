// Fill out your copyright notice in the Description page of Project Settings.

#include "TradeMenu.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "UI/Gameplay/ShopRow.h"
#include "UI/Gameplay/InventoryUI.h"
#include "CustomType.h"
#include "UI/ActionSlot.h"
#include "TheLastBastionHeroCharacter.h"
#include "Combat/HeroStatsComponent.h"
#include "PCs/SinglePlayerPC.h"


#define ShopRowSize 9

#define NordSet_Level 20
#define DwarvenSet_Level 35
#define EbonySet_Level 50
#define DaeSet_Level 65



UTradeMenu::UTradeMenu(const FObjectInitializer & ObjectInit) : Super(ObjectInit)
{
	//Wolf_Bp

}

bool UTradeMenu::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood = UpgradeList && InventoryUI &&
		WolfRow && DwarvenRow && NordicRow && EbonyRow && DaedricRow
		&& Cancel && Accept;

	if (bAllWidgetAreGood)
	{
		Accept->OnClicked.AddDynamic(this, &UTradeMenu::OnAcceptClicked);
		Cancel->OnClicked.AddDynamic(this, &UTradeMenu::OnCancelClicked);
	}
	else
		return false;


	UShopRow* shopRow = nullptr;
	for (size_t i = 0; i < UpgradeList->GetChildrenCount(); i++)
	{
		shopRow = Cast<UShopRow>(UpgradeList->GetChildAt(i));
		if (shopRow)
		{
			shopRow->SetEachSlotSize(120, 120);
		}
	}


	if (WolfRow_GearUI.Num() == ShopRowSize)
		WolfRow->SetEachSlotAction(WolfRow_GearUI);

	if (NordicRow_GearUI.Num() == ShopRowSize)
		NordicRow->SetEachSlotAction(NordicRow_GearUI);

	if (DwarvenRow_GearUI.Num() == ShopRowSize)
		DwarvenRow->SetEachSlotAction(DwarvenRow_GearUI);

	if (EbonyRow_GearUI.Num() == ShopRowSize)
		EbonyRow->SetEachSlotAction(EbonyRow_GearUI);

	if (DaedricRow_GearUI.Num() == ShopRowSize)
		DaedricRow->SetEachSlotAction(DaedricRow_GearUI);


	bCanEverTick = false;
	bIsOpened = false;
	return true;
}

void UTradeMenu::OnOpenTradeMenu(UHeroStatsComponent * _heroStats)
{   
	// Disable advanced gear rows at first
	NordicRow->SetIsEnabled(false);
	DwarvenRow->SetIsEnabled(false);
	EbonyRow->SetIsEnabled(false);
	DaedricRow->SetIsEnabled(false);

	// Display available gears to player based on Player's level
	ATheLastBastionHeroCharacter* Player = Cast<ATheLastBastionHeroCharacter>(GetOwningPlayerPawn());
	if (Player)
	{
		UHeroStatsComponent* PlayerStats = Player->GetHeroStatsComp();
		if (PlayerStats)
		{
			int CurrentPlayerLevel = PlayerStats->GetCharacterLevel();
			if (CurrentPlayerLevel >= NordSet_Level)
			{
				NordicRow->SetIsEnabled(true);
				if (CurrentPlayerLevel >= DwarvenSet_Level)
				{
					DwarvenRow->SetIsEnabled(true);
					if (CurrentPlayerLevel >= EbonySet_Level)
					{
						EbonyRow->SetIsEnabled(true);
						if (CurrentPlayerLevel >= DaeSet_Level)
							DaedricRow->SetIsEnabled(true);
					}
				}
			}
		}
	}
	InventoryUI->OnOpenTradeMenu(_heroStats);
	bIsOpened = true;
}

void UTradeMenu::AddWood(int _val)
{
	InventoryUI->AddWoodValue(_val);
}

void UTradeMenu::AddFood(int _val)
{
	InventoryUI->AddFoodValue(_val);
}

void UTradeMenu::AddMetal(int _val)
{
	InventoryUI->AddMetalValue(_val);
}

void UTradeMenu::AddStone(int _val)
{
	InventoryUI->AddStoneValue(_val);

}

//void UTradeMenu::LoadResourceFromGM()
//{
//	InventoryUI->LoadResource();
//}

void UTradeMenu::OnAcceptClicked()
{
	ASinglePlayerPC* pc =Cast<ASinglePlayerPC>(GetOwningPlayer());
	if (pc)
	{   
		ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(pc->GetCharacter());
		if (hero)
		{
			UHeroStatsComponent* heroStats = hero->GetHeroStatsComp();
			if (heroStats)
			{
				heroStats->OnTradeMenuAccept(InventoryUI);
				// pc update HUD
				pc->OnTradeMenuAccept(heroStats);
				// Take resource from player after accept
				InventoryUI->OnTradeMenuAccept();
			}
		}
	}

	OnCancelClicked();
}

void UTradeMenu::OnCancelClicked()
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


