// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ActionSlot.h"
#include "TradeMenuSlot.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FGearUI
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<class AGear> Gear_Bp;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		UTexture2D* Gear_Image;
};

UENUM(BlueprintType)
enum class EInventoryGearType : uint8
{
	SHWeapon = 0,
	THWeapon,
	RangeWeapon,
	HeavyWeapon,
	Armor
};

UENUM(BlueprintType)
enum class EUpgradeGearType : uint8
{
	LongSword = 0,
	WarAxe,
	Mace,
	CrossBow,
	Armor ,
	Katana,
	BattleAxe,
	Hammer,
	GreatSword
};



UCLASS()
class THELASTBASTION_API UTradeMenuSlot : public UActionSlot
{
	GENERATED_BODY()
	
protected:

	bool Initialize() override;

	void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (BlueprintProtected))
		// Image to tell what this action does
		FGearUI GearUI;

	UPROPERTY(BlueprintReadOnly)
		// Image to tell what this action does
		EInventoryGearType InventoryGearType;

	UPROPERTY(BlueprintReadOnly)
		// Image to tell what this action does
		EUpgradeGearType UpgradeGearType;

public:


	void SetImage(class UTexture2D* _image) override;

	FORCEINLINE void SetGearBp(TSubclassOf<class AGear> _val) { GearUI.Gear_Bp = _val; }

	/** Called on Inventory UI init, index the action slot for later condition check */
	void OnInventoryInit(int _index);

	/** Called on ShopRow init, index the action slot for later condition check */
	void OnShopRowInit(int _index);

	FORCEINLINE void SetAction(const FGearUI& _val) { GearUI = _val; }

	FORCEINLINE TSubclassOf<class AGear> GetGearClass() const { return GearUI.Gear_Bp; }

};
