// Fill out your copyright notice in the Description page of Project Settings.

#include "Hero_AnimInstance.h"
#include "TheLastBastionHeroCharacter.h"

#include "Combat/HeroStatsComponent.h"
#include "Combat/RangeWeapon.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine.h"

const TArray<FName> Attack_Montage_SectionNames
= { FName("Combo_0"), FName("Combo_1") ,FName("Combo_2"),FName("Combo_3"), FName("Combo_4"), FName("Combo_5") };


#define MONTAGE_SN_HERO_SnsEquip   TEXT("SnsEquip")
#define MONTAGE_SN_HERO_SnsUnequip TEXT("SnsUnequip")
#define MONTAGE_SN_HERO_CBEquip    TEXT("CBEquip")
#define MONTAGE_SN_HERO_CBUnequip  TEXT("CBUnequip")
#define MONTAGE_SN_HERO_FireOnce   TEXT("FireOnce")

#pragma region Class Base Interface

UHero_AnimInstance::UHero_AnimInstance(const FObjectInitializer& _objectInitalizer) :Super(_objectInitalizer)
{
	HeadTrackRate = 3.0f;

	ActivatedEquipment = EEquipType::Travel;
	AttackState = EAttackState::None;
	bTryToSprint = false;
	bTryToMove = false;
	bIsSprinting = false;
	bTryToEquip = false;
	bVelocityOverrideByAnim = false;

	MomentumRatioByActor = 0.7f;

	// Melee Init
	CurrentComboIndex = 0;
	CurrentEquipment = EEquipType::ShieldSword;
	FocusDodgeDirection = EFocusDodgeDirection::None;
	bIsFocused = false;

	// Range Init
	bTryToZoomIn = false;
	CameraZoomInRate = 10.0f;
	CameraShiftRate = 3.0f;
	CameraEquipOffset = FVector(100, 50, 0);
	CameraZoomOffset = FVector(200, 50, 10);

}

void UHero_AnimInstance::OnBeginPlay()
{
	APawn* pawn = TryGetPawnOwner();
	mCharacter = Cast<ATheLastBastionHeroCharacter>(pawn);
	if (mCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("The MeleeHero can only assigned to ATheLastBastionCharacter - UMeleeHero_AnimInstance "));
		return;
	}

	OnMontageStarted.AddDynamic(this, &UHero_AnimInstance::OnMontageStartHandle);
	OnMontageBlendingOut.AddDynamic(this, &UHero_AnimInstance::OnMontageBlendOutStartHandle);
}

void UHero_AnimInstance::OnInit()
{
	//UE_LOG(LogTemp, Warning, TEXT("UHero_AnimInstance Call OnInit"));
}

void UHero_AnimInstance::OnUpdate(float _deltaTime)
{
	if (mCharacter != nullptr)
	{
		// Head Track
		HeadTrack();

		// Get Control Axis for strafing
		MoveForwardAxis = mCharacter->GetMoveForwardAxis();
		MoveRightAxis = mCharacter->GetMoveRightAxis();

		UCharacterMovementComponent* movementComp = mCharacter->GetCharacterMovement();

		// Check if the play is falling?
		bIsInAir = movementComp->IsFalling();
		// Check if the player try to move?
		bTryToMove = movementComp->GetCurrentAcceleration().SizeSquared() > 0;

		currentSpeed = movementComp->Velocity.Size();


		if (ActivatedEquipment == EEquipType::Travel || AttackState == EAttackState::Dodging || AttackState == EAttackState::PostDodging)
			mAccelerationDirection = movementComp->GetCurrentAcceleration().GetSafeNormal();
		else
			mAccelerationDirection = UKismetMathLibrary::GetForwardVector(mCharacter->GetControlRotation());

		Acceleration_bodySpace
			= UKismetMathLibrary::InverseTransformDirection(mCharacter->GetTransform(), mAccelerationDirection);
		turn = FMath::RadiansToDegrees(FMath::Atan2(Acceleration_bodySpace.Y, Acceleration_bodySpace.X));

		switch (CurrentEquipment)
		{
		case EEquipType::CrossBow:
			RangeUpdate(movementComp, _deltaTime);
			break;
		case EEquipType::ShieldSword:
			MeleeUpdate(movementComp, _deltaTime);
			break;
		}
	}
}

void UHero_AnimInstance::MeleeUpdate(UCharacterMovementComponent * movementComp, float _deltaTime)
{
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

	// Set camera back to origional
	mCharacter->GetFollowCamera()->RelativeLocation
		= UKismetMathLibrary::VInterpTo(mCharacter->GetFollowCamera()->RelativeLocation, FVector::ZeroVector, _deltaTime, CameraShiftRate);

}

void UHero_AnimInstance::RangeUpdate(UCharacterMovementComponent* movementComp, float _deltaTime)
{
	if (ActivatedEquipment == EEquipType::Travel)
	{
		mCharacter->GetFollowCamera()->RelativeLocation
			= UKismetMathLibrary::VInterpTo(mCharacter->GetFollowCamera()->RelativeLocation, FVector::ZeroVector, _deltaTime, CameraShiftRate);
	}
	else
	{
		if (bTryToZoomIn)
		{
			// walk in zoom in mode
			if (!bTryToSprint)
			{
				MoveForwardAxis *= 0.5f;
				MoveRightAxis *= 0.5f;
				movementComp->MaxWalkSpeed = mCharacter->GetWalkSpeed();
			}
			else
				movementComp->MaxWalkSpeed = mCharacter->GetJogSpeed();

			mCharacter->GetFollowCamera()->RelativeLocation
				= UKismetMathLibrary::VInterpTo(mCharacter->GetFollowCamera()->RelativeLocation, CameraZoomOffset, _deltaTime, CameraZoomInRate);
		}
		else
		{
			mCharacter->GetFollowCamera()->RelativeLocation
				= UKismetMathLibrary::VInterpTo(mCharacter->GetFollowCamera()->RelativeLocation, CameraEquipOffset, _deltaTime, CameraShiftRate);
			movementComp->MaxWalkSpeed = mCharacter->GetJogSpeed();
		}
	}

	movementComp->RotationRate.Yaw
		= UKismetMathLibrary::MapRangeClamped(FMath::Abs(turn), 0, 180.0f,
			mCharacter->GetMinTurnRateForTravel(),
			mCharacter->GetMaxTurnRateForTravel());

	if (bVelocityOverrideByAnim)
	{
		float Z = movementComp->Velocity.Z;
		FVector overrideVelocity;
		overrideVelocity = mCharacter->GetActorForwardVector() * GetCurveValue("Speed");
		movementComp->Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, Z);

		if (AttackState == EAttackState::BeingHit)
		{
			overrideVelocity = damageMomentum * GetCurveValue("Speed");;
			movementComp->Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, Z);
		}
	}

}

void UHero_AnimInstance::OnPostEvaluate()
{
}

#pragma endregion

void UHero_AnimInstance::OnMontageStartHandle(UAnimMontage * _animMontage)
{

}

void UHero_AnimInstance::OnMontageBlendOutStartHandle(UAnimMontage * _animMontage, bool _bInterruptted)
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

void UHero_AnimInstance::OnMiddleMouseButtonPressed()
{
	// Handled by child class
	switch (CurrentEquipment)
	{
	case EEquipType::ShieldSword:
	case EEquipType::TwoHandSword:
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

		break;
	case EEquipType::CrossBow:
		break;
	default:
		break;
	}
}

void UHero_AnimInstance::OnCorLAltPressed()
{
	switch (CurrentEquipment)
	{
	case EEquipType::ShieldSword:
	case EEquipType::TwoHandSword:
		OnDodge();

		break;
	case EEquipType::CrossBow:
		break;
	default:
		break;
	}



}

void UHero_AnimInstance::OnRightMouseButtonPressed()
{
	switch (CurrentEquipment)
	{
	case EEquipType::ShieldSword:
	case EEquipType::TwoHandSword:
		
		break;
	case EEquipType::CrossBow:
		OnZoomIn();
		break;
	}
}

void UHero_AnimInstance::OnRightMouseButtonReleased()
{
	switch (CurrentEquipment)
	{
	case EEquipType::ShieldSword:
	case EEquipType::TwoHandSword:

		break;
	case EEquipType::CrossBow:
		OnZoomOut();
		break;
	}

}

void UHero_AnimInstance::OnSprintPressed()
{
	bTryToSprint = true;

	//bool ableToSprint = !bSpeedOverrideByAnim && ActivatedEquipment == EEquipType::Travel;
	bool ableToSprint = ActivatedEquipment == EEquipType::Travel && !bIsInAir;

	if (ableToSprint)
	{
		mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetSprintSpeed();
		bIsSprinting = true;
	}
}

void UHero_AnimInstance::OnSprintReleased()
{
	bTryToSprint = false;

	mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetJogSpeed();;
	bIsSprinting = false;
}

void UHero_AnimInstance::StartOverrideSpeed()
{
	if (ActivatedEquipment == EEquipType::Travel && bTryToEquip == false)
		bVelocityOverrideByAnim = true;
}

void UHero_AnimInstance::StopOverrideSpeed()
{

	bool ignore = AttackState == EAttackState::Dodging || AttackState == EAttackState::Attacking;
	if (ignore)
		return;	

	bVelocityOverrideByAnim = false;
	mCharacter->GetCharacterMovement()->MaxWalkSpeed 
		= (bIsSprinting)? mCharacter->GetSprintSpeed() : mCharacter->GetJogSpeed();
}


void UHero_AnimInstance::OnFocus()
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

void UHero_AnimInstance::ToggleFocusMode(bool _IsOn)
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
		if (AttackState == EAttackState::Attacking || AttackState == EAttackState::ReadyForNext)
		{
			mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = _IsOn;
			mCharacter->GetCharacterMovement()->bOrientRotationToMovement = !_IsOn;
		}
	}
	bIsFocusEnterPending = false;
	bIsFocusExitPending = false;
}


#pragma region Equip && Unequip


bool UHero_AnimInstance::OnEquip()
{
	bool ignore
		= AttackState != EAttackState::None
		|| Montage_IsPlaying(Equip_Montage)
		|| Equip_Montage == nullptr;

	if (ignore)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Equip Function is ignore due to wrong attack state, running montage or null Montage"));
		return false;
	}
	else
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
			bTryToEquip = true;
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
			bTryToEquip = false;
		}

		/** play animation montage based on CurrentEquipment */
		switch (CurrentEquipment)
		{
		case EEquipType::ShieldSword:
		{
			if (ActivatedEquipment == EEquipType::Travel)
			{
				// Equip
				this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_SnsEquip);
			}
			else if (ActivatedEquipment == CurrentEquipment)
			{
				// Unequip
				this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_SnsUnequip);
			}

			break;

		}
		case EEquipType::TwoHandSword:
		{
			if (ActivatedEquipment == EEquipType::Travel)
			{
				// Equip
				this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_CBEquip);
			}
			else if (ActivatedEquipment == CurrentEquipment)
			{
				// Unequip
				this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_CBUnequip);
			}
			break;

		}
		case EEquipType::CrossBow:
		{
			if (ActivatedEquipment == EEquipType::Travel)
			{
				// Equip
				this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_CBEquip);
			}
			else if (ActivatedEquipment == CurrentEquipment)
			{
				// Unequip
				this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_CBUnequip);
			}
		}
		}
		return true;
	}

}

void UHero_AnimInstance::OnSwapBetweenMeleeAndRange()
{

	UHeroStatsComponent* heroStats = mCharacter->GetHeroStatsComp();
	bool ignore
		= AttackState != EAttackState::None
		|| Montage_IsPlaying(Equip_Montage)
		|| Equip_Montage == nullptr || heroStats == nullptr;

	if (ignore)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("OnSwapBetweenMeleeAndRange is ignore due to wrong attack state, running montage, null Montage or heroStats"));
		return;
	}

	
	bool success = mCharacter->GetHeroStatsComp()->OnSwapBetweenMeleeAndRange();
	if (success)
	{
		if (ActivatedEquipment == EEquipType::Travel)
		{
			bVelocityOverrideByAnim = false;
			bTryToEquip = true;
		}
		// Check the current main weapon after swap
		AGear* mainWeapon = heroStats->GetCurrentRightHandWeapon();
		
		switch (mainWeapon->GetGearType())
		{
		case EGearType::SingleHandWeapon:
			CurrentEquipment = EEquipType::ShieldSword;
			this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_SnsEquip);
			break;
		case EGearType::DoubleHandWeapon:
			CurrentEquipment = EEquipType::TwoHandSword;
			break;
		case EGearType::CrossBow:
		{			
			CurrentEquipment = EEquipType::CrossBow;
			if (bIsFocused)
			{
				OnFocus();
			}
			this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_CBEquip);
			break;
		}
		}
		
	}


}

void UHero_AnimInstance::OnEquipWeapon()
{
	ActivatedEquipment = CurrentEquipment;
	mCharacter->GetHeroStatsComp()->OnEquipWeapon();
	mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
}

void UHero_AnimInstance::OnSheathWeapon()
{
	ActivatedEquipment = EEquipType::Travel;
	mCharacter->GetHeroStatsComp()->OnSheathWeapon();

	// After we sheath our weapon, and the sprint button is not release, we will restore sprinting
	if (bTryToSprint)
	{
		mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetSprintSpeed();
		bIsSprinting = true;
	}
	mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	mCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;

}

/** Skip equipAnimation direction to equip mode, change the movement role to strafe*/
void UHero_AnimInstance::SkipEquip()
{
	OnEquipWeapon();
	if (bIsSprinting)
	{
		// Once Attack, we activate combat mode, hence we have to slow our speed down
		// if we are sprinting then stop
		mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetJogSpeed();;
		bIsSprinting = false;
	}
	bVelocityOverrideByAnim = false;
	bTryToJump = true;
	DisableJump();
}

void UHero_AnimInstance::AttachWeapon()
{
	ActivatedEquipment = CurrentEquipment;
	mCharacter->GetHeroStatsComp()->OnEquipWeapon();

	if (bIsSprinting)
	{
		// Once Attack, we activate combat mode, hence we have to slow our speed down
		// if we are sprinting then stop
		mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetJogSpeed();;
		bIsSprinting = false;
	}
	bVelocityOverrideByAnim = false;
	DisableJump();

}


#pragma endregion

#pragma region Jump

void UHero_AnimInstance::EnableJump()
{
	bEnableJump = true;
}

void UHero_AnimInstance::DisableJump()
{
	bEnableJump = false;
}

void UHero_AnimInstance::OnJumpStart()
{
	if (bEnableJump)
	{
		if (AttackState == EAttackState::None)
		{
			bTryToJump = true;
			mCharacter->Jump();
		}
	}
}

void UHero_AnimInstance::OnJumpStop()
{
	bTryToJump = false;
	mCharacter->StopJumping();
}

#pragma endregion

#pragma region Attack
void UHero_AnimInstance::OnAttack()
{

	switch (CurrentEquipment)
	{
	case EEquipType::CrossBow:
		OnRangeAttack();
		break;
	case EEquipType::ShieldSword:
	case EEquipType::TwoHandSword:
		OnMeleeAttack();
		break;
	default:
		break;
	}

}

void UHero_AnimInstance::OnMeleeAttack()
{
	bool ignore = Montage_IsPlaying(Hit_Montage) || Attack_Montage == nullptr || bIsInAir;

	// Apply Input Filter
	if (ignore)
	{
		UE_LOG(LogTemp, Warning, TEXT("UHero_AnimInstance::OnAttack"));
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

void UHero_AnimInstance::OnRangeAttack()
{
	// Condition Check
	bool ignore = Montage_IsPlaying(Hit_Montage) ||
		Montage_IsPlaying(Fire_Montage) || 
		Fire_Montage == nullptr || bIsInAir || mCharacter == nullptr;

	if (ignore)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fire Ignore - URangeHero_AnimInstance::OnAttack"));
		return;
	}

	// If we are unarmed, equip our weapon
	if (ActivatedEquipment == EEquipType::Travel)
	{
		OnEquip();
		return;
	}

	UHeroStatsComponent* heroStats = mCharacter->GetHeroStatsComp();
	if (heroStats)
	{
		ARangeWeapon* rangeWeapon = Cast<ARangeWeapon>(heroStats->GetCurrentRightHandWeapon());

		if (rangeWeapon)
		{
			rangeWeapon->Fire();
			PlayMontage(Fire_Montage, 1.0f, MONTAGE_SN_HERO_FireOnce);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Current Weapon is not a range weapon - UHero_AnimInstance::OnRangeAttack"));
	}

}


#pragma region Fire

void UHero_AnimInstance::OnZoomIn()
{
	if (ActivatedEquipment == EEquipType::Travel)
	{
		OnEquip();
	}

	bTryToZoomIn = true;
}

void UHero_AnimInstance::OnZoomOut()
{
	if (ActivatedEquipment == EEquipType::Travel)
		return;

	bTryToZoomIn = false;
}

#pragma endregion

#pragma region  Combo Attack

void UHero_AnimInstance::LaunchCombo()
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

void UHero_AnimInstance::ResetCombo()
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

void UHero_AnimInstance::OnEnableDamage(bool bIsright, bool bIsAll)
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

void UHero_AnimInstance::OnDisableDamage(bool bIsright, bool bIsAll)
{
	//Super::OnDisableWeapon(bIsright, bIsAll);
	mCharacter->GetHeroStatsComp()->SetEnableWeapon(false, bIsright, bIsAll);

}

void UHero_AnimInstance::OnNextAttack()
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

#pragma endregion

#pragma region Dodge

void UHero_AnimInstance::OnDodge()
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
void UHero_AnimInstance::LaunchDodge()
{
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
void UHero_AnimInstance::OnDodgePost()
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
void UHero_AnimInstance::OnDodgeFinish(bool _bInterruptted)
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
FVector UHero_AnimInstance::GetFocusDodgeDirection() const
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

void UHero_AnimInstance::OnBeingHit(float _damage, FName boneName, const FVector & _shotFromDirection, const FVector & _hitLocation, const UPawnStatsComponent * _pawnStats)
{

	if (Hit_Montage)
	{
		// reset the attack state and movement override, cuz we are being attack
		ResetOnBeingHit();

		// relative position of damage causer
		FVector damageCauserRelative = _shotFromDirection;
		damageCauserRelative.Z = 0.0f;
		damageCauserRelative = damageCauserRelative.GetUnsafeNormal();



		bool HitFromFront = FVector::DotProduct(mCharacter->GetActorForwardVector(), damageCauserRelative) > 0.3f;
		FName sectionName;

		if (HitFromFront)
		{
			FVector hitRelative = _hitLocation - GetSkelMeshComponent()->GetSocketLocation(boneName);
			float hitZOffset = hitRelative.Z;
			hitRelative.Z = 0;
			hitRelative = hitRelative.GetSafeNormal();
			float vert = FVector::DotProduct(mCharacter->GetActorForwardVector(), hitRelative);
			if (vert >= 0.7f)
			{
				if (boneName.Compare("neck_01") == 0)
					sectionName = TEXT("HitHead");
				else
					sectionName = TEXT("HitCenter");
			}
			else
			{
				float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), hitRelative);
				if (hor > 0)
					sectionName = TEXT("HitRight");
				else
					sectionName = TEXT("HitLeft");
			}

			damageMomentum = - MomentumRatioByActor * damageCauserRelative - (1 - MomentumRatioByActor) * hitRelative;
			damageMomentum = damageMomentum.GetUnsafeNormal();
		}
		else
		{
			sectionName = TEXT("HitBack");
			damageMomentum = -damageCauserRelative;
		}

		UE_LOG(LogTemp, Log, TEXT("%f, %f, %f"), damageMomentum.X, damageMomentum.Y, damageMomentum.Z);

		this->PlayMontage(Hit_Montage, 1.0f, sectionName);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Hit_Montage is nullptr - UHero_AnimInstance::OnBeingHit"));
}

void UHero_AnimInstance::ResetOnBeingHit()
{
	mCharacter->bIsMovementDisabled = true;
	AttackState = EAttackState::BeingHit;
	NextAction = EActionType::None;

	bVelocityOverrideByAnim = true;
 
	if (ActivatedEquipment == EEquipType::Travel)
	{
		AttachWeapon(); 
	}
	mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;

	CurrentComboIndex = 0;
	OnDisableDamage(false, true);


}

void UHero_AnimInstance::RecoverFromBeingHit(bool _bInterrupted)
{
	if (_bInterrupted)
	{
		// Take a other hit, cuz only hit can interrupt hit
		return;
	}


	AttackState = EAttackState::None;
	bVelocityOverrideByAnim = false;	
	mCharacter->bIsMovementDisabled = false;

	mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
}

void UHero_AnimInstance::HeadTrack()
{
	
	// Calculate the headTrack yaw and pitch
	FRotator actorRotation = mCharacter->GetActorRotation();
	FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(mCharacter->GetControlRotation(), actorRotation);

	float headTrack_yaw_target = delta.Yaw;
	float headTrack_pitch_target = delta.Pitch;
	if (headTrack_yaw_target >= 135.0f)
	{
		if (headTrack_yaw_target < 90)
			headTrack_yaw_target = -headTrack_yaw_target;
		else
			headTrack_yaw_target = headTrack_yaw_target - 180.0f;

		headTrack_pitch_target = -headTrack_pitch_target;
	}
	else if (headTrack_yaw_target <= -135.0f)
	{
		if (headTrack_yaw_target > -90)
			headTrack_yaw_target = -headTrack_yaw_target;
		else
			headTrack_yaw_target = headTrack_yaw_target + 180.0f;

		headTrack_pitch_target = -headTrack_pitch_target;
	}

	HeadTrackYaw = FMath::FInterpTo(HeadTrackYaw, headTrack_yaw_target, GetWorld()->DeltaTimeSeconds, HeadTrackRate);
	HeadTrackPitch = FMath::FInterpTo(HeadTrackPitch, headTrack_pitch_target, GetWorld()->DeltaTimeSeconds, HeadTrackRate);

}

