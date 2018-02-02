// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_HitReaction.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "Animation/AIBase_AnimInstance.h"

UBTTask_HitReaction::UBTTask_HitReaction(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = false;
	bNotifyTaskFinished = true;
	bIgnoreRestartSelf = false;
}

EBTNodeResult::Type UBTTask_HitReaction::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	///*** empty in base class

	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;

	UBlackboardComponent* const bbc = OwnerComp.GetBlackboardComponent();
	ATheLastBastionBaseAIController* const baseAICtrl = Cast<ATheLastBastionBaseAIController>(OwnerComp.GetAIOwner());
	if (baseAICtrl == nullptr)
	{
		//UE_LOG(LogTemp, Error, TEXT("AIOwner is not an enemyC - UBTTask_NKAttack::ExecuteTask"));
		return NodeResult;
	}

	UAIBase_AnimInstance* animRef = baseAICtrl->GetAnimInstance();
	if (animRef == nullptr)
	{
		//UE_LOG(LogTemp, Error, TEXT("animRef is NULL - UBTTask_NKAttack::ExecuteTask"));
		return NodeResult;
	}

	const AActor* const targetActor = Cast<AActor>(bbc->GetValue<UBlackboardKeyType_Object>(baseAICtrl->GetKeyID_TargetActor()));
	if (targetActor == nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("UpdateSqrDistanceToTarget get target actor failed"));
		return NodeResult;
	}

	const APawn* const me = baseAICtrl->GetPawn();

	if (animRef->GetCurrentActionState()== EAIActionState::GettingHurt)
	{
		NodeResult = EBTNodeResult::InProgress;
		animRef->OnRecoverFromHitSignature.BindUObject(this, &UBTTask_HitReaction::OnRecoverFromHitHandle);
		return NodeResult;
	}
	else
	{
		return NodeResult;
	}

}

EBTNodeResult::Type UBTTask_HitReaction::AbortTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	// empty in base class
	return EBTNodeResult::Aborted;
}

void UBTTask_HitReaction::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
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

void UBTTask_HitReaction::OnTaskFinished(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, EBTNodeResult::Type TaskResult)
{
	// empty in base class
	// UE_LOG(LogTemp, Warning, TEXT("UBTTask_NKAttack - OnTaskFinished"));
}

void UBTTask_HitReaction::OnRecoverFromHitHandle(UBehaviorTreeComponent* OwnerComp)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;

	UBlackboardComponent* const bbc = OwnerComp->GetBlackboardComponent();
	ATheLastBastionBaseAIController* const baseAICtrl = Cast<ATheLastBastionBaseAIController>(OwnerComp->GetAIOwner());
	if (baseAICtrl == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AIOwner is not an enemyC - UBTTask_NKAttack::ExecuteTask"));
		return;
	}


	bbc->SetValue<UBlackboardKeyType_Enum>(baseAICtrl->GetKeyID_CurrentActionState(),
		static_cast<UBlackboardKeyType_Enum::FDataType>(EAIActionState::None));

	FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
}



