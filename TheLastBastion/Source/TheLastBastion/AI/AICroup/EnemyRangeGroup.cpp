// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyRangeGroup.h"
#include "AICharacters/TheLastBastionAllyCharacter.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"



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

	ConstructorHelpers::FObjectFinder<UBehaviorTree> bt(TEXT("/Game/Blueprints/AI/GroupPreset/BT_EnemyRangeGroupAI"));
	if (bt.Succeeded())
		BehaviorTree = bt.Object;
	else
		UE_LOG(LogTemp, Error, TEXT("Can not find behaviorTree - AEnemyRangeGroup::AEnemyRangeGroup"));
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

		bInBattle = true;
		SetRangeGroupTarget_OnOverLap(allyCharacter);

		//if (bInBattle == false)
		//{
		//	RangeTargetSelect_OnFirstOverlap(allyCharacter);
		//	bInBattle = true;
		//}
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

	}
}

void AEnemyRangeGroup::SetGroupVisionVolumn(float _maxGroupWidth, float _maxGroupLength)
{

	MeleeVision->SetBoxExtent(FVector(_maxGroupLength, _maxGroupWidth, GroupVolumnZ), true);

	_maxGroupWidth += RangeUnitShootingRange;
	_maxGroupLength += RangeUnitShootingRange;

	RangeVision->SetBoxExtent(FVector(_maxGroupLength, _maxGroupWidth, GroupVolumnZ), true);
}

void AEnemyRangeGroup::SetRangeGroupTarget_OnOverLap(ATheLastBastionCharacter * _target)
{
	bool additionalTargetInList = false;

	for (int iTarget = 0; iTarget < RangeGroupTargetAmount; iTarget++)
	{
		// if we have room in primary target list, then put the new target in it
		// and mark for target request as a group request after the target is dead
		if (PrimaryTargets[iTarget].PrimaryTarget == nullptr)
		{
			PrimaryTargets[iTarget].PrimaryTarget = _target;
			PrimaryTargets[iTarget].TargetRequestHandle
				= _target->OnBecomeUnvailbleTargetEvent.AddUObject(this,
					&AEnemyRangeGroup::SetRangeGroupTarget_OnRequest);

			additionalTargetInList = true;
			break;
		}
		else if (PrimaryTargets[iTarget].PrimaryTarget == _target)
		{
			// skip the target if we already have them
			break;
		}
	}

	// update target if we have new target
	if (additionalTargetInList)
	{
		TArray<int> availableTarget;

		for (int iTarget = 0; iTarget < RangeGroupTargetAmount; iTarget++)
		{
			if (PrimaryTargets[iTarget].PrimaryTarget != nullptr)
				availableTarget.Add(iTarget);
		}

		int maxIndex_availableTarget = availableTarget.Num() - 1;
		int targetIndex = 0;
		// set all group member target 
		for (int i = 0; i < AICharactersInfo.Num(); i++)
		{
			ATheLastBastionAIBase* CurrentAICharacter = AICharactersInfo[i].AICharacter;
			if (CurrentAICharacter && !CurrentAICharacter->GetIsDead())
			{
				targetIndex = FMath::RandRange(0, maxIndex_availableTarget);
				//UE_LOG(LogTemp, Log, TEXT("%d - AAllyRangeGroup::SetRangeGroupTarget_OnOverLap"), targetIndex);

				CurrentAICharacter->SetTarget(PrimaryTargets[targetIndex].PrimaryTarget, true);
			}
		}
		//UE_LOG(LogTemp, Warning, TEXT("finish assigning - AAllyRangeGroup::SetRangeGroupTarget_OnOverLap"));


	}

}

void AEnemyRangeGroup::SetRangeGroupTarget_OnOverLapEnd(ATheLastBastionCharacter * _target)
{
	// if the escaping target is dead, then do nothing
	if (_target->GetIsDead())
		return;

	for (int iTarget = 0; iTarget < RangeGroupTargetAmount; iTarget++)
	{
		if (PrimaryTargets[iTarget].PrimaryTarget == _target)
		{
			PrimaryTargets[iTarget].PrimaryTarget->
				OnBecomeUnvailbleTargetEvent.Remove(PrimaryTargets[iTarget].TargetRequestHandle);
			PrimaryTargets[iTarget].PrimaryTarget = nullptr;
			break;
		}
	}

	// find all invalid index in primary targets
	TArray<int> availableTarget;

	for (int iTarget = 0; iTarget < RangeGroupTargetAmount; iTarget++)
	{
		if (PrimaryTargets[iTarget].PrimaryTarget != nullptr)
			availableTarget.Add(iTarget);
	}

	int maxIndex_availableTarget = availableTarget.Num() - 1;
	int targetIndex = 0;

	// reassign all group member target 
	for (int i = 0; i < AICharactersInfo.Num(); i++)
	{
		ATheLastBastionAIBase* CurrentAICharacter = AICharactersInfo[i].AICharacter;
		if (CurrentAICharacter && !CurrentAICharacter->GetIsDead())
		{
			targetIndex = FMath::RandRange(0, maxIndex_availableTarget);
			CurrentAICharacter->SetTarget(PrimaryTargets[targetIndex].PrimaryTarget, true);
		}
	}
}


void AEnemyRangeGroup::SetRangeGroupTarget_OnRequest()
{
	//UE_LOG(LogTemp, Warning, TEXT("AAllyRangeGroup :: SetRangeGroupTarget_OnRequest()"));
	ATheLastBastionCharacter*  currentThreat = nullptr;

	// clear the dead dead threats
	for (auto& Elem : ThreatMap)
	{
		currentThreat = Elem.Key;
		//if (currentThreat == nullptr)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("AAIGroupBase::OnTargetRequest_Range - NULL THREAT"));
		//	RemoveThreat(currentThreat);
		//	continue;
		//}

		if (currentThreat->GetIsDead())
		{
			//UE_LOG(LogTemp, Log, TEXT("AAIGroupBase::OnTargetRequest_Range - DEAD THREAT"));
			RemoveThreat(currentThreat);
		}
	}

	// clear the primary targets
	for (int iTarget = 0; iTarget < RangeGroupTargetAmount; iTarget++)
	{
		// some one is invaild, because some one is dead
		if (PrimaryTargets[iTarget].PrimaryTarget)
		{
			if (PrimaryTargets[iTarget].TargetRequestHandle.IsValid())
			{
				PrimaryTargets[iTarget].PrimaryTarget->
					OnBecomeUnvailbleTargetEvent.Remove(PrimaryTargets[iTarget].TargetRequestHandle);
			}
			PrimaryTargets[iTarget].PrimaryTarget = nullptr;
		}

	}


	// how many remain in threat map
	int ThreatCount = ThreatMap.Num();

	// if there is no more threat, then clear all group member target
	if (ThreatCount == 0)
	{
		for (int i = 0; i < AICharactersInfo.Num(); i++)
		{
			ATheLastBastionAIBase* CurrentAICharacter = AICharactersInfo[i].AICharacter;
			if (CurrentAICharacter && !CurrentAICharacter->GetIsDead())
			{
				CurrentAICharacter->SetTarget(nullptr, true);
			}
		}
		return;
	}

	// find the nearest target candidates use manhaton distance
	TArray<FThreat> targetCandidates;
	targetCandidates.SetNum(ThreatCount);
	int currentIndex = 0;

	for (auto& Elem : ThreatMap)
	{
		currentThreat = Elem.Key;
		targetCandidates[currentIndex].Character = currentThreat;
		targetCandidates[currentIndex].GroupThreat = Elem.Value;
		FVector dir = currentThreat->GetActorLocation() - this->GetActorLocation();
		targetCandidates[currentIndex].Manhaton = FMath::Abs(dir.X) + FMath::Abs(dir.Y);
		currentIndex++;
	}

	QuickSortThreatListByManDistance(targetCandidates, 0, ThreatCount - 1);

	// how many target we are going to fill in the primary target list
	int ToFill
		= (ThreatCount >= RangeGroupTargetAmount) ? RangeGroupTargetAmount : ThreatCount;

	for (int iTarget = 0; iTarget < ToFill; iTarget++)
	{
		ATheLastBastionCharacter* _target = targetCandidates[iTarget].Character;

		PrimaryTargets[iTarget].PrimaryTarget = _target;
		PrimaryTargets[iTarget].TargetRequestHandle =
			_target->OnBecomeUnvailbleTargetEvent.AddUObject(this,
				&AEnemyRangeGroup::SetRangeGroupTarget_OnRequest);
	}

	int targetIndex;
	// set all group member target 
	for (int i = 0; i < AICharactersInfo.Num(); i++)
	{
		ATheLastBastionAIBase* CurrentAICharacter = AICharactersInfo[i].AICharacter;
		if (CurrentAICharacter && !CurrentAICharacter->GetIsDead())
		{
			targetIndex = FMath::RandRange(0, ToFill - 1);
			CurrentAICharacter->SetTarget(PrimaryTargets[targetIndex].PrimaryTarget, true);
		}
	}

}

//AActor* AEnemyRangeGroup::OnTargetRequest(const ATheLastBastionCharacter * _requestSender)
//{
//	return OnTargetRequest_Range(_requestSender);
//}


