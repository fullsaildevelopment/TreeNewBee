// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Gameplay/TradeMenuSlot.h"
#include "TradeMenu.generated.h"


/**
 * 
 */



UENUM()
enum class EType : uint8
{
	SHWeapon_Long = 0,
	SHWeapon_Short,
	RangeWeapon,
	Armor ,
	THWeapon,
	HeavyWeapon_Axe,
	HeavyWeapon_Hammer
};



UCLASS()
class THELASTBASTION_API UTradeMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UTradeMenu(const FObjectInitializer & ObjectInit);

protected:

	bool Initialize() override;

protected:

	UPROPERTY(meta = (BindWidget))
		/** the gear list that allow player to upgrade */
		class UScrollBox* UpgradeList;

	UPROPERTY(meta = (BindWidget))
		class UShopRow* WolfRow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrades")
	TArray<struct FGearUI> WolfRow_GearUI;

	UPROPERTY(meta = (BindWidget))
		class UShopRow* NordicRow;

	UPROPERTY(meta = (BindWidget))
		class UShopRow* DwarvenRow;

	UPROPERTY(meta = (BindWidget))
		class UShopRow* GlassRow;

	UPROPERTY(meta = (BindWidget))
		class UShopRow* EbonyRow;

	UPROPERTY(meta = (BindWidget))
		class UShopRow* DragonRow;

	UPROPERTY(meta = (BindWidget))
		class UShopRow* DaedricRow;

	UPROPERTY(meta = (BindWidget))
		class UInventoryUI* InventoryUI;

	UPROPERTY(meta = (BindWidget))
		class UButton* Cancel;

	UPROPERTY(meta = (BindWidget))
		class UButton* Accept;


public:

	void OnOpenTradeMenu(class UHeroStatsComponent* _heroStats);

private:

	UFUNCTION()
		void OnAcceptClicked();

	UFUNCTION()
		void OnCancelClicked();



};
