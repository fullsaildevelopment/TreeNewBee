// Fill out your copyright notice in the Description page of Project Settings.

#include "AIBase_AnimInstance.h"
#include "AICharacters/TheLastBastionAIBase.h"
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
	mCharacter = Cast<ATheLastBastionAIBase>(TryGetPawnOwner());
	if (mCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("The AIBase_AnimInstance can only assigned to ATheLastBastionEnemyCharacter - UAIBase_AnimInstance "));
		return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("AIBase_AnimInstance Call OnBeginPlay"));
}

void UAIBase_AnimInstance::OnInit()
{

}

void UAIBase_AnimInstance::OnUpdate(float _deltaTime)
{
	if (mCharacter == nullptr)
		return;

	UCharacterMovementComponent* movementComp = mCharacter->GetCharacterMovement();
	FVector velocity = movementComp->Velocity;
	currentSpeed = velocity.SizeSquared();
	FVector velocityLocalDir = UKismetMathLibrary::InverseTransformDirection(mCharacter->GetTransform(), velocity);
	velocityLocalDir = FVector(velocityLocalDir.X, velocityLocalDir.Y, 0).GetSafeNormal();
	MoveForwardAxis = velocityLocalDir.X;
	MoveRightAxis = velocityLocalDir.Y;
}

void UAIBase_AnimInstance::OnPostEvaluate()
{
}

void UAIBase_AnimInstance::Attack(EAIMeleeAttackType _attackType)
{
}

void UAIBase_AnimInstance::Fire(const AActor * _target)
{
}

void UAIBase_AnimInstance::StopFire()
{
}

void UAIBase_AnimInstance::FinishAttack()
{
}

void UAIBase_AnimInstance::SyncMotionForMeleeAttack()
{


	UCharacterMovementComponent* movementComp = mCharacter->GetCharacterMovement();

	// Sync Velocity
	float speed = GetCurveValue("Speed");

	FVector Velocity = movementComp->Velocity;
	movementComp->Velocity = mCharacter->GetActorForwardVector() * speed;
	movementComp->Velocity.Z = Velocity.Z;

	// Sync Rotation rate, the faster move the slower it rotates
	// float rotateRate = UKismetMathLibrary::MapRangeClamped(speed, 200.0f, 600.0f,360.0f, 90.0f);

	movementComp->RotationRate.Yaw = GetCurveValue("RotationRate");


}

void UAIBase_AnimInstance::SyncMotionForGettingHurt()
{
	UCharacterMovementComponent* movementComp = mCharacter->GetCharacterMovement();

	// Sync Velocity
	float speed = GetCurveValue("Speed");

	FVector Velocity = movementComp->Velocity;
	movementComp->Velocity = damageMomentum * speed;
	movementComp->Velocity.Z = Velocity.Z;

}

void UAIBase_AnimInstance::SyncMotionForNone()
{

}

FName UAIBase_AnimInstance::HitReaction_SHSword(FName boneName, const FVector & _shotFromDirection, const FVector & _hitLocation)
{
	// assume always face to attacker
	FName sectionName;

	// relative position of damage causer
	FVector damageCauserRelative = _shotFromDirection;
	damageCauserRelative.Z = 0.0f;
	damageCauserRelative = damageCauserRelative.GetUnsafeNormal();


	FVector hitRelative = _hitLocation - GetSkelMeshComponent()->GetSocketLocation(boneName);
	float hitZOffset = hitRelative.Z;
	hitRelative.Z = 0;
	hitRelative = hitRelative.GetSafeNormal();

	float vert = FVector::DotProduct(mCharacter->GetActorForwardVector(), hitRelative);

	if (boneName.Compare(HEADBONE) == 0)
	{
		/// *** Head Hit
		if (vert > 0.7f)
			sectionName = SH_HitReaction_HEAD_FRONT;
		else
		{
			float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), hitRelative);
			if (hor > 0)
				sectionName = SH_HitReaction_HEAD_RIGHT;
			else
				sectionName = SH_HitReaction_HEAD_LEFT;
		}
	}
	else
	{

		if (boneName.Compare(RIGHTLEGBONE) == 0 || boneName.Compare(LEFTLEGBONE) == 0)
		{
			float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), hitRelative);
			if (hor > 0)
				sectionName = SH_HitReaction_LEG_RIGHT;
			else
				sectionName = SH_HitReaction_LEG_LEFT;
		}

		else
		{
			if (vert > 0.7f)
				sectionName = SH_HitReaction_BODY_FRONT;
			else
			{
				float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), hitRelative);
				if (hor > 0)
					sectionName = SH_HitReaction_BODY_RIGHT;
				else
					sectionName = SH_HitReaction_BODY_LEFT;
			}
		}
	}


	damageMomentum = -MomentumRatioByActor * damageCauserRelative - (1 - MomentumRatioByActor) * hitRelative;
	damageMomentum = damageMomentum.GetUnsafeNormal();

	return sectionName;
}
