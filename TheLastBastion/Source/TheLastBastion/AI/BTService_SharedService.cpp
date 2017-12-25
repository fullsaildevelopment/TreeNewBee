// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_SharedService.h"




#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "TheLastBastionBaseAIController.h"

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

	}
}

void UBTService_SharedService::UpdateSqrDistanceToTarget(UBehaviorTreeComponent & _ownerComp)
{
	UBlackboardComponent* const bbc = _ownerComp.GetBlackboardComponent();

	ATheLastBastionBaseAIController* const ctrl = Cast<ATheLastBastionBaseAIController>(_ownerComp.GetAIOwner());

	if (ctrl == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateSqrDistanceToTarget get controller failed"));
		return;
	}

	const AActor* const targetActor = Cast<AActor>(bbc->GetValue<UBlackboardKeyType_Object>(ctrl->GetKeyID_TargetActor()));
	if (targetActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateSqrDistanceToTarget get target actor failed"));
		return;
	}

	const APawn* const me = ctrl->GetPawn();
	float distanceToTargetSqr = (me->GetActorLocation() - targetActor->GetActorLocation()).SizeSquared();


	bbc->SetValue<UBlackboardKeyType_Float>(ctrl->GetKeyID_ToTargetActorDistanceSqr(), distanceToTargetSqr);
}
