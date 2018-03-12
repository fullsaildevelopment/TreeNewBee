// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "BTTask_CombatWait.generated.h"


UCLASS()
class THELASTBASTION_API UBTTask_CombatWait : public UBTTask_Wait
{
	GENERATED_UCLASS_BODY()


	EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	FString GetStaticDescription() const override;


protected:

	void TickTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	
};
