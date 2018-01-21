// Fill out your copyright notice in the Description page of Project Settings.

#include "AIRange_AnimInstance.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "AI/TheLastBastionBaseAIController.h"
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

	if (mCharacter == nullptr)
		return;
}

void UAIRange_AnimInstance::OnUpdate(float _deltaTime)
{
	Super::OnUpdate(_deltaTime);
}

void UAIRange_AnimInstance::OnPostEvaluate()
{
	Super::OnPostEvaluate();
}

void UAIRange_AnimInstance::OnMontageStartHandle(UAnimMontage * _animMontage)
{

}

void UAIRange_AnimInstance::OnMontageBlendOutStartHandle(UAnimMontage * _animMontage, bool _bInterruptted)
{

}

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
			sectionToPlay = TEXT("PowerShot");
			PlayMontage(Fire_Montage, 1.0f, sectionToPlay);
			//bAim = false;
			UE_LOG(LogTemp, Log, TEXT("FIre"));
		}
	}

}

void UAIRange_AnimInstance::StopFire()
{
	bAim = false;
}

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

