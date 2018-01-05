// Fill out your copyright notice in the Description page of Project Settings.

#include "AIBase_AnimInstance.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Combat/PawnStatsComponent.h"


UAIBase_AnimInstance::UAIBase_AnimInstance(const FObjectInitializer& _objectInitalizer) :Super(_objectInitalizer)
{
	CurrentActionState = EAIActionState::None;
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

	currentSpeed = movementComp->Velocity.Size();
	
	mAccelerationDirection = movementComp->GetCurrentAcceleration();
	mAccelerationDirection.Normalize();

	turn = FVector::DotProduct(mCharacter->GetActorForwardVector(), mAccelerationDirection);

}

void UAIBase_AnimInstance::OnPostEvaluate()
{
}

void UAIBase_AnimInstance::OnEnableWeapon(bool bIsright, bool bIsAll)
{
	mCharacter->GetEnemyStatsComponent()->SetEnableWeapon(true, bIsright, bIsAll);
}

void UAIBase_AnimInstance::OnDisableWeapon(bool bIsright, bool bIsAll)
{
	mCharacter->GetEnemyStatsComponent()->SetEnableWeapon(false, bIsright, bIsAll);
}

void UAIBase_AnimInstance::Attack()
{
	CurrentActionState = EAIActionState::Attack;
}

void UAIBase_AnimInstance::FinishAttack()
{
	CurrentActionState = EAIActionState::None;
	// Tell BT that this attack is done
	if (mCharacter)
	{
		ATheLastBastionBaseAIController* enemyC = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
		if (enemyC)
		{
			UBehaviorTreeComponent* btc = enemyC->GetBTComp();
			if (btc)
			{
				OnFinishAttackDelegate.ExecuteIfBound(btc);
				OnFinishAttackDelegate.Unbind();
			}
		}
	}
}
