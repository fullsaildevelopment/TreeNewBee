// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_CombatWait.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "AICharacters/TheLastBastionAIBase.h"

UBTTask_CombatWait::UBTTask_CombatWait(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Wait";
	WaitTime = 5.0f;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_CombatWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTWaitTaskMemory* MyMemory = (FBTWaitTaskMemory*)NodeMemory;
	ATheLastBastionBaseAIController* aiCtrl
		= Cast<ATheLastBastionBaseAIController>(OwnerComp.GetAIOwner());

	if (aiCtrl == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("aiCtrl == nullptr - UBTTask_CombatWait::ExecuteTask"));
		return EBTNodeResult::Failed;
	}

	ATheLastBastionAIBase* aiCharacter = Cast<ATheLastBastionAIBase>(aiCtrl->GetCharacter());
	if (aiCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("aiCharacter == nullptr - UBTTask_CombatWait::ExecuteTask"));
		return EBTNodeResult::Failed;
	}

	WaitTime = FMath::Max(0.0f, aiCharacter->GetAttackWait());

	MyMemory->RemainingWaitTime 
		= FMath::FRandRange(WaitTime, (WaitTime + RandomDeviation));

	return EBTNodeResult::InProgress;
}

FString UBTTask_CombatWait::GetStaticDescription() const
{
	return FString::Printf(TEXT("Duration is overrided"));
}

void UBTTask_CombatWait::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTWaitTaskMemory* MyMemory = (FBTWaitTaskMemory*)NodeMemory;
	MyMemory->RemainingWaitTime -= DeltaSeconds;

	if (MyMemory->RemainingWaitTime <= 0.0f)
	{
		// continue execution from this node
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

