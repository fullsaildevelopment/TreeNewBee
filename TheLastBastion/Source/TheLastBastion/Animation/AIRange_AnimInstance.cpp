// Fill out your copyright notice in the Description page of Project Settings.

#include "AIRange_AnimInstance.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Combat/PawnStatsComponent.h"
#include "Combat/RangeWeapon.h"



void UAIRange_AnimInstance::OnBeginPlay()
{
	Super::OnBeginPlay();

	//OnMontageStarted.AddDynamic(this, &UAIRange_AnimInstance::OnMontageStartHandle);
	//OnMontageBlendingOut.AddDynamic(this, &UAIRange_AnimInstance::OnMontageBlendOutStartHandle);
}

void UAIRange_AnimInstance::OnInit()
{
	Super::OnInit();

}

void UAIRange_AnimInstance::OnUpdate(float _deltaTime)
{
	Super::OnUpdate(_deltaTime);

	//if (mCharacter == nullptr)
	//	return;

	//switch (CurrentActionState)
	//{
	//case EAIActionState::GettingHurt:
	//	SyncMotionForGettingHurt();
	//	break;

	//default:
	//	break;
	//}

}

void UAIRange_AnimInstance::OnPostEvaluate()
{
	Super::OnPostEvaluate();
}

//void UAIRange_AnimInstance::OnMontageStartHandle(UAnimMontage * _animMontage)
//{
//
//}
//
//void UAIRange_AnimInstance::OnMontageBlendOutStartHandle(UAnimMontage * _animMontage, bool _bInterruptted)
//{
//	if (_animMontage == Hit_Montage && !_bInterruptted)
//	{
//		UE_LOG(LogTemp, Log, TEXT("I am recover from being hit - OnMontageBlendOutStartHandle"));
//		CurrentActionState = EAIActionState::None;
//		// Tell BT that this attack is done
//		if (mCharacter)
//		{
//			ATheLastBastionBaseAIController* enemyC = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
//			// recover the rotation rate from melee attack motion sync
//			mCharacter->GetCharacterMovement()->RotationRate.Yaw = 540.0f;
//			if (enemyC)
//			{
//				UBehaviorTreeComponent* btc = enemyC->GetBTComp();
//				if (btc)
//				{
//					OnRecoverFromHitSignature.ExecuteIfBound(btc);
//					//OnRecoverFromHitSignature.Unbind();
//				}
//			}
//		}
//	}
//
//
//}

void UAIRange_AnimInstance::Fire(const AActor * _target)
{
	if (mCharacter)
	{
		ARangeWeapon* RangeWeapon = Cast<ARangeWeapon>(mCharacter->GetEnemyStatsComponent()->GetCurrentRightHandWeapon());

		if (RangeWeapon)
		{
			bAim = true;
			RangeWeapon->NPCFire(_target);
			FName sectionToPlay;
			//sectionToPlay = TEXT("SingleShot");
			sectionToPlay = MONTAGE_CB_FireOnce;
			PlayMontage(Fire_Montage, 1.0f, sectionToPlay);
			//bAim = false;
			//UE_LOG(LogTemp, Log, TEXT("FIre"));
		}
	}

}

void UAIRange_AnimInstance::StopFire()
{
	bAim = false;
}

//void UAIRange_AnimInstance::OnBeingHit(FName boneName, 
//	const FVector & _shotFromDirection, const FVector & _hitLocation)
//{
//	if (Hit_Montage == nullptr)
//	{
//		UE_LOG(LogTemp, Error, TEXT("Hit Montage is nullptr - UAIRange_AnimInstance"));
//		return;
//	}
//
//	FName sectionToPlay;
//	ECharacterType Type = mCharacter->GetCharacterType();
//	switch (Type)
//	{
//	case ECharacterType::LanCB_T0:
//		sectionToPlay = HitReaction_SHSword(boneName, _shotFromDirection, _hitLocation);
//		break;
//	default:
//		break;
//	}
//
//	PlayMontage(Hit_Montage, 1.0f, sectionToPlay);
//
//}

//void UAIRange_AnimInstance::FinishAttack()
//{
//	CurrentActionState = EAIActionState::None;
//	// Tell BT that this attack is done
//	if (mCharacter)
//	{
//		ATheLastBastionBaseAIController* enemyC = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
//		if (enemyC)
//		{
//			UBehaviorTreeComponent* btc = enemyC->GetBTComp();
//			if (btc)
//			{
//				OnFinishAttackDelegate.ExecuteIfBound(btc);
//				//OnFinishAttackDelegate.Unbind();
//			}
//		}
//	}
//}
//
//void UAIRange_AnimInstance::InitFire()
//{
//	NextAction = EAIActionState::None;
//}

