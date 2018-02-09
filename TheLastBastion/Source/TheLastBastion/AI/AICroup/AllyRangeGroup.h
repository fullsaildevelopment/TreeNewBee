// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AllyGroup.h"
#include "AllyRangeGroup.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API AAllyRangeGroup : public AAllyGroup
{
	GENERATED_BODY()
	
public:
	AAllyRangeGroup();

protected:
	void BeginPlay() override;

	UFUNCTION()
	void OnGroupVolumnOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	UFUNCTION()
	void OnGroupVolumnOverrlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void IntializeTargetsForGroupMembers();
	
	void ClearTargetsForGroupMembers();
	
};
