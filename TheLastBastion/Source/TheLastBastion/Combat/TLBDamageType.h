// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "TLBDamageType.generated.h"

/**
 * 
 */


UENUM(BlueprintType)
enum class ElementalType : uint8
{
	None = 0    UMETA(DisplayName = "None"),
	Physics = 1 UMETA(DisplayName = "Physics"),
	Posion = 2  UMETA(DisplayName = "Posion"),
	Fire = 3    UMETA(DisplayName = "Fire"),
	Ice = 4     UMETA(DisplayName = "Ice")
};


UCLASS()
class THELASTBASTION_API UTLBDamageType : public UDamageType
{
	GENERATED_BODY()


public:

	UTLBDamageType(const FObjectInitializer& _objInit);

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = DamageRender)
		ElementalType ElementalType;

	



	
};
