// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTast_StrafeAttack.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "Animation/AIMelee_AnimInstance.h"

UBTTast_StrafeAttack::UBTTast_StrafeAttack(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = false;
	bNotifyTaskFinished = true;
	bIgnoreRestartSelf = false;
}

EBTNodeResult::Type UBTTast_StrafeAttack::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	///*** empty in base class

	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;

	UBlackboardComponent* const bbc = OwnerComp.GetBlackboardComponent();
	ATheLastBastionBaseAIController* const enemyC = Cast<ATheLastBastionBaseAIController>(OwnerComp.GetAIOwner());
	if (enemyC == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AIOwner is not an enemyC - UBTTast_StrafeAttack::ExecuteTask"));
		return NodeResult;
	}

	UAIBase_AnimInstance* animRef = enemyC->GetAnimInstance();
	if (animRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("animRef is NULL - UBTTast_StrafeAttack::ExecuteTask"));
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
		UE_LOG(LogTemp, Error, TEXT("enemyC->GetPawn() is NULL - UBTTast_StrafeAttack::ExecuteTask"));
		return NodeResult;
	}

	if (animRef->GetCurrentActionState() != EAIActionState::None)
	{
		UE_LOG(LogTemp, Log, TEXT("Is Attacking, failed this task, and move to next task"));
		return NodeResult;
	}
	else
	{
		// check attack distance
		float distanceSqr = (me->GetActorLocation() - targetActor->GetActorLocation()).SizeSquared();
		bbc->SetValue<UBlackboardKeyType_Float>(enemyC->GetKeyID_ToTargetActorDistanceSqr(), distanceSqr);

		if (distanceSqr > meleeComboAttackDistanceSqr)
		{
			UE_LOG(LogTemp, Log, TEXT("Is too far too attack, failed this task, and move to next task"));
			return NodeResult;
		}
		else
		{
			NodeResult = EBTNodeResult::InProgress;
			animRef->OnFinishAttackDelegate.BindUObject(this, &UBTTast_StrafeAttack::OnFinishAttackHandle);

			EAIMeleeAttackType attackType = EAIMeleeAttackType::None;
			if (distanceSqr < dashedAttackDistanceSqr)
				attackType = EAIMeleeAttackType::InPlace;
			else
				attackType = (FMath::RandBool()) ? EAIMeleeAttackType::Move : (EAIMeleeAttackType::Move_InPlace);


			animRef->Attack(attackType);

			return NodeResult;
		}

	}
}

EBTNodeResult::Type UBTTast_StrafeAttack::AbortTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	// empty in base class
	//UE_LOG(LogTemp, Warning, TEXT("UBTTask_NKAttack - AbortTask"));
	return EBTNodeResult::Aborted;
}

void UBTTast_StrafeAttack::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	// empty in base class
}

void UBTTast_StrafeAttack::OnTaskFinished(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, EBTNodeResult::Type TaskResult)
{
	// empty in base class
	//UE_LOG(LogTemp, Warning, TEXT("UBTTask_NKAttack - OnTaskFinished"));

}

void UBTTast_StrafeAttack::OnFinishAttackHandle(UBehaviorTreeComponent* OwnerComp)
{
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}


