// Fill out your copyright notice in the Description page of Project Settings.

#include "AIMelee_AnimInstance.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "Combat/PawnStatsComponent.h"

#define HEADBONE TEXT("neck_01")
#define SH_HitReaction_HEAD_RIGHT      TEXT("Head_R")
#define SH_HitReaction_HEAD_LEFT       TEXT("Head_L")
#define SH_HitReaction_HEAD_FRONT      TEXT("Head_F")
#define SH_HitReaction_BODY_RIGHT      TEXT("Body_R")
#define SH_HitReaction_BODY_LEFT       TEXT("Body_L")
#define SH_HitReaction_BODY_FRONT      TEXT("Body_F")



void UAIMelee_AnimInstance::OnBeginPlay()
{
	Super::OnBeginPlay();
	attackChoice = EAIMeleeAttackType::None;
	UE_LOG(LogTemp, Log, TEXT("UAIMelee_AnimInstance::OnBeginPlay is Called"));

	OnMontageStarted.AddDynamic(this, &UAIMelee_AnimInstance::OnMontageStartHandle);
	OnMontageBlendingOut.AddDynamic(this, &UAIMelee_AnimInstance::OnMontageBlendOutStartHandle);

}

void UAIMelee_AnimInstance::OnInit()
{
	Super::OnInit();
}

void UAIMelee_AnimInstance::OnUpdate(float _deltaTime)
{
	Super::OnUpdate(_deltaTime);

	if (mCharacter == nullptr)
		return;
	switch (CurrentActionState)
	{
	case EAIActionState::MeleeAttack:
		SyncMotionForMeleeAttack();
		break;
	default:
		break;
	}
}

void UAIMelee_AnimInstance::OnPostEvaluate()
{
	Super::OnPostEvaluate();
}

void UAIMelee_AnimInstance::OnEnableWeapon(bool bIsright, bool bIsAll)
{
	if (mCharacter)
	{
		//UE_LOG(LogTemp, Log, TEXT("enable weapon"));
		mCharacter->GetEnemyStatsComponent()->SetEnableWeapon(true, bIsright, bIsAll);
	}
}

void UAIMelee_AnimInstance::OnDisableWeapon(bool bIsright, bool bIsAll)
{
	if (mCharacter)
	{
		mCharacter->GetEnemyStatsComponent()->SetEnableWeapon(false, bIsright, bIsAll);
	}

}

void UAIMelee_AnimInstance::Attack(EAIMeleeAttackType _attackType)
{
	NextAction = EAIActionState::MeleeAttack;
	CurrentActionState = NextAction;
	attackChoice = _attackType;
}

void UAIMelee_AnimInstance::FinishAttack()
{
	CurrentActionState = EAIActionState::None;
	// Tell BT that this attack is done
	if (mCharacter)
	{
		ATheLastBastionBaseAIController* enemyC = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
		// recover the rotation rate from melee attack motion sync
		mCharacter->GetCharacterMovement()->RotationRate.Yaw = 540.0f;
		if (enemyC)
		{
			UBehaviorTreeComponent* btc = enemyC->GetBTComp();
			if (btc)
			{
				OnFinishAttackDelegate.ExecuteIfBound(btc);
				//OnFinishAttackDelegate.Unbind();
			}
		}
	}
}

void UAIMelee_AnimInstance::InitAttack()
{
	NextAction = EAIActionState::None;
}

void UAIMelee_AnimInstance::OnMontageStartHandle(UAnimMontage * _animMontage)
{
	if (_animMontage == Hit_Montage)
	{
		UE_LOG(LogTemp, Log, TEXT("I am being hit - OnMontageStartHandle"));
	}
}

void UAIMelee_AnimInstance::OnMontageBlendOutStartHandle(UAnimMontage * _animMontage, bool _bInterruptted)
{
	if (_animMontage == Hit_Montage && !_bInterruptted)
	{
		UE_LOG(LogTemp, Log, TEXT("I am recover from being hit - OnMontageBlendOutStartHandle"));
		CurrentActionState = EAIActionState::None;
		// Tell BT that this attack is done
		if (mCharacter)
		{
			ATheLastBastionBaseAIController* enemyC = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
			// recover the rotation rate from melee attack motion sync
			mCharacter->GetCharacterMovement()->RotationRate.Yaw = 540.0f;
			if (enemyC)
			{
				UBehaviorTreeComponent* btc = enemyC->GetBTComp();
				if (btc)
				{
					OnRecoverFromHitSignature.ExecuteIfBound(btc);
					//OnRecoverFromHitSignature.Unbind();
				}
			}
		}
	}
}

void UAIMelee_AnimInstance::SyncMotionForMeleeAttack()
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

void UAIMelee_AnimInstance::OnBeingHit(float _damage, FName boneName, const FVector & _shotFromDirection, const UPawnStatsComponent * _pawnStats)
{
	
	FinishAttack();
	CurrentActionState = EAIActionState::GettingHurt;



	//UE_LOG(LogTemp, Log, TEXT("disable weapon on hit"));
	OnDisableWeapon(false, true);
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

FName UAIMelee_AnimInstance::HitReaction_SHSword(FName boneName, const FVector & _shotFromDirection)
{
	FName sectionName;
	FVector away = _shotFromDirection.GetSafeNormal();
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *boneName.ToString());

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
