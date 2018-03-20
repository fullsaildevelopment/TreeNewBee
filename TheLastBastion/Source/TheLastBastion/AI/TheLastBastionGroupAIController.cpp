// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionGroupAIController.h"
#include "AI/EnemyGroup.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"



ATheLastBastionGroupAIController::ATheLastBastionGroupAIController(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	mBTComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
	mBBComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComp"));

}

void ATheLastBastionGroupAIController::Possess(APawn * _possPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("ATheLastBastionBaseAIController possess on %s"), *_possPawn->GetName());

	Super::Possess(_possPawn);

	mGroupPawn = Cast<AEnemyGroup>(_possPawn);
	if (mGroupPawn == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ATheLastBastionGroupAIController possess non - AAIGroupBase type, %s"), *_possPawn->GetName());
		return;
	}


	UBehaviorTree* const bt = mGroupPawn->GetBehaviorTree();
	if (bt == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("This ATheLastBastionEnemyCharacter should be assigned with a behavior Tree, %s"), *_possPawn->GetName());
		return;
	}

	UBlackboardData* const bbData = bt->BlackboardAsset;
	if (bbData == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("This behavior Tree should be assigned with a blackborad, %s"), *_possPawn->GetName());
		return;
	}

	mBBComp->InitializeBlackboard(*bbData);
	Blackboard = mBBComp;

	// Fetch the Keys
	targetActor_KeyID = mBBComp->GetKeyID("targetActor");
	targetLocation_KeyID = mBBComp->GetKeyID("targetLocation");
	bIsMoving_KeyID = mBBComp->GetKeyID("bIsMoving");
	bAtDestination_KeyID = mBBComp->GetKeyID("bAtDestination");
	bIsCharging_KeyID = mBBComp->GetKeyID("bIsCharging");
	bNewWayPoint_KeyID = mBBComp->GetKeyID("bNewWayPoint");


	//UE_LOG(LogTemp, Warning, TEXT("Possess, %s"), *_possPawn->GetName());
	mBBComp->SetValue<UBlackboardKeyType_Vector>(targetLocation_KeyID, FVector::ZeroVector);
	mBBComp->SetValue<UBlackboardKeyType_Int>(bIsMoving_KeyID, 0);
	SetNewWayPoint_BBC(true);
	// Launch behavior Tree
	mBTComp->StartTree(*bt);
}

void ATheLastBastionGroupAIController::BeginPlay()
{
	Super::BeginPlay();

	// temp code, put myself under player commmand
}

void ATheLastBastionGroupAIController::SetTargetLocation_BBC(const FVector & _targetLocation)
{
	mBBComp->SetValue<UBlackboardKeyType_Vector>(targetLocation_KeyID, _targetLocation);
}

void ATheLastBastionGroupAIController::SetIsMoving_BBC(bool _val)
{
	mBBComp->SetValue<UBlackboardKeyType_Bool>(bIsMoving_KeyID, _val);
}

void ATheLastBastionGroupAIController::SetAtDestination_BBC(bool _val)
{
	mBBComp->SetValue<UBlackboardKeyType_Bool>(bAtDestination_KeyID, _val);
}

void ATheLastBastionGroupAIController::SetIsCharging_BBC(bool _val)
{
	mBBComp->SetValue<UBlackboardKeyType_Bool>(bIsCharging_KeyID, _val);
}

void ATheLastBastionGroupAIController::SetNewWayPoint_BBC(bool _val)
{
	mBBComp->SetValue<UBlackboardKeyType_Bool>(bNewWayPoint_KeyID, _val);
}

FVector ATheLastBastionGroupAIController::GetTargetLocation_BBC() const
{
	return 	mBBComp->GetValue<UBlackboardKeyType_Vector>(targetLocation_KeyID);
}

bool ATheLastBastionGroupAIController::GetIsMoving_BBC() const
{
	return 	mBBComp->GetValue<UBlackboardKeyType_Bool>(bIsMoving_KeyID);
}

bool ATheLastBastionGroupAIController::GetAtDestination_BBC() const
{
	return 	mBBComp->GetValue<UBlackboardKeyType_Bool>(bAtDestination_KeyID);
}

bool ATheLastBastionGroupAIController::GetIsCharging_BBC() const
{
	return 	mBBComp->GetValue<UBlackboardKeyType_Bool>(bIsCharging_KeyID);
}

bool ATheLastBastionGroupAIController::GetNewWayPoint_BBC() const
{
	return 	mBBComp->GetValue<UBlackboardKeyType_Bool>(bNewWayPoint_KeyID);
}
