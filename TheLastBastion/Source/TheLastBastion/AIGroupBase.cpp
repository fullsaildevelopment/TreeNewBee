// Fill out your copyright notice in the Description page of Project Settings.

#include "AIGroupBase.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "AI/TheLastBastionGroupAIController.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "AICharacters/TheLastBastionAIBase.h"
#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"



// Sets default values
AAIGroupBase::AAIGroupBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//
	//float halfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);


	float halfHeight = 200.0f;
	GroupVolumn = CreateDefaultSubobject<UBoxComponent>(TEXT("GroupVolumn"));
	if (GroupVolumn)
	{
		GroupVolumn->SetupAttachment(RootComp);
		GroupVolumn->bGenerateOverlapEvents = true;
		GroupVolumn->SetCanEverAffectNavigation(false);
		GroupVolumn->InitBoxExtent(FVector(halfHeight, halfHeight, halfHeight));
		GroupVolumn->SetCollisionProfileName("GroupTrigger");
	}

	MoveComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MoveComp"));
	if (MoveComp)
	{
		MoveComp->UpdatedComponent = RootComp;
	}

	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	if (ArrowComp)
	{
		ArrowComp->SetupAttachment(GroupVolumn);
		ArrowComp->bHiddenInGame = false;
		ArrowComp->ArrowSize = 5.0f;
	}

	AIControllerClass = ATheLastBastionGroupAIController::StaticClass();

	ConstructorHelpers::FObjectFinder<UBehaviorTree> bt(TEXT("/Game/Blueprints/AI/GroupPreset/BT_GroupAI"));
	if (bt.Succeeded())
		BehaviorTree = bt.Object;
	else
		UE_LOG(LogTemp, Error, TEXT("Can not find behaviorTree - AAIGroupBase::AAIGroupBase"));
	//
	bDisabled = false;
	bReformPending = false;
	bIsAgreesive = false;
}

// Called when the game starts or when spawned
void AAIGroupBase::BeginPlay()
{
	Super::BeginPlay();

	if (GroupVolumn && MoveComp)
	{
		SpawnChildGroup();
		GroupVolumn->OnComponentBeginOverlap.AddDynamic(this, &AAIGroupBase::OnGroupVolumnOverrlapBegin);
		GroupVolumn->OnComponentEndOverlap.AddDynamic(this, &AAIGroupBase::OnGroupVolumnOverrlapEnd);
	}

	PlayerHero = Cast<ATheLastBastionHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AAIGroupBase::SpawnChildGroup() {}

void AAIGroupBase::OnReform() {}

void AAIGroupBase::OnGroupVolumnOverrlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("overrlap with %s"), *OtherActor->GetName());
}

void AAIGroupBase::OnGroupVolumnOverrlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("overrlap end with %s"), *OtherActor->GetName());

}

// Called every frame
void AAIGroupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAIGroupBase::CheckGroupCommand()
{
	UBlackboardComponent* bbcGroup = nullptr;
	UBlackboardComponent* bbcChild = nullptr;

	ATheLastBastionGroupAIController* groupC = Cast<ATheLastBastionGroupAIController>(GetController());
	bbcGroup = groupC->GetBlackboardComponent();
	if (bbcGroup == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT(" groupC bbc == nullptr - AAIGroupBase::CheckGroupCommand"));
		return;
	}

	bbcGroup->SetValue<UBlackboardKeyType_Int>(groupC->GetKeyID_NewCommandIndex(), 0);
}

void AAIGroupBase::SetMarchLocation(const FVector & _targetLocation, int _commandIndex) {}

void AAIGroupBase::OnChildDeath(int _childIndex) {}

void AAIGroupBase::AddThreat(ATheLastBastionCharacter * _character, float _threat)
{
	float* threat = ThreatMap.Find(_character);
	if (threat == nullptr)
	{
		ThreatMap.Add(_character, _threat);
	}
	else
	{
		*threat += _threat;
	}
}

void AAIGroupBase::RemoveThreat(ATheLastBastionCharacter * _character)
{
	ThreatMap.Remove(_character);
}

AActor * AAIGroupBase::OnTargetRequest(const AActor* _requestSender)
{
	ATheLastBastionCharacter* currentThreat = nullptr;
	//filter out the dead threat
	for (auto& Elem : ThreatMap)
	{
		currentThreat = Elem.Key;
		if (currentThreat->GetIsDead())
			ThreatMap.Remove(currentThreat);
	}

	int ThreatCount = ThreatMap.Num();
	if (ThreatCount == 0)
		return nullptr;

	// find the nearest target candidates use manhaton distance
	TArray<FThreat> targetCandidates;
	targetCandidates.SetNum(ThreatCount);
	int currentIndex = 0;

	for (auto& Elem : ThreatMap)
	{
		currentThreat = Elem.Key;
		targetCandidates[currentIndex].Character = currentThreat;
		targetCandidates[currentIndex].GroupThreat = Elem.Value;
		FVector dir = currentThreat->GetActorLocation() - _requestSender->GetActorLocation();
		targetCandidates[currentIndex].Manhaton = FMath::Abs(dir.X) + FMath::Abs(dir.Y);
		currentIndex++;
	}

	QuickSortThreatListByManDistance(targetCandidates, 0, ThreatCount - 1);

	// find the most threat among the nearest
	int outIndex = 0;

	int candidatesCount = ThreatCount;
	if (candidatesCount > TARGETREQUEST_UpLimit)
		candidatesCount = TARGETREQUEST_UpLimit;
	else if (candidatesCount == 1)
		return targetCandidates[0].Character;

	for (int iCandi = 1; iCandi < candidatesCount; iCandi++)
	{
		if (targetCandidates[iCandi].GroupThreat > targetCandidates[outIndex].GroupThreat)
			outIndex = iCandi;
	}

	return targetCandidates[outIndex].Character;
}

void AAIGroupBase::QuickSortThreatListByManDistance(TArray<FThreat>& _threatList, int _left, int _right) const
{
	int i = _left, j = _right;
	FThreat temp;
	float pivot = _threatList[(_left + _right)* 0.5f].Manhaton;

	while (i <= j)
	{
		while (_threatList[i].Manhaton < pivot)
			i++;
		while (_threatList[j].Manhaton > pivot)
			j--;
		if (i <= j)
		{
			temp = _threatList[i];
			_threatList[i] = _threatList[j];
			_threatList[j] = temp;
			i++;
			j--;
		}
	}

	if (_left < j)
		QuickSortThreatListByManDistance(_threatList, _left, j);

	if (i < _right)
		QuickSortThreatListByManDistance(_threatList, i, _right);

}

int AAIGroupBase::GetMaxColoumnCount() const
{
	return 0;
}


TArray<class ATheLastBastionAIBase*> AAIGroupBase::GetColumnAt(int _index) const
{
	TArray<class ATheLastBastionAIBase*> out;

	// check validation

	int maxRowSize = FormationInfo[0];
	int maxRow = FormationInfo.Num();

	ensure(_index <= maxRowSize - 1 && _index >= 0);

	int FormationSlotIndex;
	for (int iRow = 0; iRow < maxRow; iRow++)
	{
		FormationSlotIndex = iRow * maxRowSize + _index;
		if (AICharactersInfo.IsValidIndex(FormationSlotIndex))
		{
			out.Add(AICharactersInfo[FormationSlotIndex].AICharacter);
		}
	}

	return out;
}

TArray<class ATheLastBastionAIBase*> AAIGroupBase::GetRowAt(int _index) const
{
	TArray<class ATheLastBastionAIBase*> out;

	int maxRowSize = FormationInfo[0];
	int maxRow = FormationInfo.Num();

	ensure(_index <= maxRow - 1 && _index >= 0);

	int outSize = 0;
	int startIndex = 0;
	
	if (_index == maxRow - 1)
	{
		startIndex = _index * maxRowSize;
		outSize = AICharactersInfo.Num() - startIndex;
	}
	else
	{
		startIndex = _index * maxRowSize;
		outSize = maxRowSize;
	}

	out.SetNum(outSize);

	for (int i = 0; i < outSize; i++)
	{
		out[i] = AICharactersInfo[i + startIndex].AICharacter;
	}
	return out;

}

TArray<class ATheLastBastionAIBase*> AAIGroupBase::GetFrontLine() const
{
	int frontLine = 0;
	return GetRowAt(frontLine);
}

TArray<class ATheLastBastionAIBase*> AAIGroupBase::GetBackLine() const
{
	int backLine = FormationInfo.Num() - 1;
	return GetRowAt(backLine);
}

TArray<class ATheLastBastionAIBase*> AAIGroupBase::GetRightLine() const
{
	int rightLine = FormationInfo[0] - 1;
	return GetColumnAt(rightLine);
}

TArray<class ATheLastBastionAIBase*> AAIGroupBase::GetLeftLine() const
{
	int leftLine = 0;
	return GetColumnAt(leftLine);
}


void AAIGroupBase::SwapChildenOrder() {}

void AAIGroupBase::SendGroupCommand(int _commandIndex)
{
	// give each child an group command
	ATheLastBastionBaseAIController* baseAICtrl = nullptr;
	for (int i = 0; i < AICharactersInfo.Num(); i++)
	{
		ATheLastBastionAIBase* baseAI = AICharactersInfo[i].AICharacter;
		if (baseAI && !baseAI->GetIsDead())
		{
			baseAICtrl = Cast<ATheLastBastionBaseAIController>(AICharactersInfo[i].AICharacter->GetController());
			if (baseAICtrl == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("baseAICtrl == nullptr - AAIGroupBase::SetMarchLocation"));
				continue;
			}
			// this command is given by players, 
			// if we have unfinished command, clear them for new command
			baseAICtrl->SetOldCommandIndex_BBC(0);
			baseAICtrl->SetNewCommandIndex_BBC(_commandIndex);
		}
	}

}

int AAIGroupBase::CheckTargetRelativeWhereAbout(const AActor * const _target) const
{

	FVector targetLocation = _target->GetActorLocation();
	FVector myLocation = GetActorLocation();
	FVector away = (targetLocation - myLocation).GetUnsafeNormal();
	
	FVector myFWD = GetActorForwardVector();
	
	float dotFwd = FVector::DotProduct(myFWD, away);
	if (dotFwd > COS45)
	{	
		// target is in front
		FVector targetFWD = _target->GetActorForwardVector();
		float dotHeading = FVector::DotProduct(myFWD, targetFWD);
		if (dotHeading > 0)
			return TargetAtFront_Chasing;
		else
			return TargetAtFront_Face2Face;

	}
	else if (dotFwd < -COS45)
	{
		// target is in back
		FVector targetFWD = _target->GetActorForwardVector();
		float dotHeading = FVector::DotProduct(myFWD, targetFWD);
		if (dotHeading > 0)
			return TargetFromBack_Chasing;
		else
			return TargetFromBack_Back2Back;
	}
	else
	{
		// target is on side
		FVector myRight = GetActorRightVector();
		float dotRight = FVector::DotProduct(myRight, away);
		if (dotRight > 0)
		{
			// target is on right
			return TargetFromRight;
		}
		else
		{
			// target in on left
			return TargetFromLeft;
		}
	}

	return 0;
}

void AAIGroupBase::PairColumn(AAIGroupBase * const _enemyGroup, int _myColumn, int _theirColumn)
{

	TArray<ATheLastBastionAIBase*> ourColGroup = GetColumnAt(_myColumn);
	TArray<ATheLastBastionAIBase*> theirColGroup = _enemyGroup->GetColumnAt(_theirColumn);
	int ourColSize = ourColGroup.Num();
	int theirColSize = theirColGroup.Num();
	int randomIndex;

	if (ourColSize > theirColSize)
	{
		for (int iRow = 0; iRow < ourColSize; iRow++)
		{
			if (iRow <= theirColSize - 1)
			{
				ourColGroup[iRow]->SetTarget(theirColGroup[iRow]);
				theirColGroup[iRow]->SetTarget(ourColGroup[iRow]);
			}
			else
			{
				randomIndex = FMath::RandRange(0, theirColSize - 1);
				ourColGroup[iRow]->SetTarget(theirColGroup[randomIndex]);
			}

		}
	}
	else
	{
		for (int iRow = 0; iRow < theirColSize; iRow++)
		{
			if (iRow <= ourColSize - 1)
			{
				ourColGroup[iRow]->SetTarget(theirColGroup[iRow]);
				theirColGroup[iRow]->SetTarget(ourColGroup[iRow]);
			}
			else
			{
				randomIndex = FMath::RandRange(0, ourColSize - 1);
				theirColGroup[iRow]->SetTarget(ourColGroup[randomIndex]);
			}

		}
	}
}

void AAIGroupBase::AssignColumn(AAIGroupBase * const _targetGroup, int _myColumn, int _theirColumn)
{

	TArray<ATheLastBastionAIBase*> ourColGroup = GetColumnAt(_myColumn);
	TArray<ATheLastBastionAIBase*> theirColGroup = _targetGroup->GetColumnAt(_theirColumn);
	int ourColSize = ourColGroup.Num();
	int theirColSize = theirColGroup.Num();
	int randomIndex;


	if (ourColSize > theirColSize)
	{
		for (int iRow = 0; iRow < ourColSize; iRow++)
		{
			if (iRow <= theirColSize - 1)
			{
				ourColGroup[iRow]->SetTarget(theirColGroup[iRow]);
			}
			else
			{
				randomIndex = FMath::RandRange(0, theirColSize - 1);
				ourColGroup[iRow]->SetTarget(theirColGroup[randomIndex]);
			}

		}
	}
	else
	{
		for (int iRow = 0; iRow < theirColSize; iRow++)
		{
			if (iRow <= ourColSize - 1)
			{
				ourColGroup[iRow]->SetTarget(theirColGroup[iRow]);
			}

		}
	}

}

void AAIGroupBase::InitMeleeCombat(AAIGroupBase* _targetGroup)
{


	FVector ourForwardVector = GetActorForwardVector();
	FVector theirForwardVector = _targetGroup->GetActorForwardVector();

	float dotProduct = FVector::DotProduct(ourForwardVector, theirForwardVector);
	if (dotProduct < -COS22_5)
	{
		// check number of column for two group
		int ourColCount = this->GetMaxColoumnCount();
		int theirColCount = _targetGroup->GetMaxColoumnCount();

		// find middle column
		int ourMiddleColumn = ourColCount * 0.5f;
		int theirMiddleColumn = theirColCount * 0.5f;

		int maxOffsetTimeFromMiddle;
		if (ourColCount > theirColCount)
			maxOffsetTimeFromMiddle = UKismetMathLibrary::Max(ourMiddleColumn, ourColCount - 1 - ourMiddleColumn);
		else
			maxOffsetTimeFromMiddle = UKismetMathLibrary::Max(theirMiddleColumn, theirColCount - 1 - theirMiddleColumn);



		for (int iOffset = 0; iOffset <= maxOffsetTimeFromMiddle; iOffset++)
		{
			int ourCurrrentColumn, theirCurrentColumn;
			if (iOffset == 0)
			{
				ourCurrrentColumn = iOffset + ourMiddleColumn;
				theirCurrentColumn = iOffset + theirMiddleColumn;

				PairColumn(_targetGroup, ourCurrrentColumn, theirCurrentColumn);
			}
			else
			{
				ourCurrrentColumn = iOffset + ourMiddleColumn;
				theirCurrentColumn = -iOffset + theirMiddleColumn;

				if (ourCurrrentColumn < ourColCount && theirCurrentColumn >= 0)
				{
					PairColumn(_targetGroup, ourCurrrentColumn, theirCurrentColumn);
				}
				else
				{

					if (ourCurrrentColumn >= ourColCount && theirCurrentColumn < 0)
					{
						// do nothing, both invalid
					}
					else if (ourCurrrentColumn >= ourColCount)
					{
						// their attack our nearest column
						_targetGroup->AssignColumn(this, theirCurrentColumn, ourColCount - 1);
					}
					else if (theirCurrentColumn < 0)
					{
						this->AssignColumn(_targetGroup, ourCurrrentColumn, 0);

					}

				}


				ourCurrrentColumn = -iOffset + ourMiddleColumn;
				theirCurrentColumn = +iOffset + theirMiddleColumn;


				if (ourCurrrentColumn >= 0 && theirCurrentColumn < theirColCount)
				{
					PairColumn(_targetGroup, ourCurrrentColumn, theirCurrentColumn);
				}
				else
				{

					if (ourCurrrentColumn < 0 && theirCurrentColumn >= theirColCount)
					{
						// do nothing, both invalid
					}
					else if (ourCurrrentColumn < 0)
					{
						// their attack our nearest column
						_targetGroup->AssignColumn(this, theirCurrentColumn, 0);
					}
					else if (theirCurrentColumn >= theirColCount)
					{
						this->AssignColumn(_targetGroup, ourCurrrentColumn, theirColCount - 1);

					}

				}

			}
		}

	}

}
