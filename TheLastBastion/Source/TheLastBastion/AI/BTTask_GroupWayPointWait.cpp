// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_GroupWayPointWait.h"
#include "AI/TheLastBastionGroupAIController.h"



UBTTask_GroupWayPointWait::UBTTask_GroupWayPointWait(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTaskFinished = true;
}


void UBTTask_GroupWayPointWait::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	ATheLastBastionGroupAIController* aiCtrl =Cast<ATheLastBastionGroupAIController> (OwnerComp.GetAIOwner());
	if (aiCtrl == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("aiCtrl == nullptr - UBTService_EnemyGroupTask::TickNode"));
		return;
	}

	aiCtrl->SetNewWayPoint_BBC(false);

}