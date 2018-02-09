// Fill out your copyright notice in the Description page of Project Settings.

#include "AllyRangeGroup.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
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
	ATheLastBastionEnemyCharacter* EnemyCharacter = Cast<ATheLastBastionEnemyCharacter>(OtherActor);
	if (EnemyCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("overrlap with %s"), *EnemyCharacter->GetName());
		AddThreat(EnemyCharacter, ThreatGain_AIInit);
		IntializeTargetsForGroupMembers();
	}
	
}

void AAllyRangeGroup::OnGroupVolumnOverrlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATheLastBastionEnemyCharacter* EnemyCharacter = Cast<ATheLastBastionEnemyCharacter>(OtherActor);
	if (EnemyCharacter)
	{
		RemoveThreat(EnemyCharacter);
		IntializeTargetsForGroupMembers();
	}
}

void AAllyRangeGroup::IntializeTargetsForGroupMembers()
{   
	int ThreatCount = ThreatMap.Num();
	if (ThreatCount == 0)
	{
		ClearTargetsForGroupMembers();
		return;
	}

	TArray<ATheLastBastionCharacter*> targetList;
	targetList.SetNum(ThreatCount);
	int currentTargetIndex = 0;

	for (auto& Elem : ThreatMap)
	{
		targetList[currentTargetIndex]= Elem.Key;
		currentTargetIndex++;
	}

	// give each child a target
	for (int i = 0; i < AICharactersInfo.Num(); i++)
	{
		ATheLastBastionAIBase* CurrentAICharacter = AICharactersInfo[i].AICharacter;
		if (CurrentAICharacter && !CurrentAICharacter->GetIsDead())
		{
			// get a random target from target list
			int RandomTargetIndex = FMath::RandRange(0, targetList.Num()-1);
			ATheLastBastionCharacter* RandomTarget = targetList[RandomTargetIndex];
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

