// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_EnemyGroupTask.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UBTService_EnemyGroupTask : public UBTService
{
	GENERATED_BODY()
	
protected:

	void TickNode(UBehaviorTreeComponent& _ownerComp, uint8* _nodeMemory, float _deltaSecond) override;

	

private:

	void MeleeAgainstPlayer(class AEnemyGroup* const _aiGroup);
	
};
