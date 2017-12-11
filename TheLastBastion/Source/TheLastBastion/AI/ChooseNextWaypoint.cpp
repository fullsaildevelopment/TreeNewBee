// Fill out your copyright notice in the Description page of Project Settings.

#include "ChooseNextWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "PatrolRoute.h"

EBTNodeResult::Type UChooseNextWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Protect Against No Patrol Route Component
	auto AIController = OwnerComp.GetAIOwner();
	auto ControlledPawn = AIController->GetPawn();
	auto PatrolRoute = ControlledPawn->FindComponentByClass<UPatrolRoute>();
	if (!ensure(PatrolRoute))
	{
		return EBTNodeResult::Failed;
	}

	// Display a warning when the patrol route is empty
	auto PatrollPoints = PatrolRoute->GetPatrolPoints();
	if (PatrollPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no patrol points in the patrol route ! "));
		return EBTNodeResult::Failed;
	}

	// Set Next Waypoint
	auto BlackboardComp = OwnerComp.GetBlackboardComponent();
	auto Index = BlackboardComp->GetValueAsInt(IndexKey.SelectedKeyName);
	BlackboardComp->SetValueAsObject(WaypointKey.SelectedKeyName, PatrollPoints[Index]);

	// Cyle the index
	auto NextIndex = (Index + 1) % PatrollPoints.Num();
	BlackboardComp->SetValueAsInt(IndexKey.SelectedKeyName, NextIndex);

	return EBTNodeResult::Succeeded;
}

