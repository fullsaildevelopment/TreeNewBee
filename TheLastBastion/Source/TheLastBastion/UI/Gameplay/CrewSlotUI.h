// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ActionSlot.h"
#include "CrewSlotUI.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UCrewSlotUI : public UActionSlot
{
	GENERATED_BODY()
	

protected:

	bool Initialize() override;

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		UTexture2D* Unit_Image;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* NumericValue;

public:

	void OnCrewMemberDead();

	void OnCrewSelected();


};
