// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyRangeGroup.h"
#include "AICharacters/TheLastBastionAllyCharacter.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"


void AEnemyRangeGroup::OnGroupVolumnOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Cast overlapped actor to an enemy and check if it's valid
	ATheLastBastionCharacter* allyCharacter = Cast<ATheLastBastionCharacter>(OtherActor);
	if (allyCharacter && allyCharacter->IsEnemy() == false)
	{
		AddThreat(allyCharacter, ThreatGain_AIInit);
		if (bInBattle == false)
		{
			RangeTargetSelect_OnFirstOverlap(allyCharacter);
			bInBattle = true;
		}
	}

}

void AEnemyRangeGroup::OnGroupVolumnOverrlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Cast overlapped actor to an enemy and check if it's valid
	ATheLastBastionAllyCharacter* allyCharacter = Cast<ATheLastBastionAllyCharacter>(OtherActor);
	if (allyCharacter)
	{
		// Remove this enemy from threat map
		RemoveThreat(allyCharacter);

		allyCharacter->OnBecomeUnvailbleTargetEvent.Broadcast();
		// Request another target for each allies whose target is this enemycharacter
		//UpdateTargetsOnOverlapEnd(EnemyCharacter);
	}
}

void AEnemyRangeGroup::SetGroupVolumn(float _maxGroupWidth, float _maxGroupLength)
{

	_maxGroupWidth += RangeUnitShootingRange;
	_maxGroupLength += RangeUnitShootingRange;

	GroupVolumn->SetBoxExtent(FVector(_maxGroupWidth, _maxGroupLength, GroupVolumnZ), true);

}


