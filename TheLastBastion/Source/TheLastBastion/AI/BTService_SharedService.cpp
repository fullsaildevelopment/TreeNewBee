// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_SharedService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "TheLastBastionBaseAIController.h"
#include "Animation/AIBase_AnimInstance.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"




void UBTService_SharedService::TickNode(UBehaviorTreeComponent & _ownerComp, uint8* _nodeMemory, float _deltaSecond)
{
	Super::TickNode(_ownerComp, _nodeMemory, _deltaSecond);

	switch (service)
	{
	case EAISharedServiceType::None:
	default:
		break;
	case EAISharedServiceType::UpdateSqrDistanceToTarget:
		UpdateSqrDistanceToTarget(_ownerComp);
		break;
	case EAISharedServiceType::SyncMeleeAttackRotationRateAndVelocity:
		SyncMeleeAttackRotationRateAndVelocity(_ownerComp);
		break;

	}
}

void UBTService_SharedService::UpdateSqrDistanceToTarget(UBehaviorTreeComponent & _ownerComp)
{
	UBlackboardComponent* const bbc = _ownerComp.GetBlackboardComponent();

	ATheLastBastionBaseAIController* const enemyC = Cast<ATheLastBastionBaseAIController>(_ownerComp.GetAIOwner());

	if (enemyC == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateSqrDistanceToTarget get controller failed"));
		return;
	}

	const AActor* const targetActor = Cast<AActor>(bbc->GetValue<UBlackboardKeyType_Object>(enemyC->GetKeyID_TargetActor()));
	if (targetActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateSqrDistanceToTarget get target actor failed"));
		return;
	}

	const APawn* const me = enemyC->GetPawn();
	float distanceToTargetSqr = (me->GetActorLocation() - targetActor->GetActorLocation()).SizeSquared();


	bbc->SetValue<UBlackboardKeyType_Float>(enemyC->GetKeyID_ToTargetActorDistanceSqr(), distanceToTargetSqr);
}


/** Sync the melee attack velocity based on animation frame, 
and rotation rate base off of distance to target and current speed*/
void UBTService_SharedService::SyncMeleeAttackRotationRateAndVelocity(UBehaviorTreeComponent & _ownerComp)
{

	UBlackboardComponent* const bbc = _ownerComp.GetBlackboardComponent();
	ATheLastBastionBaseAIController* const enemyC = Cast<ATheLastBastionBaseAIController>(_ownerComp.GetAIOwner());
	UAIBase_AnimInstance* animRef = enemyC->GetAnimInstance();
	ATheLastBastionEnemyCharacter* me = Cast<ATheLastBastionEnemyCharacter>(enemyC->GetPawn());
	UCharacterMovementComponent* moveComp = nullptr;
	
	if (me)
		moveComp = me->GetCharacterMovement();
	if (animRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("animRef is NULL, UBTService_SharedService::SyncMeleeAttackRotationRateAndVelocity"));
		return;
	}
	if (moveComp == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("moveComp is NULL, UBTService_SharedService::SyncMeleeAttackRotationRateAndVelocity"));
		return;
	}

	FVector Velocity = moveComp->Velocity;
	moveComp->Velocity =  me->GetActorForwardVector() * animRef->GetCurveValue("Speed");
	moveComp->Velocity.Z = Velocity.Z;
	UE_LOG(LogTemp, Log, TEXT("speed: %f"), animRef->GetCurveValue("Speed"));
}
