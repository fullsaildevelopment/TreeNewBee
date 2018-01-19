// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_RangeAttack.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Animation/AIRange_AnimInstance.h"


EBTNodeResult::Type UBTTask_RangeAttack::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	///*** empty in base class

	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;

	UBlackboardComponent* const bbc = OwnerComp.GetBlackboardComponent();
	ATheLastBastionBaseAIController* const enemyC = Cast<ATheLastBastionBaseAIController>(OwnerComp.GetAIOwner());
	if (enemyC == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AIOwner is not an enemyC - UBTTask_RangeAttack::ExecuteTask"));
		return NodeResult;
	}

	UAIBase_AnimInstance* animRef = enemyC->GetAnimInstance();
	if (animRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("animRef is NULL - UBTTask_RangeAttack::ExecuteTask"));
		return NodeResult;
	}


	const AActor* const targetActor = Cast<AActor>(bbc->GetValue<UBlackboardKeyType_Object>(enemyC->GetKeyID_TargetActor()));
	if (targetActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateSqrDistanceToTarget get target actor failed"));
		return NodeResult;
	}

	const APawn* const me = enemyC->GetPawn();
	if (me == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("enemyC->GetPawn() is NULL - UBTTask_RangeAttack::ExecuteTask"));
		return NodeResult;
	}


	// check attack distance
	float distanceSqr = (targetActor->GetActorLocation() - me->GetActorLocation()).SizeSquared();
	bbc->SetValue<UBlackboardKeyType_Float>(enemyC->GetKeyID_ToTargetActorDistanceSqr(), distanceSqr);

	const AActor* target = Cast<AActor>(bbc->GetValue<UBlackboardKeyType_Object>(enemyC->GetKeyID_TargetActor()));

	if (distanceSqr > RangeAttackDistanceSqr)
	{
		UE_LOG(LogTemp, Log, TEXT("Is too far too attack, failed this task, and move to next task"));
		return NodeResult;
	}
	else
	{
		animRef->Fire(target);
		NodeResult = EBTNodeResult::Succeeded;

		return NodeResult;
	}




	//if (animRef->GetCurrentActionState() != EAIActionState::None)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("Is Attacking, failed this task, and move to next task"));
	//	return NodeResult;
	//}
	//else
	//{
	//}
}




