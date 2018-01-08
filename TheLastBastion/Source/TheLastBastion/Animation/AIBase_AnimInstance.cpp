// Fill out your copyright notice in the Description page of Project Settings.

#include "AIBase_AnimInstance.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "Kismet/KismetMathLibrary.h"


#define HEADBONE TEXT("neck_01")
#define SH_HitReaction_HEAD_RIGHT      TEXT("Head_R")
#define SH_HitReaction_HEAD_LEFT       TEXT("Head_L")
#define SH_HitReaction_HEAD_FRONT      TEXT("Head_F")
#define SH_HitReaction_BODY_RIGHT      TEXT("Body_R")
#define SH_HitReaction_BODY_LEFT       TEXT("Body_L")
#define SH_HitReaction_BODY_FRONT      TEXT("Body_F")

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

void UAIBase_AnimInstance::OnBeingHit(float _damage, FName boneName, const FVector & _shotFromDirection, const UPawnStatsComponent * _pawnStats)
{
	
	FinishAttack();

	if (Hit_Montage == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Hit Montage is nullptr"));
		return;
	}

	FName sectionToPlay;
	ECharacterType Type = mCharacter->GetCharacterType();
	switch (Type)
	{
	case ECharacterType::LanTrooper_T0:
		sectionToPlay = HitReaction_SHSword(boneName, _shotFromDirection);
		break;
	default:
		break;
	}

	PlayMontage(Hit_Montage, 1.0f, sectionToPlay);
}

FName UAIBase_AnimInstance::HitReaction_SHSword(FName boneName, const FVector & _shotFromDirection)
{
	FName sectionName;
	FVector away = _shotFromDirection.GetSafeNormal();
	UE_LOG(LogTemp, Warning, TEXT("%s"), *boneName.ToString());

	float vert = FVector::DotProduct(mCharacter->GetActorForwardVector(), away);
	if (boneName.Compare(HEADBONE) == 0)
	{
		if (vert > 0.9f)
			sectionName = SH_HitReaction_HEAD_FRONT;
		else
		{
			float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), away);
			if (hor > 0)
				sectionName = SH_HitReaction_HEAD_RIGHT;
			else
				sectionName = SH_HitReaction_HEAD_LEFT;
		}
	}
	else
	{
		if (vert > 0.9f)
			sectionName = SH_HitReaction_BODY_FRONT;
		else
		{
			float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), away);
			if (hor > 0)
				sectionName = SH_HitReaction_BODY_RIGHT;
			else
				sectionName = SH_HitReaction_BODY_LEFT;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *sectionName.ToString());

	return sectionName;
}

