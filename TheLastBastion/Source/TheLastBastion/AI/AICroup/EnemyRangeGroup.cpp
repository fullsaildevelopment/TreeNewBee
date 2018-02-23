// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyRangeGroup.h"
#include "AICharacters/TheLastBastionAllyCharacter.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"



AEnemyRangeGroup::AEnemyRangeGroup()
{
	RangeVision = CreateDefaultSubobject<UBoxComponent>(TEXT("RangeVision"));

	if (RangeVision)
	{
		RangeVision->SetupAttachment(RootComp);
		RangeVision->bGenerateOverlapEvents = true;
		RangeVision->SetCanEverAffectNavigation(false);
		RangeVision->InitBoxExtent(FVector(VisionHalfHeight, VisionHalfHeight, VisionHalfHeight));
		RangeVision->SetCollisionProfileName("EnemyRangeTrigger");
	}

}


void AEnemyRangeGroup::OnRangeVisionOverrlapBegin(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
	const FHitResult & SweepResult)
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

void AEnemyRangeGroup::OnRangeVisionOverrlapEnd(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void AEnemyRangeGroup::SetGroupVisionVolumn(float _maxGroupWidth, float _maxGroupLength)
{

	Super::SetGroupVisionVolumn(_maxGroupWidth, _maxGroupLength);

	_maxGroupWidth += RangeUnitShootingRange;
	_maxGroupLength += RangeUnitShootingRange;

	RangeVision->SetBoxExtent(FVector(_maxGroupLength, _maxGroupWidth, GroupVolumnZ), true);
}

AActor * AEnemyRangeGroup::OnTargetRequest(const ATheLastBastionCharacter * _requestSender)
{
	return OnTargetRequest_Range(_requestSender);
}


