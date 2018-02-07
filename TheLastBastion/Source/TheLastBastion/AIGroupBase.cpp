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

	Hero = Cast<ATheLastBastionHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AAIGroupBase::SpawnChildGroup()
{

}

void AAIGroupBase::OnReform()
{
}

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

void AAIGroupBase::SetMarchLocation(const FVector & _targetLocation, int _commandIndex)
{

}

void AAIGroupBase::OnChildDeath(int _childIndex)
{

}

int AAIGroupBase::GetMaxColoumnCount() const
{
	return 0;
}

TArray<class ATheLastBastionAIBase*> AAIGroupBase::GetFrontLine() const
{
	TArray<ATheLastBastionAIBase*> out;
	int outSize = 0;
	if (FormationInfo.IsValidIndex(0))
		outSize = FormationInfo[0];

	out.SetNum(outSize);

	for (int iCharacter = 0; iCharacter < outSize; iCharacter++)
	{
		out[iCharacter] = AICharactersInfo[iCharacter].AICharacter;
	}
	return out;
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

void AAIGroupBase::SwapChildenOrder()
{

}

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
