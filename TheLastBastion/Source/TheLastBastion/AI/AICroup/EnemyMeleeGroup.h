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
	
	
protected:

	UFUNCTION()
		void OnGroupVolumnOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	UFUNCTION()
		void OnGroupVolumnOverrlapEnd(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	
};
