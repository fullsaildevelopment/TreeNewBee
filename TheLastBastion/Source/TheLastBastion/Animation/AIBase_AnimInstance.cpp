// Fill out your copyright notice in the Description page of Project Settings.

#include "AIBase_AnimInstance.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "Kismet/KismetMathLibrary.h"



UAIBase_AnimInstance::UAIBase_AnimInstance(const FObjectInitializer& _objectInitalizer) :Super(_objectInitalizer)
{
	CurrentActionState = EAIActionState::None;
	MomentumRatioByActor = 0.7f;
}

void UAIBase_AnimInstance::OnBeginPlay()
{
	mCharacter = Cast<ATheLastBastionEnemyCharacter>(TryGetPawnOwner());
	if (mCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("The AIBase_AnimInstance can only assigned to ATheLastBastionEnemyCharacter - UAIBase_AnimInstance "));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("AIBase_AnimInstance Call OnBeginPlay"));
}

void UAIBase_AnimInstance::OnInit()
{

}

void UAIBase_AnimInstance::OnUpdate(float _deltaTime)
{
	if (mCharacter == nullptr)
		return;

	UCharacterMovementComponent* movementComp = mCharacter->GetCharacterMovement();
	currentSpeed = movementComp->Velocity.SizeSquared();	

}

void UAIBase_AnimInstance::OnPostEvaluate()
{
}

void UAIBase_AnimInstance::Attack(EAIMeleeAttackType _attackType)
{
}

void UAIBase_AnimInstance::Fire()
{
}

void UAIBase_AnimInstance::FinishAttack()
{
}



