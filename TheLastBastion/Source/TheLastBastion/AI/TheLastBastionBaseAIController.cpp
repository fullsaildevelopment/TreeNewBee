// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionBaseAIController.h"
#include "AICharacters/TheLastBastionAIBase.h"
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
	mCharacter = Cast<ATheLastBastionAIBase>(_possPawn);
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
	NewCommandIndex_KeyID = mBBComp->GetKeyID("NewCommandIndex");
	OldCommandIndex_KeyID = mBBComp->GetKeyID("OldCommandIndex");

	mBBComp->SetValue<UBlackboardKeyType_Float>(ToTargetActorDistanceSqr_KeyId, MAX_FLT);
	mBBComp->SetValue<UBlackboardKeyType_Vector>(targetLocation_KeyID, _possPawn->GetActorLocation());
	mBBComp->SetValue<UBlackboardKeyType_Int>(NewCommandIndex_KeyID, 0);
	mBBComp->SetValue<UBlackboardKeyType_Int>(OldCommandIndex_KeyID, 0);
	mBBComp->SetValue<UBlackboardKeyType_Enum>(CurrentActionState_KeyID, static_cast<UBlackboardKeyType_Enum::FDataType>(EAIActionState::None));
	//UE_LOG(LogTemp, Warning, TEXT("Possess, %s"), *_possPawn->GetName());
	
	// Launch behavior Tree
	mBTComp->StartTree(*bt);


}

void ATheLastBastionBaseAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ATheLastBastionBaseAIController::OnBeingHit(ECharacterType _characterType)
{

	switch (_characterType)
	{
	case ECharacterType::LanTrooper_T0:
	case ECharacterType::LanCB_T0:
	default:
		SetAICurrentActionState_BBC(EAIActionState::GettingHurt);
		break;
	}
}

void ATheLastBastionBaseAIController::SetTargetActor_BBC(AActor* _target)
{
	Blackboard->SetValue<UBlackboardKeyType_Object>(targetActor_KeyID, _target);
}

void ATheLastBastionBaseAIController::SetTargetLocation_BBC(const FVector& _targetLocation) 
{
	Blackboard->SetValue<UBlackboardKeyType_Vector>(targetLocation_KeyID, _targetLocation);
}

void ATheLastBastionBaseAIController::SetAICurrentActionState_BBC(EAIActionState _aiState)
{
	Blackboard->SetValue<UBlackboardKeyType_Enum>(GetKeyID_CurrentActionState(),
		static_cast<UBlackboardKeyType_Enum::FDataType>(_aiState));
}

void ATheLastBastionBaseAIController::SetNewCommandIndex_BBC(int _newCommand)
{
	Blackboard->SetValue<UBlackboardKeyType_Int>(NewCommandIndex_KeyID, _newCommand);
}

void ATheLastBastionBaseAIController::SetOldCommandIndex_BBC(int _oldCommand)
{
	Blackboard->SetValue<UBlackboardKeyType_Int>(OldCommandIndex_KeyID, _oldCommand);
}

void ATheLastBastionBaseAIController::SetToTargetLocationDistanceSqr(float _disTanceSqr)
{
	Blackboard->SetValue<UBlackboardKeyType_Float>(ToTargetLocationDistanceSqr_KeyId, _disTanceSqr);
}

void ATheLastBastionBaseAIController::SetToTargetActorDistanceSqr(float _disTanceSqr)
{
	Blackboard->SetValue<UBlackboardKeyType_Float>(ToTargetActorDistanceSqr_KeyId, _disTanceSqr);
}

AActor * ATheLastBastionBaseAIController::GetTargetActor_BBC() const
{
	AActor* targetActor = Cast<AActor>(Blackboard->GetValue<UBlackboardKeyType_Object>(targetActor_KeyID));
	return targetActor;
}

FVector ATheLastBastionBaseAIController::GetTargetLocation_BBC() const
{
	return Blackboard->GetValue<UBlackboardKeyType_Vector>(targetLocation_KeyID);
}

EAIActionState ATheLastBastionBaseAIController::GetAICurrentActionState_BBC() const
{
	return static_cast<EAIActionState>(Blackboard->GetValue<UBlackboardKeyType_Enum>(GetKeyID_CurrentActionState()));
}

int ATheLastBastionBaseAIController::GetNewCommandIndex_BBC() const
{
	return Blackboard->GetValue<UBlackboardKeyType_Int>(NewCommandIndex_KeyID);
}

int ATheLastBastionBaseAIController::GetOldCommandIndex_BBC() const
{
	return Blackboard->GetValue<UBlackboardKeyType_Int>(OldCommandIndex_KeyID);

}

float ATheLastBastionBaseAIController::GetToTargetLocationDistanceSqr() const
{
	return Blackboard->GetValue < UBlackboardKeyType_Float > (ToTargetLocationDistanceSqr_KeyId);
}

float ATheLastBastionBaseAIController::GetToTargetActorDistanceSqr() const
{
	return Blackboard->GetValue < UBlackboardKeyType_Float >(ToTargetActorDistanceSqr_KeyId);

}
