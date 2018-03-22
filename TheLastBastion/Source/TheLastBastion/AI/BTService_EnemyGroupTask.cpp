// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_EnemyGroupTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"

#include "AI/TheLastBastionGroupAIController.h"
#include "AI/EnemyGroup.h"



void UBTService_EnemyGroupTask::TickNode(UBehaviorTreeComponent & _ownerComp, uint8* _nodeMemory, float _deltaSecond)
{

	Super::TickNode(_ownerComp, _nodeMemory, _deltaSecond);

	ATheLastBastionGroupAIController* aiCtrl = Cast<ATheLastBastionGroupAIController>(_ownerComp.GetAIOwner());
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

	
	int injuries = 0;

	int groupMaxNumber = aiGroup->GetGroupSizeMax();
	int currentGroupSize = aiGroup->GetGroupSize();
	int deadNumber = groupMaxNumber - currentGroupSize;

	injuries += deadNumber;
	for (int iChar = 0; iChar < currentGroupSize; iChar++)
	{
		if (aiGroup->GetGroupMemberAt(iChar)->HasFullHealth() == false)
			injuries++;
	}


	//UE_LOG(LogTemp, Log, TEXT(" %s, Injuries : %d:  - UBTService_EnemyGroupTask::TickNode"), *aiGroup->GetName(), injuries);

	if (injuries >= 0.5f * groupMaxNumber)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Rage !!!!!!!!!!!!!!!!! - UBTService_EnemyGroupTask::TickNode"), *aiGroup->GetName());
		aiCtrl->SetIsCharging_BBC(true);
	}

}

void UBTService_EnemyGroupTask::MeleeAgainstPlayer(AEnemyGroup* const _aiGroup)
{
	_aiGroup->MeleeAgainstPlayer_OnEnemyGroupMission();
}
