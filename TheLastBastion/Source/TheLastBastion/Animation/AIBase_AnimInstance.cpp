// Fill out your copyright notice in the Description page of Project Settings.

#include "AIBase_AnimInstance.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Combat/PawnStatsComponent.h"


UAIBase_AnimInstance::UAIBase_AnimInstance(const FObjectInitializer& _objectInitalizer) :Super(_objectInitalizer)
{
	AttackState = EAIAttackState::None;
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

	// the more of the angle between forward vector and acceleration, the more rotation speed
	//movementComp->RotationRate.Yaw
	//	= UKismetMathLibrary::MapRangeClamped(turn, 1.0f, -1.0f,
	//		mCharacter->GetMinTurnRateForTravel(),
	//		mCharacter->GetMaxTurnRateForTravel());

}

void UAIBase_AnimInstance::OnPostEvaluate()
{
}

void UAIBase_AnimInstance::OnEnableWeapon(bool bIsright, bool bIsAll)
{
	mCharacter->GetEnemyStatsComponent()->EnableWeapon(bIsright, bIsAll);
}

void UAIBase_AnimInstance::OnDisableWeapon(bool bIsright, bool bIsAll)
{
	mCharacter->GetEnemyStatsComponent()->DisableWeapon(bIsright, bIsAll);

}

void UAIBase_AnimInstance::OnAttack()
{
	AttackState = EAIAttackState::Attack;
}

void UAIBase_AnimInstance::FinishAttack()
{
	AttackState = EAIAttackState::None;
}
