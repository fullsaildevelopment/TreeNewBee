// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "TLBDamageType.generated.h"

/**
 * 
 */



UCLASS()
class THELASTBASTION_API UTLBDamageType : public UDamageType
{
	GENERATED_BODY()


public:

	UTLBDamageType(const FObjectInitializer& _objInit);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsComboShot;

public:

	FORCEINLINE void SetIsComboShot(bool _val) { bIsComboShot = _val; }
	FORCEINLINE bool GetIsComboShot() const { return bIsComboShot; }
};
