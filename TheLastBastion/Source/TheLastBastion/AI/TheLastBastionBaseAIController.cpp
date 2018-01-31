// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionBaseAIController.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "Animation/AIBase_AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Kismet/GameplayStatics.h"


const float initDistance = 999999999.0f;
ATheLastBastionBaseAIController::ATheLastBastionBaseAIController(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	mBTComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
	mBBComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComp"));
	
}

void ATheLastBastionBaseAIController::Possess(APawn* _possPawn)
{
	//UE_LOG(LogTemp, Warning, TEXT("ATheLastBastionBaseAIController possess on %s"), *_possPawn->GetName());

	Super::Possess(_possPawn);
	mCharacter = Cast<ATheLastBastionEnemyCharacter>(_possPawn);
	if (mCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ATheLastBastionBaseAIController possess non - ATheLastBastionEnemyCharacter type, %s"), *_possPawn->GetName());
		return;
	}

	mAnimInstanceRef = Cast<UAIBase_AnimInstance>(mCharacter->GetMesh()->GetAnimInstance());
	if (mAnimInstanceRef == nullptr)
	{
		UE_LOG(LogTemp, Error,
			TEXT("mAnimInstanceRef is NUll -- ATheLastBastionBaseAIController::BeginPlay"));
		return;
	}


	UBehaviorTree* const bt = mCharacter->GetBehaviorTree();
	if (bt == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("This ATheLastBastionEnemyCharacter should be assigned with a behavior Tree, %s"), *_possPawn->GetName());
		return;
	}


	UBlackboardData* const bbData = bt->BlackboardAsset;
	if (bbData == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("This behavior Tree should be assigned with a blackborad, %s"), *_possPawn->GetName());
		return;
	}

	mBBComp->InitializeBlackboard(*bbData);
	Blackboard = mBBComp;

	// Fetch the Keys
	targetActor_KeyID = mBBComp->GetKeyID("targetActor");
	targetLocation_KeyID = mBBComp->GetKeyID("targetLocation");
	ToTargetActorDistanceSqr_KeyId = mBBComp->GetKeyID("ToTargetActorDistanceSqr");
	CurrentActionState_KeyID = mBBComp->GetKeyID("CurrentActionState");

	mBBComp->SetValue<UBlackboardKeyType_Float>(ToTargetActorDistanceSqr_KeyId, MAX_FLT);
	mBBComp->SetValue<UBlackboardKeyType_Vector>(targetLocation_KeyID, _possPawn->GetActorLocation());
	mBBComp->SetValue<UBlackboardKeyType_Enum>(CurrentActionState_KeyID, static_cast<UBlackboardKeyType_Enum::FDataType>(EAIActionState::None));
	//UE_LOG(LogTemp, Warning, TEXT("Possess, %s"), *_possPawn->GetName());

	// Launch behavior Tree
	mBTComp->StartTree(*bt);


}

void ATheLastBastionBaseAIController::BeginPlay()
{
	Super::BeginPlay();


	// Init BlackBoard Value, set player is my target for now
	// if (bSetInitTargetToHost)
	//	mBBComp->SetValue<UBlackboardKeyType_Object>(targetActor_KeyID,
	//		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	//if (mCharacter)
	//{
	//	mAnimInstanceRef = Cast<UAIBase_AnimInstance>(mCharacter->GetMesh()->GetAnimInstance());
	//	if (mAnimInstanceRef == nullptr)
	//	{
	//		UE_LOG(LogTemp, Error, 
	//			TEXT("mAnimInstanceRef is NUll -- ATheLastBastionBaseAIController::BeginPlay"));
	//		return;
	//	}
	//}
}

void ATheLastBastionBaseAIController::OnBeingHit(ECharacterType _characterType)
{

	switch (_characterType)
	{
	case ECharacterType::LanCB_T0:
	default:
		mBBComp->SetValue<UBlackboardKeyType_Enum>(CurrentActionState_KeyID, 
			static_cast<UBlackboardKeyType_Enum::FDataType>(EAIActionState::GettingHurt));
		break;
	}
}
