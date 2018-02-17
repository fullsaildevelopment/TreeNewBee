// Fill out your copyright notice in the Description page of Project Settings.

#include "AllyRangeGroup.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

AAllyRangeGroup::AAllyRangeGroup()
{
	RangeVision = CreateDefaultSubobject<UBoxComponent>(TEXT("RangeVision"));

	if (RangeVision)
	{
		RangeVision->SetupAttachment(RootComp);
		RangeVision->bGenerateOverlapEvents = true;
		RangeVision->SetCanEverAffectNavigation(false);
		RangeVision->InitBoxExtent(FVector(VisionHalfHeight, VisionHalfHeight, VisionHalfHeight));
		RangeVision->SetCollisionProfileName("AllyRangeTrigger");
	}

}

void AAllyRangeGroup::OnRangeVisionOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Cast overlapped actor to an enemy and check if it's valid
	ATheLastBastionEnemyCharacter* EnemyCharacter = Cast<ATheLastBastionEnemyCharacter>(OtherActor);
	if (EnemyCharacter)
	{
		AddThreat(EnemyCharacter, ThreatGain_AIInit);
		if (bInBattle == false)
		{
			RangeTargetSelect_OnFirstOverlap(EnemyCharacter);
			bInBattle = true;
		}
	}

}

void AAllyRangeGroup::OnRangeVisionOverrlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Cast overlapped actor to an enemy and check if it's valid
	ATheLastBastionEnemyCharacter* EnemyCharacter = Cast<ATheLastBastionEnemyCharacter>(OtherActor);
	if (EnemyCharacter)
	{
		// Remove this enemy from threat map
		RemoveThreat(EnemyCharacter);

		EnemyCharacter->OnBecomeUnvailbleTargetEvent.Broadcast();
		// Request another target for each allies whose target is this enemycharacter
		//UpdateTargetsOnOverlapEnd(EnemyCharacter);
	}
}

//void AAllyRangeGroup::UpdateTargetsOnOverlapEnd(AActor* CurrentTargetActor)
//{
//	for (int i = 0; i < AICharactersInfo.Num(); i++)
//	{
//		ATheLastBastionAIBase* CurrentAICharacter = AICharactersInfo[i].AICharacter;
//		if (CurrentAICharacter && !CurrentAICharacter->GetIsDead())
//		{
//			// check if current ai's target equals the one removed from the threat map
//			if (CurrentTargetActor == CurrentAICharacter->GetTarget())
//			{
//				//Request another target from threat map for current ai
//				OnTargetRequest(CurrentAICharacter);
//			}
//		}
//	}
//}

void AAllyRangeGroup::SetGroupVisionVolumn(float _maxGroupWidth, float _maxGroupLength)
{

	MeleeVision->SetBoxExtent(FVector(_maxGroupWidth, _maxGroupLength, GroupVolumnZ), true);

	_maxGroupWidth += RangeUnitShootingRange;
	_maxGroupLength += RangeUnitShootingRange;

	RangeVision->SetBoxExtent(FVector(_maxGroupWidth, _maxGroupLength, GroupVolumnZ), true);

}

AActor * AAllyRangeGroup::OnTargetRequest(const ATheLastBastionCharacter * _requestSender)
{
	return OnTargetRequest_Range(_requestSender);
}


