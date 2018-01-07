// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SharedService.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EAISharedServiceType : uint8
{
	None = 0                                     UMETA(DisplayName = "None"),
	UpdateSqrDistanceToTarget = 1                UMETA(DisplayName = "Update Squared Distance To Target Actor"),
	SyncMeleeAttackRotationRateAndVelocity = 2   UMETA(DisplayName = "Sync Melee Attack Motion")
};



UCLASS()
class THELASTBASTION_API UBTService_SharedService : public UBTService
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, category = SharedService)
		EAISharedServiceType service;

protected:

	void TickNode(UBehaviorTreeComponent& _ownerComp, uint8* _nodeMemory, float _deltaSecond) override;

private:

	void UpdateSqrDistanceToTarget(UBehaviorTreeComponent & _ownerComp);

	void SyncMeleeAttackRotationRateAndVelocity(UBehaviorTreeComponent & _ownerComp);


};
