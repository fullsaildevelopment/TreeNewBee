// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/EnemyGroup.h"
#include "EnemyMeleeGroup.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API AEnemyMeleeGroup : public AEnemyGroup
{
	GENERATED_BODY()
	
public:
	AEnemyMeleeGroup();
	
protected:

	UFUNCTION()
		void OnMeleeVisionOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	UFUNCTION()
		void OnMeleeVisionOverrlapEnd(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

public:

	/** If this group arrive at its destination, and its children get hit*/
	void OnChildTakingDamage() override;
	
};
