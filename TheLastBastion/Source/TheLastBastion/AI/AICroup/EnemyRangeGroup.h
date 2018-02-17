// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/EnemyGroup.h"
#include "EnemyRangeGroup.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API AEnemyRangeGroup : public AEnemyGroup
{
	GENERATED_BODY()
	
	
public:
	AEnemyRangeGroup();

protected:

	UFUNCTION()
		void OnRangeVisionOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	UFUNCTION()
		void OnRangeVisionOverrlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;


	void SetGroupVisionVolumn(float _maxGroupWidth, float _maxGroupLength) override;

	AActor* OnTargetRequest(const ATheLastBastionCharacter* _requestSender) override;


};
