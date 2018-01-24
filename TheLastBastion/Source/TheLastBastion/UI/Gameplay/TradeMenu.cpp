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


#define ShopRowSize 7

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
		WolfRow && DwarvenRow && NordicRow && GlassRow && EbonyRow && DragonRow && DaedricRow
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
	{
		WolfRow->SetEachSlotAction(WolfRow_GearUI);
	}

	return true;
}

void UTradeMenu::OnOpenTradeMenu(UHeroStatsComponent * _heroStats)
{
	InventoryUI->OnOpenTradeMenu(_heroStats);
}

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
			}
		}
	}

	OnCancelClicked();
}

void UTradeMenu::OnCancelClicked()
{
	this->RemoveFromParent();
	APlayerController* pc = GetOwningPlayer();
	pc->bShowMouseCursor = false;
	FInputModeGameOnly InputMode;
	pc -> SetInputMode(InputMode);
}


