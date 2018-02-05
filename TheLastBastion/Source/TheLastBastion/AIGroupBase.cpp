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
	UE_LOG(LogTemp, Warning, TEXT("overrlap with %s"), *OtherActor->GetName())

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
	ATheLastBastionGroupAIController* groupC = Cast<ATheLastBastionGroupAIController>(GetController());
	if (groupC == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("groupC == nullptr - AAIGroupBase::SetMarchLocation"));
		return;
	}

	UBlackboardComponent* bbcGroup = groupC->GetBlackboardComponent();
	if (bbcGroup == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("bbc == nullptr - AAIGroupBase::SetMarchLocation"));
		return;
	}

	// Set new command index to _commandIndex
	// Set Target location

	FVector targetFwd, targetRight;// , targetLocation;


	/// command pre-execute
	// calculate the desired forward and right vector for child location padding
	switch (_commandIndex)
	{
	case GC_GOTOLOCATION:
	{
		targetFwd = _targetLocation - GetActorLocation();
		FVector2D targetFwd2D = FVector2D(targetFwd.X, targetFwd.Y).GetSafeNormal();
		targetFwd = FVector(targetFwd2D.X, targetFwd2D.Y, 0);
		targetRight = FVector(-targetFwd2D.Y, targetFwd2D.X, 0);
		break;
	}
	case GC_HOLDLOCATION:
	case GC_DISTRIBUTE:
	default:
		break;
	case GC_FORWARD:
	{
		targetFwd = this->GetActorForwardVector();
		targetRight = this->GetActorRightVector();
		break;
	}
	}

	// check for going backward
	float dir = FVector::DotProduct(GetActorForwardVector(), targetFwd);

	// swap group offset if moving backward
	if (dir < 0)
	{
		SwapChildenOrder();
	}

	/// Command post - execure
	// Immediate Change the rotation or reformat the group relative offset information based on command
	switch (_commandIndex)
	{
	case GC_GOTOLOCATION:
	default:
	{
		this->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(_targetLocation, _targetLocation + 10 * targetFwd));
		break;
	}
	case GC_HOLDLOCATION:
	case GC_FORWARD:
	case GC_DISTRIBUTE:
	{
		break;
	}
	}

	bbcGroup->SetValue<UBlackboardKeyType_Vector>(groupC->GetKeyID_TargetLocation(), _targetLocation);
	bbcGroup->SetValue<UBlackboardKeyType_Vector>(groupC->GetKeyID_TargetForward(), targetFwd);
	bbcGroup->SetValue<UBlackboardKeyType_Vector>(groupC->GetKeyID_TargetRight(), targetRight);

	// give group march command
	bbcGroup->SetValue<UBlackboardKeyType_Int>(groupC->GetKeyID_NewCommandIndex(), _commandIndex);

	// give each child an march command
	ATheLastBastionBaseAIController* baseAICtrl = nullptr;
	UBlackboardComponent* bbcChild = nullptr;
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

			bbcChild = baseAICtrl->GetBlackboardComponent();
			if (bbcChild == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("bbc == nullptr - AAIGroupBase::SetMarchLocation"));
				continue;
			}

			bbcChild->SetValue<UBlackboardKeyType_Int>(baseAICtrl->GetKeyID_NewCommandIndex(), _commandIndex);
		}
	}
}

void AAIGroupBase::OnChildDeath(int _childIndex)
{

}

void AAIGroupBase::SwapChildenOrder()
{

	int swapTimes = 0;
	int totalAmount = 0;
	for (int iClass = 0; iClass < AIToSpawn.Num(); iClass++)
	{
		totalAmount += AIToSpawn[iClass].TotalNumber;
	}
	swapTimes = totalAmount * 0.5f;
	//FVector offsetTemp;
	ATheLastBastionAIBase* AITemp = nullptr;

	int indexToSwap = 0;
	for (int iSwap = 0; iSwap < swapTimes; iSwap++)
	{
		indexToSwap = totalAmount - 1 - iSwap;
		AITemp = AICharactersInfo[iSwap].AICharacter;
		AICharactersInfo[iSwap].AICharacter = AICharactersInfo[indexToSwap].AICharacter;
		AICharactersInfo[indexToSwap].AICharacter = AITemp;
		AICharactersInfo[iSwap].AICharacter->SetParent(this, iSwap);
		AICharactersInfo[indexToSwap].AICharacter->SetParent(this, indexToSwap);
		//offsetTemp = AICharactersInfo[iSwap].GroupRelativeOffset;
		//AICharactersInfo[iSwap].GroupRelativeOffset = AICharactersInfo[indexToSwap].GroupRelativeOffset;
		//AICharactersInfo[indexToSwap].GroupRelativeOffset = offsetTemp;
	}

	//int iClassToSwap = 0;
	//int timeToSwapForClass = AIToSpawn.Num() * 0.5f;
	//FAISpawnInfo AISpawnTemp;
	//for (int iClass = 0; iClass < timeToSwapForClass; iClass++)
	//{
	//	iClassToSwap = AIToSpawn.Num() - 1 - iClass;
	//	AISpawnTemp = AIToSpawn[iClass];
	//	AIToSpawn[iClass] = AIToSpawn[iClassToSwap];
	//	AIToSpawn[iClassToSwap] = AISpawnTemp;
	//	int iChildToSwap = 0;
	//	int timeToSwapForClass
	//}
}
