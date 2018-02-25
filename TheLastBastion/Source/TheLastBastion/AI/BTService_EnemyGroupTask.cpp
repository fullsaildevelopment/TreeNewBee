// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_EnemyGroupTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "TheLastBastionBaseAIController.h"
#include "AI/EnemyGroup.h"



void UBTService_EnemyGroupTask::TickNode(UBehaviorTreeComponent & _ownerComp, uint8* _nodeMemory, float _deltaSecond)
{

	Super::TickNode(_ownerComp, _nodeMemory, _deltaSecond);

	AAIController* aiCtrl = _ownerComp.GetAIOwner();
	if (aiCtrl == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("aiCtrl == nullptr - UBTService_EnemyGroupTask::TickNode"));
		return;
	}
	AEnemyGroup* aiGroup = Cast<AEnemyGroup>(aiCtrl->GetPawn());
	if (aiGroup == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("aiGroup == nullptr - UBTService_EnemyGroupTask::TickNode"));
		return;
	}

	if (aiGroup->IsInBattle())
	{
		return;
	}

	int currentTask = aiGroup->GetMainTask();

	switch (currentTask)
	{
	case EM_MeleeAgainstPlayer:
		MeleeAgainstPlayer(aiGroup);
		break;
	default:
		break;
	}


	UE_LOG(LogTemp, Log, TEXT("dt: %f"), _deltaSecond);
}

void UBTService_EnemyGroupTask::MeleeAgainstPlayer(AEnemyGroup* const _aiGroup)
{
	_aiGroup->MeleeAgainstPlayer_OnEnemyGroupMission();
}
