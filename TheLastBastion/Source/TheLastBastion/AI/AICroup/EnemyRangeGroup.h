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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TargetSelection)
		TArray<FRangeGroupPrimaryTarget> PrimaryTargets;

protected:

	UFUNCTION()
		void OnRangeVisionOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	UFUNCTION()
		void OnRangeVisionOverrlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;


	void SetGroupVisionVolumn(float _maxGroupWidth, float _maxGroupLength) override;

	//AActor* OnTargetRequest(const ATheLastBastionCharacter* _requestSender) override;

	/** Put the overlap target in to primary target list, if there is a room*/
	void SetRangeGroupTarget_OnOverLap(class ATheLastBastionCharacter* _target);

	/** Clear the target from the primary target list on overlap end*/
	void SetRangeGroupTarget_OnOverLapEnd(class ATheLastBastionCharacter* _target);

	/** Evaluate the threat map, update the primary target list
	and assign target to group member*/
	void SetRangeGroupTarget_OnRequest();

};
