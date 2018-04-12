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

	//OnMontageStarted.AddDynamic(this, &UAIMelee_AnimInstance::OnMontageStartHandle);
	//OnMontageBlendingOut.AddDynamic(this, &UAIMelee_AnimInstance::OnMontageBlendOutStartHandle);
}

void UAIMelee_AnimInstance::OnInit()
{
	Super::OnInit();
}

void UAIMelee_AnimInstance::OnUpdate(float _deltaTime)
{
	Super::OnUpdate(_deltaTime);

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
		if (CurrentActionState != EAIActionState::GettingHurt && !mCharacter->GetIsDead())
		{
			CurrentActionState = EAIActionState::MeleeAttack;
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
	//NextAction = EAIActionState::MeleeAttack;
	NextAction = EAIActionState::MeleePreAttack;
	CurrentActionState = NextAction;
	attackChoice = _attackType;
}

void UAIMelee_AnimInstance::FinishAttack()
{
	CurrentActionState = EAIActionState::None;
	// Tell BT that this attack is done
	if (mCharacter)
	{
		ATheLastBastionBaseAIController* baseAICtrl = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
		if (baseAICtrl)
		{
			UBehaviorTreeComponent* btc = baseAICtrl->GetBTComp();
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

void UAIMelee_AnimInstance::AttackRecover()
{
	CurrentActionState = EAIActionState::MeleePostAttack;


	// recover the rotation rate from melee attack motion sync
	mCharacter->GetCharacterMovement()->RotationRate.Yaw = 540.0f;

	// rotate to target direction
	ATheLastBastionBaseAIController* baseAICtrl = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
	if (baseAICtrl)
	{
		AActor* targetActor = baseAICtrl->GetTargetActor_BBC();
		if (targetActor)
		{
			FVector tarDir = (targetActor->GetActorLocation() - mCharacter->GetActorLocation()).GetUnsafeNormal();
			FVector forward = mCharacter->GetActorForwardVector();
			FVector right = mCharacter->GetActorRightVector();
			turn = 1.0f - FVector::DotProduct(forward, tarDir);
			if (turn <= 0.07f)
			{
				turn = 0;
			}
			else
			{
				float dotRight = FVector::DotProduct(right, tarDir);
				UE_LOG(LogTemp, Log, TEXT("dotRight: %f - FinishAttack"), dotRight);

				bool isRight = dotRight > 0.0f;
				if (isRight)
					turn = 1;
				else
					turn = -1;
			}
		}

	}
}

//void UAIMelee_AnimInstance::OnMontageStartHandle(UAnimMontage * _animMontage)
//{
//}
//
//void UAIMelee_AnimInstance::OnMontageBlendOutStartHandle(UAnimMontage * _animMontage, bool _bInterruptted)
//{
//	if (_animMontage == Hit_Montage && !_bInterruptted)
//	{
//		//UE_LOG(LogTemp, Log, TEXT("I am recover from being hit - OnMontageBlendOutStartHandle"));
//		CurrentActionState = EAIActionState::None;
//		// Tell BT that this attack is done
//		if (mCharacter)
//		{
//			ATheLastBastionBaseAIController* baseAICtrl = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
//			// recover the rotation rate from melee attack motion sync
//			mCharacter->GetCharacterMovement()->RotationRate.Yaw = 540.0f;
//			if (baseAICtrl)
//			{
//				baseAICtrl->SetAICurrentActionState_BBC(CurrentActionState);
//				UBehaviorTreeComponent* btc = baseAICtrl->GetBTComp();
//				if (btc)
//				{
//					OnRecoverFromHitSignature.ExecuteIfBound(btc);
//				}
//			}
//		}
//	}
//}

//void UAIMelee_AnimInstance::OnBeingHit(FName boneName, const FVector & _shotFromDirection, const FVector & _hitLocation)
//{
//
//
//}



void UAIMelee_AnimInstance::ResetOnBeingHit()
{
	FinishAttack();
	OnDisableWeapon(true, true);

	// recover the rotation rate from melee attack motion sync
	mCharacter->GetCharacterMovement()->RotationRate.Yaw = 540.0f;

	Super::ResetOnBeingHit();
}


