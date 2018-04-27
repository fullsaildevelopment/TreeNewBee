// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTast_StrafeAttack.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "TheLastBastionCharacter.h"
#include "AICharacters/TheLastBastionAIBase.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Animation/AIMelee_AnimInstance.h"

#include "DrawDebugHelpers.h"


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
	ATheLastBastionBaseAIController* const baseAICtrl = Cast<ATheLastBastionBaseAIController>(OwnerComp.GetAIOwner());
	if (baseAICtrl == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AIOwner is not an baseAICtrl - UBTTast_StrafeAttack::ExecuteTask"));
		return NodeResult;
	}

	UAIBase_AnimInstance* animRef = baseAICtrl->GetAnimInstance();
	if (animRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("animRef is NULL - UBTTast_StrafeAttack::ExecuteTask"));
		return NodeResult;
	}


	const ATheLastBastionCharacter* const targetActor = Cast<ATheLastBastionCharacter>(bbc->GetValue<UBlackboardKeyType_Object>(baseAICtrl->GetKeyID_TargetActor()));
	if (targetActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("targetActor == nullptr - UBTTast_StrafeAttack::ExecuteTask"));
		//baseAICtrl->SetIsRelocate(false);
		return NodeResult;
	}

	ATheLastBastionAIBase* const me = Cast<ATheLastBastionAIBase>(baseAICtrl->GetPawn());
	if (me == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("baseAICtrl->GetPawn() is NULL - UBTTast_StrafeAttack::ExecuteTask"));
		//baseAICtrl->SetIsRelocate(false);
		return NodeResult;
	}

	if (animRef->GetCurrentActionState() != EAIActionState::None)
	{
		//UE_LOG(LogTemp, Log, TEXT("Is Attacking, failed this task, and move to next task"));
		//baseAICtrl->SetIsRelocate(false);
		return NodeResult;
	}
	else
	{
		// check attack distance
		float distanceSqr = (me->GetActorLocation() - targetActor->GetActorLocation()).SizeSquared();
		bbc->SetValue<UBlackboardKeyType_Float>(baseAICtrl->GetKeyID_ToTargetActorDistanceSqr(), distanceSqr);

		if (distanceSqr > me->GetMeleeAttackDist_Sq())
		{
			//UE_LOG(LogTemp, Log, TEXT("Is too far too attack, failed this task, and move to next task"));
			//baseAICtrl->SetIsRelocate(false);
			return NodeResult;
		}
		else if (targetActor->GetIsDead())
		{
			//baseAICtrl->SetIsRelocate(false);
			return EBTNodeResult::Succeeded;
		}
		else
		{
			// Setup attack strategy
			FHitResult hitResult;
			ECollisionChannel objectColType = (me->IsEnemy())? ECC_EnemyBody:ECC_HeroBody;
			FCollisionQueryParams queryParams;
			queryParams.AddIgnoredActor(me);

			// skip attack if there is friend unit in front of us 
			const bool bHit = GetWorld()->LineTraceSingleByObjectType(hitResult,
				me->GetActorLocation(), targetActor->GetActorLocation(), objectColType, queryParams);
			if (bHit)
			{
				//UE_LOG(LogTemp, Log, TEXT("Friendly Ahead - UBTTast_StrafeAttack::ExecuteTask"));
				return EBTNodeResult::Succeeded;
			}


			NodeResult = EBTNodeResult::Failed;
			//animRef->OnFinishAttackDelegate.BindUObject(this, &UBTTast_StrafeAttack::OnFinishAttackHandle);



			//EAIMeleeAttackType attackType = EAIMeleeAttackType::None;
			//if (distanceSqr < me->GetMinimumMoveAttackDist_Sq())
			//	attackType = EAIMeleeAttackType::InPlace;
			//else
			//	attackType = (FMath::RandBool()) ? EAIMeleeAttackType::Move : (EAIMeleeAttackType::Move_InPlace);

			baseAICtrl->SetIsPaused_BBC(true);
			animRef->Attack(me->MeleeComboSelection(distanceSqr), me->GetComboCounter());
			//baseAICtrl->SetIsRelocate(false);
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


