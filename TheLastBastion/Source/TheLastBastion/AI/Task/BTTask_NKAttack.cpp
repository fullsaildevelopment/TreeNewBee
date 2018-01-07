// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_NKAttack.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "Animation/AIBase_AnimInstance.h"

UBTTask_NKAttack::UBTTask_NKAttack(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = false;
	bNotifyTaskFinished = true;
	bIgnoreRestartSelf = false;
}

EBTNodeResult::Type UBTTask_NKAttack::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	///*** empty in base class

	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;


	UBlackboardComponent* const bbc = OwnerComp.GetBlackboardComponent();
	ATheLastBastionBaseAIController* const enemyC = Cast<ATheLastBastionBaseAIController>(OwnerComp.GetAIOwner());
	if (enemyC == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AIOwner is not an enemyC - UBTTask_NKAttack::ExecuteTask"));
		return NodeResult;
	}
	
	UAIBase_AnimInstance* animRef = enemyC->GetAnimInstance();
	if (animRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("animRef is NULL - UBTTask_NKAttack::ExecuteTask"));
		return NodeResult;
	}


	const AActor* const targetActor = Cast<AActor>(bbc->GetValue<UBlackboardKeyType_Object>(enemyC->GetKeyID_TargetActor()));
	if (targetActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateSqrDistanceToTarget get target actor failed"));
		return NodeResult;
	}

	const APawn* const me = enemyC->GetPawn();



	// check attack distance
	float distanceSqr = (me->GetActorLocation() - targetActor->GetActorLocation()).SizeSquared();
	bbc->SetValue<UBlackboardKeyType_Float>(enemyC->GetKeyID_ToTargetActorDistanceSqr(), distanceSqr);

	if (distanceSqr > meleeComboAttackDistanceSqr)
	{
		UE_LOG(LogTemp, Log, TEXT("Is too far too attack, failed this task, and move to next task"));
		return NodeResult;
	}
	else if (animRef->GetCurrentActionState() == EAIActionState::MeleeAttack)
	{
		UE_LOG(LogTemp, Log, TEXT("Is Attacking, failed this task, and move to next task"));
		return NodeResult;
	}
	else
	{
		NodeResult = EBTNodeResult::InProgress;
		animRef->OnFinishAttackDelegate.BindUObject(this, &UBTTask_NKAttack::OnFinishAttackHandle);
		animRef->Attack();
		return NodeResult;
	}
	
}

EBTNodeResult::Type UBTTask_NKAttack::AbortTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	// empty in base class
	return EBTNodeResult::Aborted;
}

void UBTTask_NKAttack::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	// empty in base class

	//UE_LOG(LogTemp, Log, TEXT("UBTTask_NKAttack - TickTask"));
	//mNodeResult = EBTNodeResult::Succeeded;
	//FinishLatentTask(OwnerComp, mNodeResult);
}

//void UBTTask_NKAttack::OnMessage(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, FName Message, int32 RequestID, bool bSuccess)
//{
//	Super::OnMessage(OwnerComp, NodeMemory, Message, RequestID, bSuccess);
//}

void UBTTask_NKAttack::OnTaskFinished(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, EBTNodeResult::Type TaskResult)
{
	// empty in base class
	UE_LOG(LogTemp, Warning, TEXT("UBTTask_NKAttack - OnTaskFinished"));

}

void UBTTask_NKAttack::OnFinishAttackHandle(UBehaviorTreeComponent* OwnerComp)
{
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}
