// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "BTTask_GroupWayPointWait.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UBTTask_GroupWayPointWait : public UBTTask_Wait
{
	GENERATED_UCLASS_BODY()

protected:

	void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	
	
	
};
