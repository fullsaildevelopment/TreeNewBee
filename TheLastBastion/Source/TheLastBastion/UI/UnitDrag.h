// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "UI/Gameplay/CrewSlotUI.h"
#include "UnitDrag.generated.h"

/**
 * 
 */

UCLASS()
class THELASTBASTION_API UUnitDrag : public UDragDropOperation
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		struct FUnitData unitData;
	
};
