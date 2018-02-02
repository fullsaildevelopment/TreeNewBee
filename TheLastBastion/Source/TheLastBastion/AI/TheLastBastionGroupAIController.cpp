// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionGroupAIController.h"
#include "AIGroupBase.h"
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

	mGroupPawn = Cast<AAIGroupBase>(_possPawn);
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
	newCommandIndex_KeyID = mBBComp->GetKeyID("NewCommandIndex");
	oldCommandIndex_KeyID = mBBComp->GetKeyID("OldCommandIndex");
	targetForward_KeyID = mBBComp->GetKeyID("TargetForward");
	targetRight_KeyID = mBBComp->GetKeyID("TargetRight");

	//UE_LOG(LogTemp, Warning, TEXT("Possess, %s"), *_possPawn->GetName());
	mBBComp->SetValue<UBlackboardKeyType_Vector>(targetLocation_KeyID, FVector::ZeroVector);
	mBBComp->SetValue<UBlackboardKeyType_Int>(oldCommandIndex_KeyID, 0);
	mBBComp->SetValue<UBlackboardKeyType_Int>(newCommandIndex_KeyID, 0);
	mBBComp->SetValue<UBlackboardKeyType_Vector>(targetForward_KeyID, this->GetActorForwardVector());
	mBBComp->SetValue<UBlackboardKeyType_Vector>(targetRight_KeyID, this->GetActorRightVector());


	// Launch behavior Tree
	mBTComp->StartTree(*bt);
}

void ATheLastBastionGroupAIController::BeginPlay()
{
	Super::BeginPlay();

	// temp code, put myself under player commmand
}
