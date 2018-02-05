// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_SharedDecorator.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "Animation/AIBase_AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"



void UBTDecorator_SharedDecorator::SetOwner(AActor* InActorOwner)
{
	AIOwner = Cast<ATheLastBastionBaseAIController>(InActorOwner);

	if (AIOwner == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTDecorator_SharedDecorator get AIOwner failed"));
	}

}


void UBTDecorator_SharedDecorator::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	if (AIOwner != nullptr)
	{
		switch (sharedDecorator)
		{
		default:
		case EAISharedDecoratorType::None:
		case EAISharedDecoratorType::AttackConditionCheck:
			break;
		}
	}

}

/** called when underlying node has finished
* this function should be considered as const (don't modify state of object) if node is not instanced! */
void UBTDecorator_SharedDecorator::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult)
{
	if (AIOwner != nullptr)
	{
		switch (sharedDecorator)
		{
		default:
		case EAISharedDecoratorType::None:
			break;
		case EAISharedDecoratorType::AttackConditionCheck:
			AttackConditionCheck_ExecutionFinish();
			break;
		}
	}
}
/** called when underlying node was processed (deactivated or failed to activate)
* this function should be considered as const (don't modify state of object) if node is not instanced! */
void UBTDecorator_SharedDecorator::OnNodeProcessed(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult)
{

}

/** calculates raw, core value of decorator's condition. Should not include calling IsInversed */
bool UBTDecorator_SharedDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{

	switch (sharedDecorator)
	{
	case EAISharedDecoratorType::None:
	default:
		return true;
	case EAISharedDecoratorType::AttackConditionCheck:
		return AttackConditionCheck_CalculateRawConidtionValue(OwnerComp);

	}
}

bool UBTDecorator_SharedDecorator::AttackConditionCheck_CalculateRawConidtionValue(UBehaviorTreeComponent& OwnerComp) const
{

	UBlackboardComponent* const bbc = OwnerComp.GetBlackboardComponent();
	ATheLastBastionBaseAIController* const ctrl = Cast<ATheLastBastionBaseAIController>(OwnerComp.GetAIOwner());
	
	if (ctrl == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT(" AttackConditionCheck_CalculateRawConidtionValue get controller failed "));
		return false;
	}
	
	bool enableToAttack
		= ctrl->GetBaseAICharacter()->GetAnimInstanceRef()->GetCurrentActionState() == EAIActionState::None &&
		bbc->GetValue<UBlackboardKeyType_Float>(ctrl->GetKeyID_ToTargetActorDistanceSqr()) <= 50000;

	return enableToAttack;
}

void UBTDecorator_SharedDecorator::AttackConditionCheck_ExecutionFinish()
{

}


