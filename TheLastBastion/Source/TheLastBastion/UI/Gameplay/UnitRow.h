// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitRow.generated.h"

/**
 * 
 */
#define UnitRowSize 4

UCLASS()
class THELASTBASTION_API UUnitRow : public UUserWidget
{
	GENERATED_BODY()
	
	
protected:

	bool Initialize() override;

protected:

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* UnitsClassRow;

	UPROPERTY(meta = (BineWidget))
		class UTextBlock* UnitClassName;

	UPROPERTY(meta = (BindWidget))
		class UCrewSlotUI* Slot_0;

	UPROPERTY(meta = (BindWidget))
		class UCrewSlotUI* Slot_1;

	UPROPERTY(meta = (BindWidget))
		class UCrewSlotUI* Slot_2;

	UPROPERTY(meta = (BindWidget))
		class UCrewSlotUI* Slot_3;


public:

	void SetHeadingText(FText _headingText);

	void SetEachSlotSize(float _width, float _height);

	void SetEachSlotAction(const TArray<struct FUnitData>& _unitData);

	//void SetEachSlotImage(const TArray<UTexture2D*>& _actionImages);

};
