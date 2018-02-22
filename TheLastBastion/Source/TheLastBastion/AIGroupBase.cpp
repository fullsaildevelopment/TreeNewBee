// Fill out your copyright notice in the Description page of Project Settings.

#include "AIGroupBase.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/WidgetComponent.h"
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


	MeleeVision = CreateDefaultSubobject<UBoxComponent>(TEXT("GroupVolumn"));
	if (MeleeVision)
	{
		MeleeVision->SetupAttachment(RootComp);
		MeleeVision->bGenerateOverlapEvents = true;
		MeleeVision->SetCanEverAffectNavigation(false);
		MeleeVision->InitBoxExtent(FVector(VisionHalfHeight, VisionHalfHeight, VisionHalfHeight));
	}

	MoveComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MoveComp"));
	if (MoveComp)
	{
		MoveComp->UpdatedComponent = RootComp;
	}

	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	if (ArrowComp)
	{
		ArrowComp->SetupAttachment(MeleeVision);
		ArrowComp->bHiddenInGame = false;
		ArrowComp->ArrowSize = 5.0f;
	}

	AIControllerClass = ATheLastBastionGroupAIController::StaticClass();

	ConstructorHelpers::FObjectFinder<UBehaviorTree> bt(TEXT("/Game/Blueprints/AI/GroupPreset/BT_GroupAI"));
	if (bt.Succeeded())
		BehaviorTree = bt.Object;
	else
		UE_LOG(LogTemp, Error, TEXT("Can not find behaviorTree - AAIGroupBase::AAIGroupBase"));

	GroupHUD = CreateDefaultSubobject<UWidgetComponent>(TEXT("GroupHUD"));
	if (GroupHUD)
	{
		GroupHUD->SetupAttachment(MeleeVision);
		GroupHUD->SetDrawAtDesiredSize(true);
		GroupHUD->RelativeLocation = FVector(0, 0, 250);
		GroupHUD->SetWidgetSpace(EWidgetSpace::Screen);
		GroupHUD->bGenerateOverlapEvents = false;
		GroupHUD->SetCollisionProfileName("UI");
	}


	//
	bDisabled = false;
	bReformPending = false;
	bIsAgreesive = false;
}

// Called when the game starts or when spawned
void AAIGroupBase::BeginPlay()
{
	Super::BeginPlay();




	if (MeleeVision)
	{
		MeleeVision->OnComponentBeginOverlap.AddDynamic(this, &AAIGroupBase::OnMeleeVisionOverrlapBegin);
		MeleeVision->OnComponentEndOverlap.AddDynamic(this, &AAIGroupBase::OnMeleeVisionOverrlapEnd);
	}

	if (RangeVision)
	{
		RangeVision->OnComponentBeginOverlap.AddDynamic(this, &AAIGroupBase::OnRangeVisionOverrlapBegin);
		RangeVision->OnComponentEndOverlap.AddDynamic(this, &AAIGroupBase::OnRangeVisionOverrlapEnd);
	}


	if (MoveComp)
	{
		SpawnChildGroup();
	}
	ToggleHUDVisibility(false);

	PlayerHero = Cast<ATheLastBastionHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	GetWorldTimerManager().SetTimer(mGroupUpdateTimer, this, &AAIGroupBase::Update, 1.0f, true, 1.0f);
}

void AAIGroupBase::Update()
{

	if (bInBattle)
	{
		UpdateGroupVolumnDuringBattle();
	}

}

void AAIGroupBase::UpdateGroupVolumnDuringBattle()
{
	FVector newLocation = FVector::ZeroVector;
	int groupSize = AICharactersInfo.Num();
	float divBy = GetDivider(groupSize);
	for (int i = 0; i < groupSize; i++)
	{
		newLocation += AICharactersInfo[i].AICharacter->GetActorLocation();
	}
	newLocation *= divBy;
	//UE_LOG(LogTemp, Log, 
	//	TEXT("AAIGroupBase::UpdateGroupVolumnDuringBattle %f, %f,%f   - %s"),
	//	newLocation.X , newLocation.Y , newLocation.Z, *this->GetName());

	SetActorLocation(newLocation);
}

void AAIGroupBase::SpawnChildGroup() {}

void AAIGroupBase::OnReform() {}

void AAIGroupBase::RangeTargetSelect_OnFirstOverlap(AActor* TargetActor)
{
	// give each child a target
	for (int i = 0; i < AICharactersInfo.Num(); i++)
	{
		ATheLastBastionAIBase* CurrentAICharacter = AICharactersInfo[i].AICharacter;
		if (CurrentAICharacter && !CurrentAICharacter->GetIsDead())
		{
			CurrentAICharacter->SetTarget(TargetActor);
			//DrawDebugLine(GetWorld(), CurrentAICharacter->GetActorLocation(), TargetActor->GetActorLocation(), FColor::Cyan, false, 5.0f, 0, 2.0f);
		}
	}
}

void AAIGroupBase::OnMeleeVisionOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {}

void AAIGroupBase::OnMeleeVisionOverrlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {}

void AAIGroupBase::OnRangeVisionOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {}

void AAIGroupBase::OnRangeVisionOverrlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {}

//void AAIGroupBase::OnGroupVolumnOverrlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
//{
//	UE_LOG(LogTemp, Warning, TEXT("overrlap with %s"), *OtherActor->GetName());
//}
//
//void AAIGroupBase::OnGroupVolumnOverrlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
//{
//
//	UE_LOG(LogTemp, Warning, TEXT("overrlap end with %s"), *OtherActor->GetName());
//
//}

void AAIGroupBase::SetGroupVisionVolumn(float _maxGroupWidth, float _maxGroupLength)
{
	MeleeVision->SetBoxExtent(FVector(_maxGroupLength, _maxGroupWidth, GroupVolumnZ), true);

	MeleeVision->RelativeLocation = FVector::ZeroVector;
	MeleeVision->AddRelativeLocation(FVector(-0.5f * _maxGroupLength + GroupFrontExtraVision, 0, 0));
}

float AAIGroupBase::GetDivider(int _index) const
{
	ensure(_index != 0 && _index <= MaxGroupSize);
	switch (_index)
	{
	case 1:
		return 1.0f;
	case 2:
		return 0.5f;
	case 3:
		return 0.333f;
	case 4:
		return 0.25f;
	case 5:
		return 0.2f;
	case 6:
		return 0.1667f;
	case 7:
		return 0.1428f;
	case 8:
		return 0.125f;
	case 9:
		return 0.111f;
	case 10:
		return 0.1f;
	case 11:
		return 0.0909f;
	case 12:
		return 0.0833f;
	case 13:
		return 0.0769f;
	case 14:
		return 0.0714f;
	case 15:
		return 0.0667f;
	case 16:
		return 0.0625f;
	case 17:
		return 0.0588f;
	case 18:
		return 0.0556f;
	case 19:
		return 0.0526f;
	case 20:
		return 0.05f;
	default:
		return 1.0f;
	}
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

void AAIGroupBase::AddThreatByGroup(AAIGroupBase * _targetGroup)
{
	int targetGroupSize = _targetGroup->GetGroupSize();

	for (int i = 0; i < targetGroupSize; i++)
	{
		AddThreat(_targetGroup->GetGroupMemberAt(i), ThreatGain_AIInit);
	}
}

void AAIGroupBase::RemoveThreat(ATheLastBastionCharacter * _character)
{
	ThreatMap.Remove(_character);
	// check if we have remaining threat
	if (ThreatMap.Num() == 0)
	{
		// there is no more threat
		bInBattle = false;
	}
}

void AAIGroupBase::RemoveThreatByGroup(AAIGroupBase * _targetGroup)
{
	for (int i = 0; i <  _targetGroup->GetGroupSize(); i++)
	{
		ThreatMap.Remove(_targetGroup->GetGroupMemberAt(i));
	}

	if (ThreatMap.Num() == 0)
	{
		// there is no more threat
		bInBattle = false;
	}

}

AActor * AAIGroupBase::OnTargetRequest(const ATheLastBastionCharacter* _requestSender)
{
	return OnTargetRequest_Melee(_requestSender);
}

AActor * AAIGroupBase::OnTargetRequest_Melee(const ATheLastBastionCharacter * _requestSender)
{
	ATheLastBastionCharacter* currentThreat = nullptr;
	//filter out the dead threat
	for (auto& Elem : ThreatMap)
	{
		currentThreat = Elem.Key;
		if (currentThreat == nullptr || currentThreat->GetIsDead())
			RemoveThreat(currentThreat);
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

AActor * AAIGroupBase::OnTargetRequest_Range(const ATheLastBastionCharacter * _requestSender)
{
	ATheLastBastionCharacter* currentThreat = nullptr;
	//filter out the dead threat
	for (auto& Elem : ThreatMap)
	{
		currentThreat = Elem.Key;
		if (currentThreat == nullptr || currentThreat->GetIsDead())
			RemoveThreat(currentThreat);
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

	int remainShooter = AICharactersInfo.Num();
	int maxTarget = remainShooter * 0.5f;
	int targetSelectionRandomRange = 0;

	targetSelectionRandomRange = (maxTarget < ThreatCount) ? maxTarget : ThreatCount;
	targetSelectionRandomRange--;
	targetSelectionRandomRange = (targetSelectionRandomRange < 0) ? 0 : targetSelectionRandomRange;
	int outIndex = FMath::RandRange(0, targetSelectionRandomRange);

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

void AAIGroupBase::ToggleHUDVisibility(bool _val)
{
	UUserWidget* hud = GroupHUD->GetUserWidgetObject();

	if (hud)
	{
		if (_val)
			hud->SetVisibility(ESlateVisibility::Visible);
		else
			hud->SetVisibility(ESlateVisibility::Hidden);
	}

}

int AAIGroupBase::GetMaxColoumnCount() const
{
	return 0;
}

int AAIGroupBase::GetMaxRowCount() const
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
		float dotHeadingFwd = FVector::DotProduct(myFWD, targetFWD);

		if (dotHeadingFwd > COS45)
		{
			UE_LOG(LogTemp, Log, TEXT("TargetAtFront_Chasing"));
			return TargetAtFront_Chasing;
		}
		else if (dotHeadingFwd < -COS45)
		{
			UE_LOG(LogTemp, Log, TEXT("TargetAtFront_Face2Face"));
			return TargetAtFront_Face2Face;
		}
		else
		{

			FVector targetRight = _target->GetActorRightVector();
			float dotHeadingAgainstRight = FVector::DotProduct(myFWD, targetRight);
			if (dotHeadingAgainstRight > 0)
			{
				UE_LOG(LogTemp, Log, TEXT("TargetAtFront_FlankItsLeft"));
				return TargetAtFront_FlankItsLeft;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("TargetAtFront_FlankItsRight"));
				return TargetAtFront_FlankItsRight;
			}
		}
	}
	else if (dotFwd < -COS45)
	{
		// target is in back
		FVector targetFWD = _target->GetActorForwardVector();
		float dotHeading = FVector::DotProduct(myFWD, targetFWD);


		if (dotHeading  > COS45)
		{
			// add negative effect - being surprised on group member for
			// short time period
			UE_LOG(LogTemp, Log, TEXT("Be surprised - TargetFromBack_BeingChased"));
			UE_LOG(LogTemp, Log, TEXT("TargetFromBack_BeingChased"));
			return TargetFromBack_BeingChased;
		}
		else if (dotHeading  < -COS45)
		{
			UE_LOG(LogTemp, Log, TEXT("TargetFromBack_Back2Back"));
			return TargetFromBack_Back2Back;
		}
		else
		{
			FVector targetRight = _target->GetActorRightVector();
			float dotHeadingAgainstRight = FVector::DotProduct(myFWD, targetRight);
			if (dotHeadingAgainstRight > 0)
			{
				UE_LOG(LogTemp, Log, TEXT("TargetFromBack_ToMyLeft"));
				return TargetFromBack_ToMyLeft;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("TargetFromBack_ToMyRight"));
				return TargetFromBack_ToMyRight;
			}

		}
	}
	else
	{
		// target is on side
		FVector myRight = GetActorRightVector();
		float dotRight = FVector::DotProduct(myRight, away);
		if (dotRight > 0)
		{
			// target on Right
			FVector targetFWD = _target->GetActorForwardVector();
			float dotHeading = FVector::DotProduct(myRight, targetFWD);

			if (dotHeading < 0)
			{
				if (dotHeading  < -COS45)
				{
					// add negative effect - being flanked on group member for 
					// short time period
					UE_LOG(LogTemp, Log, TEXT("being flanked - TargetFromRight_FaceToUs"));
				}
				UE_LOG(LogTemp, Log, TEXT("TargetFromRight_FaceToUs"));
				return TargetFromRight_FaceToUs;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("TargetFromRight_BackToUs"));
				return TargetFromRight_BackToUs;
			}
		}
		else
		{
			// target on Right
			FVector targetFWD = _target->GetActorForwardVector();
			float dotHeading = FVector::DotProduct(myRight, targetFWD);

			if (dotHeading > 0)
			{
				if (dotHeading  > COS45)
				{
					// add negative effect - being flanked on group member for 
					// short time period
					UE_LOG(LogTemp, Log, TEXT("being flanked - TargetFromLeft_FaceToUs"));

				}
				UE_LOG(LogTemp, Log, TEXT("TargetFromLeft_FaceToUs"));
				return TargetFromLeft_FaceToUs;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("TargetFromLeft_BackToUs"));
				return TargetFromLeft_BackToUs;
			}
		}
	}
}

/** my column against their column, at same index order, reverse column to column*/
void AAIGroupBase::MeleeTargetSelect_TAtF_Chasing(AAIGroupBase * const _targetGroup)
{

	// check number of column for two group
	int ourColCount = this->GetMaxColoumnCount();
	int theirColCount = _targetGroup->GetMaxColoumnCount();

	// find middle column
	int ourMiddleColumn = ourColCount * 0.5f;
	int theirMiddleColumn = theirColCount * 0.5f;


	///         0 - 1 - 2 - 3 - 4         middle column = 5 / 2 = 2.5 = 2
	/// 0 - 1 - 2 - 3 - 4 - 5 - 6 - 7     middle column = 8 / 2 = 4,  maxOffsetFromMiddleRow = 4 - 0 = 4, for loop 0 to 4

	int maxOffsetTimeFromMiddle;
	if (ourColCount > theirColCount)
		maxOffsetTimeFromMiddle = UKismetMathLibrary::Max(ourMiddleColumn, ourColCount - 1 - ourMiddleColumn);
	else
		maxOffsetTimeFromMiddle = UKismetMathLibrary::Max(theirMiddleColumn, theirColCount - 1 - theirMiddleColumn);

	// Start from middle column to starting and ending column at each iteration
	for (int iOffset = 0; iOffset <= maxOffsetTimeFromMiddle; iOffset++)
	{
		int ourCurrrentColumn, theirCurrentColumn;
		if (iOffset == 0)
		{
			ourCurrrentColumn = iOffset + ourMiddleColumn;
			theirCurrentColumn = iOffset + theirMiddleColumn;
			AssignColumnToColumn(_targetGroup, ourCurrrentColumn, theirCurrentColumn, true);
		}
		else
		{
			// both go right
			ourCurrrentColumn  = iOffset + ourMiddleColumn;
			theirCurrentColumn = iOffset + theirMiddleColumn;

			if (ourCurrrentColumn < ourColCount && theirCurrentColumn < theirColCount)
			{
				// if both column are valid
				AssignColumnToColumn(_targetGroup, ourCurrrentColumn, theirCurrentColumn, true);
			}
			else
			{

				if (ourCurrrentColumn >= ourColCount)
				{
					// do nothing if our column is invalid
				}
				else
				{
					// if target group has no more column to fight us on the right, 
					// we use the rest of column to target his very right column
					if (theirCurrentColumn >= theirColCount)
						this->AssignColumnToColumn(_targetGroup, ourCurrrentColumn, theirColCount - 1);
				}

			}

			// both go left
			ourCurrrentColumn = -iOffset + ourMiddleColumn;
			theirCurrentColumn = -iOffset + theirMiddleColumn;

			if (ourCurrrentColumn >= 0 && theirCurrentColumn >= 0)
			{
				this->AssignColumnToColumn(_targetGroup, ourCurrrentColumn, theirCurrentColumn, true);
			}
			else
			{
				if (ourCurrrentColumn < 0)
				{
					// do nothing, both invalid
				}
				else 
				{
					if (theirCurrentColumn < 0)
						this->AssignColumnToColumn(_targetGroup, ourCurrrentColumn, 0, true);
				}
			}

		}
	}

}

/** my column against their column, at opposite index order*/
void AAIGroupBase::MeleeTargetSelect_TAtF_F2F(AAIGroupBase * const _targetGroup)
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
			AssignColumnToColumn(_targetGroup, ourCurrrentColumn, theirCurrentColumn);
		}
		else
		{
			ourCurrrentColumn = iOffset + ourMiddleColumn;
			theirCurrentColumn = -iOffset + theirMiddleColumn;

			if (ourCurrrentColumn < ourColCount && theirCurrentColumn >= 0)
			{
				AssignColumnToColumn(_targetGroup, ourCurrrentColumn, theirCurrentColumn);
			}
			else
			{

				if (ourCurrrentColumn >= ourColCount)
				{
					// their attack our nearest column
					//_targetGroup->AssignColumn(this, theirCurrentColumn, ourColCount - 1);
				}
				else if (theirCurrentColumn < 0)
				{
					this->AssignColumnToColumn(_targetGroup, ourCurrrentColumn, 0);

				}

			}


			ourCurrrentColumn = -iOffset + ourMiddleColumn;
			theirCurrentColumn = +iOffset + theirMiddleColumn;


			if (ourCurrrentColumn >= 0 && theirCurrentColumn < theirColCount)
			{
				this->AssignColumnToColumn(_targetGroup, ourCurrrentColumn, theirCurrentColumn);

				//PairColumn(_targetGroup, ourCurrrentColumn, theirCurrentColumn);
			}
			else
			{
				if (ourCurrrentColumn < 0) {}
				else if (theirCurrentColumn >= theirColCount)
				{
					this->AssignColumnToColumn(_targetGroup, ourCurrrentColumn, theirColCount - 1);
				}

			}

		}
	}

}

/** my column against their row, at same index order*/
void AAIGroupBase::MeleeTargetSelect_TAtF_FL(AAIGroupBase * const _targetGroup)
{
	/// my column against their row, at same index order

	int ourColCount = this->GetMaxColoumnCount();
	int theirRowCount = _targetGroup->GetMaxRowCount();

	// find middle row for us and middle column for them 
	int ourMiddleCol   = ourColCount * 0.5f;
	int theirMiddleRow = theirRowCount * 0.5f;


	int maxOffsetFromMiddle;
	if (ourColCount > theirRowCount)
		maxOffsetFromMiddle = UKismetMathLibrary::Max(ourMiddleCol, ourColCount - 1 - ourMiddleCol);
	else
		maxOffsetFromMiddle = UKismetMathLibrary::Max(theirMiddleRow, theirRowCount - 1 - theirMiddleRow);

	// Start from middle column to starting and ending column at each iteration
	for (int iOffset = 0; iOffset <= maxOffsetFromMiddle; iOffset++)
	{
		int ourCurrentCol, theirCurrentRow;
		if (iOffset == 0)
		{
			ourCurrentCol = iOffset + ourMiddleCol;
			theirCurrentRow = iOffset + theirMiddleRow;
			AssignColumnToRow(_targetGroup, ourCurrentCol, theirCurrentRow);
		}
		else
		{
			// both go right
			ourCurrentCol = iOffset + ourMiddleCol;
			theirCurrentRow = iOffset + theirMiddleRow;

			if (ourCurrentCol < ourColCount && theirCurrentRow < theirRowCount)
			{
				// if both column are valid
				AssignColumnToRow(_targetGroup, ourCurrentCol, theirCurrentRow);
			}
			else
			{

				if (ourCurrentCol >= ourColCount)
				{
					// do nothing if our column is invalid
				}
				else
				{
					// if target group has no more row to fight our column on our the right, 
					// we use the rest of column against their very last row
					if (theirCurrentRow >= theirRowCount)
						this->AssignColumnToRow(_targetGroup, ourCurrentCol, theirRowCount - 1);
				}

			}

			// both go left
			ourCurrentCol = -iOffset + ourMiddleCol;
			theirCurrentRow = -iOffset + theirMiddleRow;

			if (ourCurrentCol >= 0 && theirCurrentRow >= 0)
			{
				this->AssignColumnToRow(_targetGroup, ourCurrentCol, theirCurrentRow);
			}
			else
			{
				if (ourCurrentCol < 0)
				{
					// do nothing, both invalid
				}
				else
				{
					if (theirCurrentRow < 0)
						this->AssignColumnToRow(_targetGroup, ourCurrentCol, 0);
				}
			}

		}
	}



}

/**my column against their row, at opposite index order, reverse column to row*/
void AAIGroupBase::MeleeTargetSelect_TAtF_FR(AAIGroupBase * const _targetGroup)
{
	/// my column against their row, at opposite index order

	int ourColCount = this->GetMaxColoumnCount();
	int theirRowCount = _targetGroup->GetMaxRowCount();

	// find middle row for us and middle column for them 
	int ourMiddleCol = ourColCount * 0.5f;
	int theirMiddleRow = theirRowCount * 0.5f;


	int maxOffsetFromMiddle;
	if (ourColCount > theirRowCount)
		maxOffsetFromMiddle = UKismetMathLibrary::Max(ourMiddleCol, ourColCount - 1 - ourMiddleCol);
	else
		maxOffsetFromMiddle = UKismetMathLibrary::Max(theirMiddleRow, theirRowCount - 1 - theirMiddleRow);

	// Start from middle column to starting and ending column at each iteration
	for (int iOffset = 0; iOffset <= maxOffsetFromMiddle; iOffset++)
	{
		int ourCurrentCol, theirCurrentRow;
		if (iOffset == 0)
		{
			ourCurrentCol = ourMiddleCol;
			theirCurrentRow = theirMiddleRow;
			AssignColumnToRow(_targetGroup, ourCurrentCol, theirCurrentRow, true);
		}
		else
		{
			// ours going right, theirs going up
			ourCurrentCol = iOffset + ourMiddleCol;
			theirCurrentRow = - iOffset + theirMiddleRow;

			if (ourCurrentCol < ourColCount && theirCurrentRow >= 0)
			{
				// if both column are valid
				AssignColumnToRow(_targetGroup, ourCurrentCol, theirCurrentRow, true);
			}
			else
			{

				if (ourCurrentCol >= ourColCount)
				{
					// do nothing if our column is invalid
				}
				else
				{
					// if target group has no more row to fight our column on our the right, 
					// we use the rest of column against their very FIRST row
					if (theirCurrentRow < 0)
						this->AssignColumnToRow(_targetGroup, ourCurrentCol, 0, true);
				}

			}

			// ours going left, theirs going down
			ourCurrentCol = -iOffset + ourMiddleCol;
			theirCurrentRow = iOffset + theirMiddleRow;

			if (ourCurrentCol >= 0 && theirCurrentRow < theirRowCount)
			{
				this->AssignColumnToRow(_targetGroup, ourCurrentCol, theirCurrentRow, true);
			}
			else
			{
				if (ourCurrentCol < 0)
				{
					// do nothing, both invalid
				}
				else
				{
					if (theirCurrentRow >= theirRowCount)
						this->AssignColumnToRow(_targetGroup, ourCurrentCol, theirRowCount - 1, true);
				}
			}

		}
	}


}

/**my row against their column, at same index order*/
void AAIGroupBase::MeleeTargetSelect_TFL_Face2Us(AAIGroupBase * const _targetGroup)
{
	/// my row against their column at same index order

	///        T        c0 c1 c2 c3
	///                   0 1 2 3    r0
	///      6 3 0        4 5 6 7    r1
	///      7 4 1        8 9 0 1    r2
	///      8 5 2        2 3 4 5    r3
	///        
	
	// find our row count against their column count
	int ourRowCount = this->GetMaxRowCount();
	int theirColCount = _targetGroup->GetMaxColoumnCount();

	// find middle row for us and middle column for them 
	int ourMiddleRow = ourRowCount * 0.5f;
	int theirMiddleColumn = theirColCount * 0.5f;


	int maxOffsetFromMiddle;
	if (ourRowCount > theirColCount)
		maxOffsetFromMiddle = UKismetMathLibrary::Max(ourMiddleRow, ourRowCount - 1 - ourMiddleRow);
	else
		maxOffsetFromMiddle = UKismetMathLibrary::Max(theirMiddleColumn, theirColCount - 1 - theirMiddleColumn);

	// Start from middle column to starting and ending column at each iteration
	for (int iOffset = 0; iOffset <= maxOffsetFromMiddle; iOffset++)
	{
		int ourCurrentRow, theirCurrentColumn;
		if (iOffset == 0)
		{
			ourCurrentRow = iOffset + ourMiddleRow;
			theirCurrentColumn = iOffset + theirMiddleColumn;
			AssignRowToColumn(_targetGroup, ourCurrentRow, theirCurrentColumn);
		}
		else
		{
			// both go right
			ourCurrentRow = iOffset + ourMiddleRow;
			theirCurrentColumn = iOffset + theirMiddleColumn;

			if (ourCurrentRow < ourRowCount && theirCurrentColumn < theirColCount)
			{
				// if both column are valid
				AssignRowToColumn(_targetGroup, ourCurrentRow, theirCurrentColumn);
			}
			else
			{

				if (ourCurrentRow >= ourRowCount)
				{
					// do nothing if our column is invalid
				}
				else
				{
					// if target group has no more column to fight us on the right, 
					// we use the rest of row against their very right column
					if (theirCurrentColumn >= theirColCount)
						this->AssignRowToColumn(_targetGroup, ourCurrentRow, theirColCount - 1);
				}

			}

			// both go left
			ourCurrentRow = -iOffset + ourMiddleRow;
			theirCurrentColumn = -iOffset + theirMiddleColumn;

			if (ourCurrentRow >= 0 && theirCurrentColumn >= 0)
			{
				this->AssignRowToColumn(_targetGroup, ourCurrentRow, theirCurrentColumn);
			}
			else
			{
				if (ourCurrentRow < 0)
				{
					// do nothing, both invalid
				}
				else
				{
					if (theirCurrentColumn < 0)
						this->AssignRowToColumn(_targetGroup, ourCurrentRow, 0);
				}
			}

		}
	}

}

/**my row against their column, at opposite index order, reverse row to column*/
void AAIGroupBase::MeleeTargetSelect_TFR_Face2Us(AAIGroupBase * const _targetGroup)
{
	/// my row against their column at opposite index order, reverse row - column assignment

	///         c0 c1 c2 c3              T   
	///           0 1 2 3    r0		         
	///           4 5 6 7    r1		   2 5 8 
	///           8 9 0 1    r2		   1 4 7 
	///           2 3 4 5    r3		   0 3 6 
	///        


	// find our row count against their column count
	int ourRowCount = this->GetMaxRowCount();
	int theirColCount = _targetGroup->GetMaxColoumnCount();

	// find middle row for us and middle column for them 
	int ourMiddleRow = ourRowCount * 0.5f;
	int theirMiddleColumn = theirColCount * 0.5f;


	int maxOffsetFromMiddle;
	if (ourRowCount > theirColCount)
		maxOffsetFromMiddle = UKismetMathLibrary::Max(ourMiddleRow, ourRowCount - 1 - ourMiddleRow);
	else
		maxOffsetFromMiddle = UKismetMathLibrary::Max(theirMiddleColumn, theirColCount - 1 - theirMiddleColumn);

	// Start from middle column to starting and ending column at each iteration
	for (int iOffset = 0; iOffset <= maxOffsetFromMiddle; iOffset++)
	{
		int ourCurrentRow, theirCurrentColumn;
		if (iOffset == 0)
		{
			ourCurrentRow = ourMiddleRow;
			theirCurrentColumn = theirMiddleColumn;
			AssignRowToColumn(_targetGroup, ourCurrentRow, theirCurrentColumn, true);
		}
		else
		{
			// ours go down, their go left
			ourCurrentRow = iOffset + ourMiddleRow;
			theirCurrentColumn = -iOffset + theirMiddleColumn;

			if (ourCurrentRow < ourRowCount && theirCurrentColumn >= 0)
			{
				// if both column are valid
				AssignRowToColumn(_targetGroup, ourCurrentRow, theirCurrentColumn, true);
			}
			else
			{

				if (ourCurrentRow >= ourRowCount)
				{
					// do nothing if our column is invalid
				}
				else
				{
					// if target group has no more column to fight us on its left, 
					// we use the rest of last rows against their very left column
					if (theirCurrentColumn < 0)
						this->AssignRowToColumn(_targetGroup, ourCurrentRow, 0, true);
				}

			}

			// ours go up, their go right
			ourCurrentRow = -iOffset + ourMiddleRow;
			theirCurrentColumn = iOffset + theirMiddleColumn;

			if (ourCurrentRow >= 0 && theirCurrentColumn < theirColCount)
			{
				this->AssignRowToColumn(_targetGroup, ourCurrentRow, theirCurrentColumn, true);
			}
			else
			{
				if (ourCurrentRow < 0)
				{
					// do nothing, both invalid
				}
				else
				{
					if (theirCurrentColumn >= theirColCount)
						this->AssignRowToColumn(_targetGroup, ourCurrentRow, theirColCount - 1, true);
				}
			}

		}
	}



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

void AAIGroupBase::AssignColumnToColumn(AAIGroupBase * const _targetGroup, int _myColumn, int _theirColumn, bool _isReversed)
{
	TArray<ATheLastBastionAIBase*> ourColGroup = GetColumnAt(_myColumn);
	TArray<ATheLastBastionAIBase*> theirColGroup = _targetGroup->GetColumnAt(_theirColumn);
	int ourColSize = ourColGroup.Num();
	int theirColSize = theirColGroup.Num();

	const int theirMaxColIndex = theirColSize - 1;
	const int ourMaxColIndex = ourColSize - 1;
	int theirCurrentCol;


	if (ourColSize > theirColSize)
	{
		for (int iRow = 0; iRow < ourColSize; iRow++)
		{
			if (iRow <= theirMaxColIndex)
				theirCurrentCol = (_isReversed) ? theirMaxColIndex - iRow : iRow;
			else
				theirCurrentCol = FMath::RandRange(0, theirColSize - 1);
			ourColGroup[iRow]->SetTarget(theirColGroup[theirCurrentCol]);


		}
	}
	else
	{
		for (int iRow = 0; iRow < ourColSize; iRow++)
		{
			theirCurrentCol = (_isReversed) ? theirMaxColIndex - iRow : iRow;
			ourColGroup[iRow]->SetTarget(theirColGroup[theirCurrentCol]);
		}
	}

}

void AAIGroupBase::AssignRowToColumn(AAIGroupBase * const _targetGroup, int _myRow, int _theirColumn, bool _isReversed)
{
	TArray<ATheLastBastionAIBase*> ourRowGroup = GetRowAt(_myRow);
	TArray<ATheLastBastionAIBase*> theirColGroup = _targetGroup->GetColumnAt(_theirColumn);

	int ourRowSize = ourRowGroup.Num();
	int theirColSize = theirColGroup.Num();

	const int ourMaxRowIndex = ourRowSize - 1;
	const int theirMaxColIndex = theirColSize - 1;

	int theirCurrentRow = 0;

	if (ourRowSize > theirColSize)
	{
		for (int iCol = 0; iCol < ourRowSize; iCol++)
		{
			if (iCol <= theirMaxColIndex)
				theirCurrentRow = (_isReversed)? theirMaxColIndex - iCol:iCol;
			else
				theirCurrentRow = FMath::RandRange(0, theirColSize - 1);
			ourRowGroup[iCol]->SetTarget(theirColGroup[theirCurrentRow]);


		}
	}
	else
	{
		for (int iCol = 0; iCol < ourRowSize; iCol++)
		{
			//if (iCol <= ourRowSize - 1)
			//{
			//	theirCurrentRow = (_isReversed) ? theirMaxColIndex - iCol : iCol;
			//	ourRowGroup[iCol]->SetTarget(theirColGroup[theirCurrentRow]);
			//}
			theirCurrentRow = (_isReversed) ? theirMaxColIndex - iCol : iCol;
			ourRowGroup[iCol]->SetTarget(theirColGroup[theirCurrentRow]);
		}
	}

}

void AAIGroupBase::AssignColumnToRow(AAIGroupBase * const _targetGroup, int _myColumn, int _theirRow, bool _isReversed)
{
	TArray<ATheLastBastionAIBase*> ourColGroup = GetColumnAt(_myColumn);
	TArray<ATheLastBastionAIBase*> theirRowGroup = _targetGroup->GetRowAt(_theirRow);

	int ourColSize = ourColGroup.Num();
	int theirRowSize = theirRowGroup.Num();

	const int theirMaxRowIndex = theirRowSize - 1;
	const int ourMaxColIndex = ourColSize - 1;


	int theirCurrentCol = 0;
	if (ourColSize > theirRowSize)
	{
		for (int iRow = 0; iRow < ourColSize; iRow++)
		{
			if (iRow <= theirMaxRowIndex)
				theirCurrentCol = (_isReversed)? theirMaxRowIndex - iRow:iRow;
			else
				theirCurrentCol = FMath::RandRange(0, theirRowSize - 1);

			ourColGroup[iRow]->SetTarget(theirRowGroup[theirCurrentCol]);
		}
	}
	else
	{

		for (int iRow = 0; iRow < ourColSize; iRow++)
		{
			theirCurrentCol = (_isReversed) ? theirMaxRowIndex - iRow : iRow;
			ourColGroup[iRow]->SetTarget(theirRowGroup[theirCurrentCol]);
		}
	}

}

void AAIGroupBase::MeleeTargetSelectionOnOverlap(AAIGroupBase* _targetGroup)
{

	int targetGroupWhereAbout = CheckTargetRelativeWhereAbout(_targetGroup);

	// target selection based on Target group where about

	switch (targetGroupWhereAbout)
	{
	case TargetAtFront_Chasing:
	case TargetFromBack_BeingChased:
		MeleeTargetSelect_TAtF_Chasing(_targetGroup);
		break;
	case TargetAtFront_FlankItsLeft:
	case TargetFromBack_ToMyLeft:
		MeleeTargetSelect_TAtF_FL(_targetGroup);
		break;

	case TargetAtFront_FlankItsRight:
	case TargetFromBack_ToMyRight:
		MeleeTargetSelect_TAtF_FR(_targetGroup);
		break;

	case TargetAtFront_Face2Face:
	case TargetFromBack_Back2Back:
		MeleeTargetSelect_TAtF_F2F(_targetGroup);
		break;

	case TargetFromLeft_FaceToUs:
	case TargetFromRight_BackToUs:
		MeleeTargetSelect_TFL_Face2Us(_targetGroup);
		break;
	case TargetFromLeft_BackToUs:
	case TargetFromRight_FaceToUs:
		MeleeTargetSelect_TFR_Face2Us(_targetGroup);
		break;
	}
}
