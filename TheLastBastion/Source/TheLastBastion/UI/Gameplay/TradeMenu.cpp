// Fill out your copyright notice in the Description page of Project Settings.

#include "TradeMenu.h"
#include "Components/ScrollBox.h"
#include "UI/Gameplay/ShopRow.h"
#include "UI/Gameplay/InventoryUI.h"
#include "UObject/ConstructorHelpers.h"


#define ShopRowSize 7

UTradeMenu::UTradeMenu(const FObjectInitializer & ObjectInit) : Super(ObjectInit)
{

	LoadWolfThumbNail();


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

	WolfRow->SetEachSlotActionImage(WolfRowImages);

	return true;
}

void UTradeMenu::OnOpenTradeMenu(UHeroStatsComponent * _heroStats)
{
	InventoryUI->OnOpenTradeMenu(_heroStats);
}

void UTradeMenu::FindImage(UTexture2D * _image, const TCHAR * _imageToFind)
{

	if (_image == nullptr)
	{
		// Set the default player avatar image
		ConstructorHelpers::FObjectFinder<UTexture2D> image(_imageToFind);
		if (image.Succeeded())
			_image = image.Object;
	}

}

void UTradeMenu::LoadWolfThumbNail()
{

	WolfRowImages.SetNum(ShopRowSize);

	ConstructorHelpers::FObjectFinder<UTexture2D> ArmorTB(TEXT("/Game/UI/ThumbNail/TN_WolfArmor"));
	if (ArmorTB.Succeeded())
		WolfRowImages[(int)EType::Armor] = ArmorTB.Object;

	ConstructorHelpers::FObjectFinder<UTexture2D> SHWeaponLongTB(TEXT("/Game/UI/ThumbNail/TN_WolfArmor"));
	if (SHWeaponLongTB.Succeeded())
		WolfRowImages[(int)EType::SHWeapon_Long] = SHWeaponLongTB.Object;

	ConstructorHelpers::FObjectFinder<UTexture2D> SHWeaponShortTB(TEXT("/Game/UI/ThumbNail/TN_WolfArmor"));
	if (SHWeaponShortTB.Succeeded())
		WolfRowImages[(int)EType::SHWeapon_Short] = SHWeaponShortTB.Object;

	ConstructorHelpers::FObjectFinder<UTexture2D> RangeWeaponTB(TEXT("/Game/UI/ThumbNail/TN_WolfArmor"));
	if (RangeWeaponTB.Succeeded())
		WolfRowImages[(int)EType::RangeWeapon] = RangeWeaponTB.Object;

	ConstructorHelpers::FObjectFinder<UTexture2D> THWeaponTB(TEXT("/Game/UI/ThumbNail/TN_WolfArmor"));
	if (THWeaponTB.Succeeded())
		WolfRowImages[(int)EType::THWeapon] = THWeaponTB.Object;

	ConstructorHelpers::FObjectFinder<UTexture2D> HeavyWeaponAxeTB(TEXT("/Game/UI/ThumbNail/TN_WolfArmor"));
	if (HeavyWeaponAxeTB.Succeeded())
		WolfRowImages[(int)EType::HeavyWeapon_Axe] = HeavyWeaponAxeTB.Object;

	ConstructorHelpers::FObjectFinder<UTexture2D> HeavyWeaponHammerTB(TEXT("/Game/UI/ThumbNail/TN_WolfArmor"));
	if (HeavyWeaponHammerTB.Succeeded())
		WolfRowImages[(int)EType::HeavyWeapon_Hammer] = HeavyWeaponHammerTB.Object;

}
