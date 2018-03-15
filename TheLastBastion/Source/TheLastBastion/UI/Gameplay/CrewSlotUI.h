// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ActionSlot.h"
#include "CrewSlotUI.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FUnitData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<class ATheLastBastionAIBase> Unit_Bp;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		UTexture2D* Unit_Image = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		int Price = 0;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		bool IsMelee;

};


UCLASS()
class THELASTBASTION_API UCrewSlotUI : public UActionSlot
{
	GENERATED_BODY()

protected:

	bool Initialize() override;

	void NativeOnDragDetected(const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent,
		UDragDropOperation*& OutOperation) override;

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FUnitData Unit_Data;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* NumericValue;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		int AllyIndex = -1;

	UPROPERTY(BlueprintReadOnly)
		bool bFromSelection;

public:

	FORCEINLINE TSubclassOf<class ATheLastBastionAIBase> GetUnitClass() const { return Unit_Data.Unit_Bp; }
	FORCEINLINE bool IsMeleeGroup() const { return Unit_Data.IsMelee; }
	FORCEINLINE void SetUnitClass(TSubclassOf<class ATheLastBastionAIBase> _class) { Unit_Data.Unit_Bp = _class; }
	/** Set thumbNail Image*/
	FORCEINLINE void SetImage(UTexture2D* _image) override { Unit_Data.Unit_Image = _image; }
	FORCEINLINE void SetAllyIndex(float _index) { AllyIndex = _index; }

	void ClearNumericValue();
	/** set the player cost by this unit*/
	void SetPrice(int _price);

	void SetUnitData(const FUnitData& _data, bool _bShowPrice = false);

	/** set the amount of this unit player currently have*/
	void SetUnitNumber(int _number);

	void OnCrewMemberDead();

	void OnCrewSelected();

	bool IsCrewSlotEmpty() const;

};
