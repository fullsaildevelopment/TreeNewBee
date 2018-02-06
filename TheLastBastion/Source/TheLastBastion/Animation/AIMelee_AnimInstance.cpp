// Fill out your copyright notice in the Description page of Project Settings.

#include "AIMelee_AnimInstance.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "Combat/PawnStatsComponent.h"




void UAIMelee_AnimInstance::OnBeginPlay()
{
	Super::OnBeginPlay();
	attackChoice = EAIMeleeAttackType::None;
	//UE_LOG(LogTemp, Log, TEXT("UAIMelee_AnimInstance::OnBeginPlay is Called"));

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
	case EAIActionState::GettingHurt:
		SyncMotionForGettingHurt();
		break;
	case EAIActionState::None:
		
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
		if (CurrentActionState != EAIActionState::GettingHurt)
		{
			mCharacter->GetEnemyStatsComponent()->SetEnableWeapon(true, bIsright, bIsAll);

		}
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
			ATheLastBastionBaseAIController* baseAICtrl = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
			// recover the rotation rate from melee attack motion sync
			mCharacter->GetCharacterMovement()->RotationRate.Yaw = 540.0f;
			if (baseAICtrl)
			{
				baseAICtrl->SetAIActionState_BBC(CurrentActionState);
				UBehaviorTreeComponent* btc = baseAICtrl->GetBTComp();
				if (btc)
				{


					OnRecoverFromHitSignature.ExecuteIfBound(btc);
					//OnRecoverFromHitSignature.Unbind();
				}
			}
		}
	}
}


void UAIMelee_AnimInstance::OnBeingHit(FName boneName, const FVector & _shotFromDirection, const FVector & _hitLocation)
{

	if (Hit_Montage == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Hit Montage is nullptr"));
		return;
	}

	FinishAttack();
	CurrentActionState = EAIActionState::GettingHurt;
	//UE_LOG(LogTemp, Log, TEXT("disable weapon on hit"));
	OnDisableWeapon(false, true);


	ATheLastBastionBaseAIController* enemyC = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
	if (enemyC == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("enemyC is nullptr - UAIRange_AnimInstance"));
		return;
	}

	enemyC->OnBeingHit(mCharacter->GetCharacterType());

	FName sectionToPlay;
	ECharacterType Type = mCharacter->GetCharacterType();
	switch (Type)
	{
	case ECharacterType::LanTrooper_T0:
		sectionToPlay = HitReaction_SHSword(boneName, _shotFromDirection, _hitLocation);
		break;
	default:
		break;
	}

	PlayMontage(Hit_Montage, 1.0f, sectionToPlay);
}


