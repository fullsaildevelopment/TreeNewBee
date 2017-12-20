// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeHero_AnimInstance.h"
#include "TheLastBastionHeroCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"

#include "Combat/HeroStatsComponent.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "TimerManager.h"


const TArray<FName> Attack_Montage_SectionNames
= {FName("Combo_0"), FName("Combo_1") ,FName("Combo_2"),FName("Combo_3"), FName("Combo_4") };



UMeleeHero_AnimInstance ::UMeleeHero_AnimInstance(const FObjectInitializer& _objectInitalizer) : Super(_objectInitalizer)
{
	CurrentComboIndex = 0;
	CurrentEquipment = EEquipType::ShieldSword;
}

void UMeleeHero_AnimInstance::OnBeginPlay()
{
	Super::OnBeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("UMeleeHero_AnimInstance Call BeginPlay, %d"), (int)AttackState);

}

void UMeleeHero_AnimInstance::OnInit()
{
	Super::OnInit();
	//UE_LOG(LogTemp, Warning, TEXT("UMeleeHero_AnimInstance Call OnInit"));
}

void UMeleeHero_AnimInstance::OnUpdate(float _deltaTime)
{
	Super::OnUpdate(_deltaTime);


	if (bIsFocused)
	{
		FRotator unFocus = mCharacter->GetController()->GetControlRotation();

		if (mCharacter->GetHeroStatsComp()->GetCurrentTarget())
		{
			FRotator Focus
				= UKismetMathLibrary::FindLookAtRotation(
					mCharacter->GetFollowCamera()->GetComponentLocation(),
					mCharacter->GetHeroStatsComp()->GetCurrentTarget()->GetActorLocation());
			Focus.Roll = unFocus.Roll;
			Focus.Pitch = unFocus.Pitch;
			mCharacter->GetController()->SetControlRotation(Focus);
		}
	}

	// disable head track during slash someone
	if (AttackState == EAttackState :: None)
		HeadTrackAlpha += 0.05f;
	else
		HeadTrackAlpha -= 0.05f;

	HeadTrackAlpha = FMath::Clamp(HeadTrackAlpha, 0.0f, 1.0f);
}

void UMeleeHero_AnimInstance::OnPostEvaluate()
{

}


void UMeleeHero_AnimInstance::OnEquip()
{
	Super::OnEquip();

	switch (CurrentEquipment)
	{
	case EEquipType::ShieldSword:
		if (ActivatedEquipment == EEquipType::Travel)
		{
			// Unequip
			this->PlayMontage(Equip_Montage, 1.0f, TEXT("UnequipSHSword"));
			OnActionInterrupt();
		}
		else if (ActivatedEquipment == CurrentEquipment)
		{
			// Equip
			this->PlayMontage(Equip_Montage, 1.0f, TEXT("EquipSHSword"));
		}

		break;
	case EEquipType::TwoHandSword:
		if (ActivatedEquipment == EEquipType::Travel)
		{
			// Unequip
			this->PlayMontage(Equip_Montage, 1.0f, TEXT("UnequipTHSword"));
			OnActionInterrupt();
		}
		else if (ActivatedEquipment == CurrentEquipment)
		{
			// Equip
			this->PlayMontage(Equip_Montage, 1.0f, TEXT("EquipTHSword"));
		}
		break;
	}

	UE_LOG(LogTemp, Warning, TEXT("Equip ! - UMeleeHero_AnimInstance"));

}

void UMeleeHero_AnimInstance::OnActionInterrupt()
{
	Super::OnActionInterrupt();
	CurrentComboIndex = 0;
}

void UMeleeHero_AnimInstance::OnFocus()
{

	if (mCharacter->GetHeroStatsComp()->GetCurrentTarget() == nullptr)
	{
		bIsFocused = false;
		mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
		mCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		return;
	}

	if (ActivatedEquipment == EEquipType::Travel)
	{
		// Equip, play animation
		UE_LOG(LogTemp, Warning, TEXT("Focus during travel, equip is implemented automatically"));
		OnEquip();
	}

	if (bIsFocused == false)
	{
		bIsFocused = true;
		mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;

	}
	else
	{
		bIsFocused = false;
		mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
		mCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	}

}


bool UMeleeHero_AnimInstance::OnAttack()
{
	if (Super::OnAttack())
	{
		switch (AttackState)
		{
		case EAttackState::None:
		case EAttackState::ReadyForNext:
		case EAttackState::PostDodging:
		{
			// trigger the attack immediately, clear the next action marker
			UE_LOG(LogTemp, Warning, TEXT("trigger the attack immediately, clear the next action marker"));
			LaunchCombo();
			return true;
		}

		case EAttackState::Attacking:
		case EAttackState::Dodging:
		{
			// catch attack action, update next action marker, 
			// and wait if not being updated by later input
			UE_LOG(LogTemp, Warning, TEXT("catch attack action, update next action marker"));
			NextAction = EActionType::Attack;
			//NextComboRotation = mAccelerationDirection;
			return true;
		}

		case EAttackState::BeAttacked:
		case EAttackState::PreWinding:
		default:
			UE_LOG(LogTemp, Warning, TEXT("current attack state ignore attack action"));
			return false;
		}
	}
	else
		return false;
}


void UMeleeHero_AnimInstance::LaunchCombo()
{
	if (CurrentComboIndex >= Attack_Montage_SectionNames.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("mCurrentComboIndex: %d, reach max Combo Index, reset "), CurrentComboIndex);
		CurrentComboIndex = 0;

		//bIsAnimationVelocityPrior = false;
		////bVelocityOverrideByAnim = false;
		//bRotationRateOverrideByAnim = false;
		//AttackState = EAttackState::None;
		//NextAction = EActionType::None;
		//return;
	}

	this->PlayMontage(Attack_Montage, 1.0f, Attack_Montage_SectionNames[CurrentComboIndex]);
	AttackState = EAttackState::PreWinding;
	NextAction = EActionType::None;
	CurrentComboIndex++;
	bVelocityOverrideByAnim = true;
	bRotationRateOverrideByAnim = false;
}

void UMeleeHero_AnimInstance::OnEnableDamage(bool bIsright, bool bIsAll)
{
	Super::OnEnableWeapon(bIsright, bIsAll);

	//Lock the character rotation during attacking

	bRotationRateOverrideByAnim = true;
	mCharacter->GetCharacterMovement()->RotationRate.Yaw = 0;
}

void UMeleeHero_AnimInstance::OnDisableDamage(bool bIsright, bool bIsAll)
{
	Super::OnDisableWeapon(bIsright, bIsAll);
}

void UMeleeHero_AnimInstance::OnNextAttack()
{
	switch (NextAction)
	{
	case EActionType::None:
	default:
		// We did not catch any action, we put ourself to wait
		// and recover the rotate
		AttackState = EAttackState::ReadyForNext;
		bRotationRateOverrideByAnim = true;
		break;
	case EActionType::Attack:
		LaunchCombo();
		break;
	case EActionType::Skill:
		break;
	case EActionType::Dodge:
		LaunchDodge();
		break;
	}

}

void UMeleeHero_AnimInstance::OnResetCombo()
{
	Super::OnResetCombo();
	if (AttackState == EAttackState::Dodging)
		return;
	CurrentComboIndex = 0;
	bVelocityOverrideByAnim = false;
	bRotationRateOverrideByAnim = false;
	mCharacter->GetMovementComponent()->Velocity = mAccelerationDirection;
}


bool UMeleeHero_AnimInstance::OnDodge()
{
	if (Super::OnDodge())
	{
		switch (AttackState)
		{
		case EAttackState::None:
		case EAttackState::PreWinding:
		case EAttackState::ReadyForNext:
		{
			// Trigger the action immeditately, clear the next action marker
			LaunchDodge();
			return true;
		}

		case EAttackState::Attacking:
		case EAttackState::PostDodging:
		{
			// Update the next action marker,
			// waiting until the current action is finished, 
			// if it is not be update by later input 
			UE_LOG(LogTemp, Warning, TEXT("Catch continuous Dodge"));
			NextAction = EActionType::Dodge;
			return true;
		}

		case EAttackState::BeAttacked:
		case EAttackState::Dodging:
		default:
		{
			// Ignore
			return false;
		}
		}
	}
	else return false;
}

void UMeleeHero_AnimInstance::OnDodgePost()
{
	Super::OnDodgePost();
	// Check Next Action Mark
	switch (NextAction)
	{
	case EActionType::None:
	case EActionType::Dodge:
	default:
		// We did not catch any action, we put ourself to wait
		// speed and rotation still being locked up
		AttackState = EAttackState::PostDodging;
		break;
	case EActionType::Attack:
		LaunchCombo();
		break;
	case EActionType::Skill:
		break;
	}


}

void UMeleeHero_AnimInstance::LaunchDodge()
{	
	Super::LaunchDodge();
	CurrentComboIndex = 0;
}

void UMeleeHero_AnimInstance::OnDodgeFinish()
{
	Super::OnDodgeFinish();

	// Check Next Action Mark, 
	// This is only for dodge continuously
	switch (NextAction)
	{
	case EActionType::None:
	default:
		break;
		// We did not catch any action, we put ourself to wait
		// speed and rotation still being locked up
		AttackState = EAttackState::PostDodging;
		break;
	case EActionType::Dodge:
		LaunchDodge();
		break;
	case EActionType::Attack:
		LaunchCombo();
		break;
	case EActionType::Skill:
		break;
	}
}






