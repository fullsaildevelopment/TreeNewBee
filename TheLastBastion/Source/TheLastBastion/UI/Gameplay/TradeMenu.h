// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Gameplay/TradeMenuSlot.h"
#include "TradeMenu.generated.h"



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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrades")
		TArray<struct FGearUI> NordicRow_GearUI;

	UPROPERTY(meta = (BindWidget))
		class UShopRow* DwarvenRow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrades")
		TArray<struct FGearUI> DwarvenRow_GearUI;

	UPROPERTY(meta = (BindWidget))
		class UShopRow* EbonyRow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrades")
		TArray<struct FGearUI> EbonyRow_GearUI;

	UPROPERTY(meta = (BindWidget))
		class UShopRow* DaedricRow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrades")
		TArray<struct FGearUI> DaedricRow_GearUI;

	UPROPERTY(meta = (BindWidget))
		class UInventoryUI* InventoryUI;

	UPROPERTY(meta = (BindWidget))
		class UButton* Cancel;

	UPROPERTY(meta = (BindWidget))
		class UButton* Accept;

	bool bIsOpened;


public:

	void OnOpenTradeMenu(class UHeroStatsComponent* _heroStats);


	/** Update the resource data in inventory menu while trade menu is opened*/
	void AddWood(int _val);

	/** Update the resource data in inventory menu while trade menu is opened*/
	void AddFood(int _val);

	/** Update the resource data in inventory menu while trade menu is opened*/
	void AddMetal(int _val);

	/** Update the resource data in inventory menu while trade menu is opened*/
	void AddStone(int _val);


	FORCEINLINE bool IsOpened() const { return bIsOpened; }
	FORCEINLINE class UInventoryUI* GetInventoryUI() const { return InventoryUI; }

private:

	UFUNCTION()
		void OnAcceptClicked();

	UFUNCTION()
		void OnCancelClicked();

};
