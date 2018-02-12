// Fill out your copyright notice in the Description page of Project Settings.

#include "AllyRangeGroup.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "AI/EnemyGroup.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

AAllyRangeGroup::AAllyRangeGroup()
{

}

void AAllyRangeGroup::BeginPlay()
{
	Super::BeginPlay();
}

void AAllyRangeGroup::OnGroupVolumnOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Cast overlapped actor to enemy group and check if it's valid
	AEnemyGroup* EnemyGroup = Cast<AEnemyGroup>(OtherActor);
	if (EnemyGroup)
	{
		AddThreatByGroup(EnemyGroup);
		if (IsInBattle() == false)
		{   
			bInBattle = true;
			GetWorldTimerManager().SetTimer(TimerHandle_TargetListUpdater, this, &AAllyRangeGroup::UpdateTargetList, 2.0f, true, 0.0f);
		}
	}
	
}

void AAllyRangeGroup::OnGroupVolumnOverrlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AEnemyGroup* EnemyGroup = Cast<AEnemyGroup>(OtherActor);
	if (EnemyGroup)
	{
		//RemoveThreatByGroup(EnemyGroup);
		//IntializeTargetsForGroupMembers();
	}
}

void AAllyRangeGroup::UpdateTargetList()
{   
	// Clear the target list first
	TargetList.Empty();

	// Loop through threat map and add enemies inside shooting range to target list
	for (auto& Elem : ThreatMap)
	{   
		FVector currentEnemyLocation = Elem.Key->GetActorLocation();
		bool IsInRange = GroupVolumn->Bounds.GetBox().IsInside(currentEnemyLocation);
		if (IsInRange == true)
		{
			TargetList.Add(Elem.Key);
		}
	}

	//
	UpdateTargetsForGroupMembers();
}

void AAllyRangeGroup::UpdateTargetsForGroupMembers()
{   
	// give each child a target
	for (int i = 0; i < AICharactersInfo.Num(); i++)
	{
		ATheLastBastionAIBase* CurrentAICharacter = AICharactersInfo[i].AICharacter;
		if (CurrentAICharacter && !CurrentAICharacter->GetIsDead())
		{   
			// get a random target from target list
			int RandomTargetIndex = FMath::RandRange(0, TargetList.Num() - 1);
			ATheLastBastionCharacter* RandomTarget = TargetList[RandomTargetIndex];
			if (RandomTarget && RandomTarget->GetIsDead() == false)
			{
				CurrentAICharacter->SetTarget(RandomTarget);
				DrawDebugLine(GetWorld(), CurrentAICharacter->GetActorLocation(), RandomTarget->GetActorLocation(), FColor::Cyan, false, 5.0f, 0, 2.0f);
			}
		}
	}
}

void AAllyRangeGroup::ClearTargetsForGroupMembers()
{
	for (int i = 0; i < AICharactersInfo.Num(); i++)
	{
		ATheLastBastionAIBase* CurrentAICharacter = AICharactersInfo[i].AICharacter;
		if (CurrentAICharacter && !CurrentAICharacter->GetIsDead())
		{
			CurrentAICharacter->SetTarget(nullptr);
		}
	}
}


