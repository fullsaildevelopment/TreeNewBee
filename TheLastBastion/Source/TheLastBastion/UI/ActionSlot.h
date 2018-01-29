// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionSlot.generated.h"

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
	SHWeapon_Long = 0,
	SHWeapon_Short,
	RangeWeapon,
	Armor,
	THWeapon,
	HeavyWeapon_Axe,
	HeavyWeapon_Hammer
};



UCLASS()
class THELASTBASTION_API UActionSlot : public UUserWidget
{
	GENERATED_BODY()
	
private:

	static TSubclassOf<class UUserWidget> WBP_DraggedItem;


public:

	UActionSlot(const FObjectInitializer& _objInit);

protected:

	bool Initialize() override;

	void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	FReply NativeOnMouseButtonDown
	(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;


	//void NativeOnDragEnter(const FGeometry& InGeometry, 
	//	const FDragDropEvent& InDragDropEvent, 
	//	UDragDropOperation* InOperation) override;

	//void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, 
	//	UDragDropOperation* InOperation) override;


protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (BlueprintProtected))
		// Image to tell what this action does
		FGearUI GearUI;

	UPROPERTY(meta = (BindWidget))
		class USizeBox* SlotSize;

	UPROPERTY(meta = (BindWidget))
		class UButton* ActionButton;

	UPROPERTY(BlueprintReadOnly)
		// Image to tell what this action does
		EInventoryGearType InventoryGearType;
	
	UPROPERTY(BlueprintReadOnly)
		// Image to tell what this action does
		EUpgradeGearType UpgradeGearType;

	UPROPERTY(BlueprintReadOnly)
		bool bFromUpgrade;


public:

	void SetSize(float _width, float _height);

	FORCEINLINE void SetGearBp(TSubclassOf<class AGear> _val) { GearUI.Gear_Bp = _val; }

	/** Called on Inventory UI init, index the action slot for later condition check */
	void OnInventoryInit(int _index);

	/** Called on ShopRow init, index the action slot for later condition check */
	void OnShopRowInit(int _index);

	FORCEINLINE void SetAction(const FGearUI& _val) { GearUI = _val; }

	FORCEINLINE void SetActionImage(class UTexture2D* _image) { GearUI.Gear_Image = _image; }
		
	FORCEINLINE TSubclassOf<class AGear> GetGearClass() const { return GearUI.Gear_Bp; }

};
