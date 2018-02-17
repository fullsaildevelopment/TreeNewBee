// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AllyGroup.h"
#include "AllyMeleeGroup.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API AAllyMeleeGroup : public AAllyGroup
{
	GENERATED_BODY()
	
	

protected:

	UFUNCTION()
		void OnMeleeVisionOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	UFUNCTION()
		void OnMeleeVisionOverrlapEnd(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	
};
