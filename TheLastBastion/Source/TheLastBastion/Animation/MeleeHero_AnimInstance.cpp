// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeHero_AnimInstance.h"
#include "TheLastBastionHeroCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Combat/HeroStatsComponent.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "TimerManager.h"


const TArray<FName> Attack_Montage_SectionNames
= {FName("Combo_0"), FName("Combo_1") ,FName("Combo_2"),FName("Combo_3"), FName("Combo_4"), FName("Combo_5")};



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

	if (mCharacter)
	{
		if (bIsFocused)
		{
			mCharacter->GetCameraBoom()->CameraRotationLagSpeed = mCharacter->GetLockOnCameraRotationLag();
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
			else
			{
				UE_LOG(LogTemp, Warning,
					TEXT("Melee Hero has null target during focus mode, this may caused by focus exit pending"));
			}
		}
		else
		{
			mCharacter->GetCameraBoom()->CameraRotationLagSpeed 
				= UKismetMathLibrary::MapRangeClamped(FMath::Abs(turn),
					0, 90.0f,
					mCharacter->GetLockOnCameraRotationLag(),
					mCharacter->GetNonLockOnCameraRotationLag());

				
				mCharacter->GetLockOnCameraRotationLag();
		}
		// disable head track during slash someone
		if (AttackState == EAttackState::None)
			HeadTrackAlpha += 0.05f;
		else
			HeadTrackAlpha -= 0.05f;
		HeadTrackAlpha = FMath::Clamp(HeadTrackAlpha, 0.0f, 1.0f);
		spineAngleOverrideAlpha
			= FMath::Clamp(1 - HeadTrackAlpha, 0.0f, 0.8f);
		spineAngleRotator = FRotator(0, 0, -15.f - HeadTrackPitch);
	}

}

void UMeleeHero_AnimInstance::OnPostEvaluate()
{

}

bool UMeleeHero_AnimInstance::OnEquip()
{	
	if (Super::OnEquip())
	{
		/** if we pass the input filter, and we are going to do animation ?*/
		switch (CurrentEquipment)
		{
		case EEquipType::ShieldSword:
			if (ActivatedEquipment == EEquipType::Travel)
			{
				// Equip
				this->PlayMontage(Equip_Montage, 1.0f, TEXT("EquipSHSword"));

			}
			else if (ActivatedEquipment == CurrentEquipment)
			{
				// Unequip
				this->PlayMontage(Equip_Montage, 1.0f, TEXT("UnequipSHSword"));
			}

			break;
		case EEquipType::TwoHandSword:
			if (ActivatedEquipment == EEquipType::Travel)
			{
				// Equip
				this->PlayMontage(Equip_Montage, 1.0f, TEXT("EquipTHSword"));
			}
			else if (ActivatedEquipment == CurrentEquipment)
			{
				// Unequip
				this->PlayMontage(Equip_Montage, 1.0f, TEXT("UnequipTHSword"));
			}
			break;
		}
		return true;
	}
	else return false;

}

void UMeleeHero_AnimInstance::OnActionInterrupt()
{

	//UE_LOG(LogTemp, Error, TEXT("OnActionInterrupt UMeleeHero_AnimInstance"));

	Super::OnActionInterrupt();
	CurrentComboIndex = 0;
}

void UMeleeHero_AnimInstance::OnFocus()
{

	// if we dont have a target at all, why bother to focus, right?
	if (mCharacter->GetHeroStatsComp()->GetCurrentTarget() == nullptr)
	{
		// Toggle the focus off, the hero stats comp will clear the current target, if we are currently dodging, we need to set
		// this focus exit to pending
		if (AttackState == EAttackState::Dodging || AttackState == EAttackState::PostDodging)
			bIsFocusExitPending = true;
		else
			ToggleFocusMode(false);
		return;
	}

	// how can we enter the focus mode without get our blade on, right? Focus always false in Travel
	if (ActivatedEquipment == EEquipType::Travel)
	{
		// Equip, play animation
		UE_LOG(LogTemp, Warning, TEXT("Focus during travel, equip is implemented automatically"));
		OnEquip();
	}

	// Toggle focus mode
	if (bIsFocused == false)
	{
		// if we are going to focus during dodging, set focus to pending, and enter focus when this dodging is finished
		if (AttackState == EAttackState::Dodging || AttackState == EAttackState::PostDodging)
			bIsFocusEnterPending = true;
		else
			ToggleFocusMode(true);
	}
	else
	{
		// this is for using dodge quit out of enemy detector
		if (AttackState == EAttackState::Dodging || AttackState == EAttackState::PostDodging)
			bIsFocusExitPending = true;
		else
			ToggleFocusMode(false);
	}

}

#pragma region  Combo Attack
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
		// start over
		UE_LOG(LogTemp, Warning, TEXT("mCurrentComboIndex: %d, reach max Combo Index, reset "), CurrentComboIndex);
		CurrentComboIndex = 0;
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

	//Lock the character rotation during attacking if it is not in focus mode
	if (!bIsFocused)
	{
		bRotationRateOverrideByAnim = true;
		mCharacter->GetCharacterMovement()->RotationRate.Yaw = 0;
	}
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

#pragma endregion


#pragma region Dodge
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

	if (FocusDodgeDirection != EFocusDodgeDirection::None)
	{
		mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	}

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

	if (!bIsFocused)
	{
		// Lock the rotation rate if it is not focus mode
		bRotationRateOverrideByAnim = true;
		mCharacter->GetCharacterMovement()->RotationRate.Yaw = 0;
		if (bTryToMove)
		{
			mSpeedOverrideDirection = mAccelerationDirection;
			this->PlayMontage(Dodge_Montage, 1.0f, TEXT("Dodge_Fwd"));
		}
		else
		{
			mSpeedOverrideDirection = mCharacter->GetActorForwardVector();
			mSpeedOverrideDirection = FVector(-mSpeedOverrideDirection.X, -mSpeedOverrideDirection.Y, -mSpeedOverrideDirection.Z);
			this->PlayMontage(Dodge_Montage, 1.0f, TEXT("Dodge_Bwd"));
		}
	}
	else
	{

		// Determine with animation to play by the input,
		// Mark the direction, for movement sync in update 
		FName dodgeSection;
		if (MoveForwardAxis == 0)
		{
			if (MoveRightAxis > 0)
			{
				dodgeSection = TEXT("Dodge_Right90");
				FocusDodgeDirection = EFocusDodgeDirection::Right90;
			}
			else if (MoveRightAxis < 0)
			{
				dodgeSection = TEXT("Dodge_Left90");
				FocusDodgeDirection = EFocusDodgeDirection::Left90;
			}
			else
			{
				dodgeSection = TEXT("Dodge_Bwd");
				FocusDodgeDirection = EFocusDodgeDirection::Back;
			}
		}
		else if (MoveForwardAxis > 0)
		{
			if (MoveRightAxis > 0)
			{
				dodgeSection = TEXT("Dodge_Right90");
				FocusDodgeDirection = EFocusDodgeDirection::Right45;
			}
			else if (MoveRightAxis<0)
			{
				dodgeSection = TEXT("Dodge_Left90");
				FocusDodgeDirection = EFocusDodgeDirection::Left45;
			}
			else
			{
				dodgeSection = TEXT("Dodge_Fwd");
				FocusDodgeDirection = EFocusDodgeDirection::Forward;
			}
		}
		else
		{
			if (MoveRightAxis > 0)
			{
				dodgeSection = TEXT("Dodge_Right90");
				FocusDodgeDirection = EFocusDodgeDirection::Right135;

			}
			else if (MoveRightAxis<0)
			{
				dodgeSection = TEXT("Dodge_Left90");
				FocusDodgeDirection = EFocusDodgeDirection::Left135;
			}
			else
			{
				dodgeSection = TEXT("Dodge_Bwd");
				FocusDodgeDirection = EFocusDodgeDirection::Back;
			}
		}

		this->PlayMontage(Dodge_Montage, 1.0f, dodgeSection);
	}

}

void UMeleeHero_AnimInstance::OnDodgeFinish()
{
	Super::OnDodgeFinish();

	// Focus Dodge is over, reset the direction flag
	FocusDodgeDirection = EFocusDodgeDirection::None;
	// Check Next Action Mark, 	this helps for dodge continuously

	// if we have a pending focus, this is a time to do it
	if (bIsFocusEnterPending)
		ToggleFocusMode(true);
	else if (bIsFocusExitPending)
		ToggleFocusMode(false);

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

#pragma endregion







