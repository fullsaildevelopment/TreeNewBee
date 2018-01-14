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
	DodgeMinTurnThreshold = 1.0f;
	FocusDodgeDirection = EFocusDodgeDirection::None;
	bIsFocused = false;
}

void UMeleeHero_AnimInstance::OnBeginPlay()
{
	Super::OnBeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("UMeleeHero_AnimInstance Call BeginPlay, %d"), (int)AttackState);

	OnMontageStarted.AddDynamic(this, &UMeleeHero_AnimInstance::OnMontageStartHandle);
	OnMontageBlendingOut.AddDynamic(this, &UMeleeHero_AnimInstance::OnMontageBlendOutStartHandle);
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
		UCharacterMovementComponent* movementComp = mCharacter->GetCharacterMovement();

		if (ActivatedEquipment == EEquipType::Travel || AttackState == EAttackState::Dodging || AttackState == EAttackState::PostDodging)
			mAccelerationDirection = movementComp->GetCurrentAcceleration().GetSafeNormal();
		else
			mAccelerationDirection = UKismetMathLibrary::GetForwardVector(mCharacter->GetControlRotation());

		Acceleration_bodySpace
			= UKismetMathLibrary::InverseTransformDirection(mCharacter->GetTransform(), mAccelerationDirection);
		turn = FMath::RadiansToDegrees(FMath::Atan2(Acceleration_bodySpace.Y, Acceleration_bodySpace.X));

		// the more of the angle between forward vector and acceleration, the more rotation speed
		if (ActivatedEquipment == EEquipType::Travel)
		{
			movementComp->RotationRate.Yaw
				= UKismetMathLibrary::MapRangeClamped(FMath::Abs(turn), 0, 180.0f,
					mCharacter->GetMinTurnRateForTravel(),
					mCharacter->GetMaxTurnRateForTravel());
		}
		else
		{
			if (AttackState == EAttackState::PreWinding)
				movementComp->RotationRate.Yaw = mCharacter->GetMaxTurnRateForCombat();
			else
				movementComp->RotationRate.Yaw
				= UKismetMathLibrary::MapRangeClamped(FMath::Abs(turn), 0, 180.0f,
					mCharacter->GetMinTurnRateForCombat(),
					mCharacter->GetMaxTurnRateForCombat());
		}

		///** movement update during velocity override
		if (bVelocityOverrideByAnim)
		{
			float Z = movementComp->Velocity.Z;
			FVector overrideVelocity;

			switch (AttackState)
			{
			case EAttackState::None:
			case EAttackState::PreWinding:
			case EAttackState::Attacking:
			case EAttackState::ReadyForNext:
			default:
				overrideVelocity = mCharacter->GetActorForwardVector() * GetCurveValue("Speed");
				movementComp->Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, Z);
				break;
			case EAttackState::Dodging:
			case EAttackState::PostDodging:
			{
				if (bIsFocused)
					overrideVelocity = GetFocusDodgeDirection() * GetCurveValue("Speed");
				else
					overrideVelocity = mAccelerationDirection * GetCurveValue("Speed");
				movementComp->Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, Z);
				break;
			}
			case EAttackState::BeingHit:
			{
				overrideVelocity = damageMomentum * GetCurveValue("Speed");;
				movementComp->Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, Z);
				break;
			}
			}
		}

		///** Update controller rotation and camera rotation lag during focus
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
		}

		///** disable head track during slash someone
		if (AttackState == EAttackState::None)
			HeadTrackAlpha += 0.05f;
		else
			HeadTrackAlpha -= 0.05f;
		HeadTrackAlpha = FMath::Clamp(HeadTrackAlpha, 0.0f, 1.0f);


		///** determine spine track angle and disable spine track if we are not in attacking frame
		bool isNotAttackFrame = AttackState == EAttackState::None || 
			AttackState == EAttackState::Dodging || 
			AttackState == EAttackState::PostDodging;

		if (isNotAttackFrame)
			spineAngleOverrideAlpha -= 0.05f;
		else
			spineAngleOverrideAlpha += 0.05f;

		spineAngleOverrideAlpha = FMath::Clamp(spineAngleOverrideAlpha, 0.0f, .8f);
		spineAngleRotator = FRotator(0, 0, -15.f - HeadTrackPitch);
	}
}

void UMeleeHero_AnimInstance::OnPostEvaluate()
{

}

void UMeleeHero_AnimInstance::OnMontageStartHandle(UAnimMontage * _animMontage)
{

}

void UMeleeHero_AnimInstance::OnMontageBlendOutStartHandle(UAnimMontage * _animMontage, bool _bInterruptted)
{
	if (_animMontage == Attack_Montage && !_bInterruptted)
	{
		ResetCombo();
	}
	else if (_animMontage == Dodge_Montage)
	{
		OnDodgeFinish(_bInterruptted);
	}
	else if (_animMontage == Hit_Montage)
	{
		RecoverFromBeingHit(_bInterruptted);
	}
}

void UMeleeHero_AnimInstance::ResetOnBeingHit()
{

	Super::ResetOnBeingHit();

	CurrentComboIndex = 0;

	OnDisableDamage(false, true);
}

bool UMeleeHero_AnimInstance::OnEquip()
{
	if (Super::OnEquip())
	{
		// Equip
		if (ActivatedEquipment != CurrentEquipment)
		{
			// if we are sprinting then stop
			if (bIsSprinting)
			{
				mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetJogSpeed();;
				bIsSprinting = false;
			}
			DisableJump();
			bVelocityOverrideByAnim = false;
		}
		// UnEquip
		else
		{
			/** No need to be in focus mode, if we already sheath our weapon*/
			if (bIsFocused)
			{
				OnFocus();
				UE_LOG(LogTemp, Warning, TEXT("Unequip during focus, unfocus is implemented automatically"));
			}

		}

		/** play animation montage based on CurrentEquipment */
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


#pragma region  Focus Mode

void UMeleeHero_AnimInstance::OnMiddleMouseButtonPressed()
{
	if (mCharacter)
	{
		UHeroStatsComponent* heroStats = mCharacter->GetHeroStatsComp();

		if (heroStats)
		{
			heroStats->OnFocus();
			OnFocus();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("heroStats is Null - UMeleeHero_AnimInstance::OnMiddleMouseButtonPressed "));
		}
	}
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

void UMeleeHero_AnimInstance::OnCorLAltPressed()
{
	OnDodge();
}

void UMeleeHero_AnimInstance::ToggleFocusMode(bool _IsOn)
{
	bIsFocused = _IsOn;
	mCharacter->bIsYawControllDisabled = bIsFocused;

	if (ActivatedEquipment == EEquipType::Travel)
	{
		mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = _IsOn;
		mCharacter->GetCharacterMovement()->bOrientRotationToMovement = !_IsOn;
	}
	else
	{
		if (AttackState == EAttackState ::Attacking || AttackState == EAttackState ::ReadyForNext)
		{
			mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = _IsOn;
			mCharacter->GetCharacterMovement()->bOrientRotationToMovement = !_IsOn;
		}
	}
	bIsFocusEnterPending = false;
	bIsFocusExitPending = false;
}

#pragma endregion


#pragma region  Combo Attack

void UMeleeHero_AnimInstance::OnAttack()
{

	bool ignore = Montage_IsPlaying(Hit_Montage) || Attack_Montage == nullptr || bIsInAir;


	// Apply Input Filter
	if (ignore)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack ignore - UMeleeHero_AnimInstance::OnAttack"));
		return;
	}

	// equip weapon to weapon socket if we havent
	if (ActivatedEquipment == EEquipType::Travel)
		SkipEquip();

	switch (AttackState)
	{
	case EAttackState::None:
	case EAttackState::ReadyForNext:
	case EAttackState::PostDodging:
	{
		// trigger the attack immediately, clear the next action marker
		//UE_LOG(LogTemp, Warning, TEXT("trigger the attack immediately, clear the next action marker"));
		LaunchCombo();
		return;
	}

	case EAttackState::Attacking:
	case EAttackState::Dodging:
	{
		// catch attack action, update next action marker, 
		// and wait if not being updated by later input
		//UE_LOG(LogTemp, Warning, TEXT("catch attack action, update next action marker"));
		NextAction = EActionType::Attack;
		return;
	}

	case EAttackState::PreWinding:
	default:
		//UE_LOG(LogTemp, Warning, TEXT("current attack state ignore attack action"));
		return;
	}


}

void UMeleeHero_AnimInstance::LaunchCombo()
{
	if (CurrentComboIndex >= Attack_Montage_SectionNames.Num())
	{
		// start over
		//UE_LOG(LogTemp, Warning, TEXT("mCurrentComboIndex: %d, reach max Combo Index, reset "), CurrentComboIndex);
		CurrentComboIndex = 0;
	}

	this->PlayMontage(Attack_Montage, 1.0f, Attack_Montage_SectionNames[CurrentComboIndex]);
	AttackState = EAttackState::PreWinding;
	NextAction = EActionType::None;
	CurrentComboIndex++;
	bVelocityOverrideByAnim = true;

	if (!bIsFocused)
	{
		mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	//bRotationRateOverrideByAnim = false;
}

void UMeleeHero_AnimInstance::ResetCombo()
{
	UE_LOG(LogTemp, Log, TEXT("Reset Combo"));
	AttackState = EAttackState::None;
	NextAction = EActionType::None;

	if (AttackState == EAttackState::Dodging)
		return;
	CurrentComboIndex = 0;
	bVelocityOverrideByAnim = false;

	if (!bIsFocused)
	{
		mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	//mCharacter->GetMovementComponent()->Velocity = mAccelerationDirection;
}

void UMeleeHero_AnimInstance::OnEnableDamage(bool bIsright, bool bIsAll)
{
	if (mCharacter)
	{
		AttackState = EAttackState::Attacking;
		mCharacter->GetHeroStatsComp()->SetEnableWeapon(true, bIsright, bIsAll);

		//Lock the character rotation during attacking if it is not in focus mode
		if (!bIsFocused)
		{
			mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
			mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}
}

void UMeleeHero_AnimInstance::OnDisableDamage(bool bIsright, bool bIsAll)
{
	//Super::OnDisableWeapon(bIsright, bIsAll);
	mCharacter->GetHeroStatsComp()->SetEnableWeapon(false, bIsright, bIsAll);

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

#pragma endregion


#pragma region Dodge
void UMeleeHero_AnimInstance::OnDodge()
{

	bool ignore
		= Montage_IsPlaying(Hit_Montage) || 
		bIsInAir || Dodge_Montage == nullptr;
	//(FMath::Abs(turn) > DodgeMinTurnThreshold && !bIsFocused)

	if (ignore)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Dodge is ignored due to inAir or Montage Null"));
		return;
	}

	// if we havnt equip our weapon, skip animation and attach weapon to weapon socket
	if (ActivatedEquipment == EEquipType::Travel)
		SkipEquip();

	switch (AttackState)
	{
	case EAttackState::None:
	case EAttackState::PreWinding:
	case EAttackState::ReadyForNext:
	{
		// Trigger the action immeditately, clear the next action marker
		LaunchDodge();
		return;
	}

	case EAttackState::Attacking:
	case EAttackState::PostDodging:
	{
		// Update the next action marker,
		// waiting until the current action is finished, 
		// if it is not be update by later input 
		UE_LOG(LogTemp, Warning, TEXT("Catch continuous Dodge"));
		NextAction = EActionType::Dodge;
		return;
	}
	case EAttackState::Dodging:
	default:
	{
		// Ignore
		return;
	}
	}
}

void UMeleeHero_AnimInstance::LaunchDodge()
{
	//Super::LaunchDodge();

	// Update State
	AttackState = EAttackState::Dodging;
	// Clear Next Action Marker
	NextAction = EActionType::None;

	bVelocityOverrideByAnim = true;
	CurrentComboIndex = 0;


	if (!bIsFocused)
	{
		// when we are dodging while not in focus, we only dodge forward, in the direction
		// of our current movement acceleration
		mCharacter->bUsePreviousMovementAxis = true;
		mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
		mCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		this->PlayMontage(Dodge_Montage, 1.0f, TEXT("Dodge_Fwd"));
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

void UMeleeHero_AnimInstance::OnDodgePost()
{
	//Super::OnDodgePost();

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

void UMeleeHero_AnimInstance::OnDodgeFinish(bool _bInterruptted)
{
	if (!_bInterruptted)
	{
		AttackState = EAttackState::None;
		bVelocityOverrideByAnim = false;
		FocusDodgeDirection = EFocusDodgeDirection::None;

		if (!bIsFocused)
		{
			mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
			mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}

	if (!bIsFocused)
	{
		mCharacter->bUsePreviousMovementAxis = false;
		//mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		//mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	// if we have a pending focus, this is a time to do it
	if (bIsFocusEnterPending)
		ToggleFocusMode(true);
	else if (bIsFocusExitPending)
		ToggleFocusMode(false);

}

FVector UMeleeHero_AnimInstance::GetFocusDodgeDirection() const
{
	switch (FocusDodgeDirection)
	{
	case EFocusDodgeDirection::Forward:
		return mCharacter->GetActorForwardVector();

	case EFocusDodgeDirection::Right45:
		return (.5 * mCharacter->GetActorForwardVector() + mCharacter->GetActorRightVector()).GetUnsafeNormal();

	case EFocusDodgeDirection::Left45:
		return (.5f *  mCharacter->GetActorForwardVector() - mCharacter->GetActorRightVector()).GetUnsafeNormal();

	case EFocusDodgeDirection::Right135:
		return (-.5f * mCharacter->GetActorForwardVector() + mCharacter->GetActorRightVector()).GetUnsafeNormal();

	case EFocusDodgeDirection::Left135:
		return (-.5f * mCharacter->GetActorForwardVector() - mCharacter->GetActorRightVector()).GetUnsafeNormal();

	case EFocusDodgeDirection::Right90:
		return mCharacter->GetActorRightVector();
	case EFocusDodgeDirection::Back:
	default:
	{
		FVector back = mCharacter->GetActorForwardVector();
		back = FVector(-back.X, -back.Y, -back.Z);
		return back;
	}
	case EFocusDodgeDirection::Left90:
	{
		FVector left = mCharacter->GetActorRightVector();
		left = FVector(-left.X, -left.Y, -left.Z);
		return left;
	}
	}
}

#pragma endregion







