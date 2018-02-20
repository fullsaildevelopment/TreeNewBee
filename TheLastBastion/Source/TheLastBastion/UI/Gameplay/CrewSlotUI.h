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
		UTexture2D* Unit_Image;

};


UCLASS()
class THELASTBASTION_API UCrewSlotUI : public UActionSlot
{
	GENERATED_BODY()
	

protected:

	bool Initialize() override;

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FUnitData Unit_Data;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* NumericValue;

public:

	FORCEINLINE void SetUnitData(const FUnitData& _data) {Unit_Data = _data;}

	void OnCrewMemberDead();

	void OnCrewSelected();


};
