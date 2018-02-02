// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "UI/ActionSlot.h"
#include "ItemDrag.generated.h"

/**
 * 
 */
enum class EUpgradeGearType : uint8;

UCLASS()
class THELASTBASTION_API UItemDrag : public UDragDropOperation
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(BlueprintReadOnly)		
	struct FGearUI GearUI;

	EUpgradeGearType UpgradeGearType;

};
