// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HitReaction.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UBTTask_HitReaction : public UBTTaskNode
{
	GENERATED_UCLASS_BODY()

		/** starts this task, should return Succeeded, Failed or InProgress
		*  (use FinishLatentTask() when returning InProgress)
		* this function should be considered as const (don't modify state of object) if node is not instanced! */
		EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,
			uint8* NodeMemory) override;

protected:

	UPROPERTY(EditInstanceOnly, Category = CombatStrategy)
		/** sqr distance to start a melee combo*/
		float meleeComboAttackDistanceSqr;

protected:

	/** aborts this task, should return Aborted or InProgress
	*  (use FinishLatentAbort() when returning InProgress)
	* this function should be considered as const (don't modify state of object) if node is not instanced! */
	EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) override;

	/** ticks this task
	* this function should be considered as const (don't modify state of object) if node is not instanced! */
	void TickTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory, float DeltaSeconds) override;

	//** message handler, default implementation will finish latent execution/abortion
	/* this function should be considered as const (don't modify state of object) if node is not instanced! */
	//void OnMessage(UBehaviorTreeComponent& OwnerComp, 
	//	uint8* NodeMemory, FName Message, int32 RequestID, bool bSuccess) override;

	/** called when task execution is finished
	* this function should be considered as const (don't modify state of object) if node is not instanced! */
	void OnTaskFinished(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;


private:

	void OnRecoverFromHitHandle(UBehaviorTreeComponent* OwnerComp);

	
	





	
};
