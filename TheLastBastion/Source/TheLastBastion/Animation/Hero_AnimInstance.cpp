// Fill out your copyright notice in the Description page of Project Settings.

#include "Hero_AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TheLastBastionHeroCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Combat/HeroStatsComponent.h"
#include "Engine.h"


UHero_AnimInstance::UHero_AnimInstance(const FObjectInitializer& _objectInitalizer) :Super(_objectInitalizer)
{
	HeadTrackRate = 3.0f;
	DodgeMinTurnThreshold = 1.0f;
	ActivatedEquipment = EEquipType::Travel;
	AttackState = EAttackState::None;
	bTryToSprint = false;
	bTryToMove = false;
	bIsSprinting = false;
	bIsFocused = false;

	bRotationRateOverrideByAnim = false;
	bVelocityOverrideByAnim = false;
	FocusDodgeDirection = EFocusDodgeDirection::None;
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
	UE_LOG(LogTemp, Warning, TEXT("UHero_AnimInstance Call OnBeginPlay"));

}

void UHero_AnimInstance::OnInit()
{
	//activatedEquipment = EEquipType::Travel;
	//UE_LOG(LogTemp, Warning, TEXT("UHero_AnimInstance Call OnInit"));
}

void UHero_AnimInstance::OnUpdate(float _deltaTime)
{

	if (mCharacter != nullptr)
	{
		// Get Control Axis for strafing
		MoveForwardAxis = mCharacter->GetMoveForwardAxis();
		MoveRightAxis = mCharacter->GetMoveRightAxis();

		UCharacterMovementComponent* movementComp = mCharacter->GetCharacterMovement();

		// Check if the play is falling?
		bIsInAir = movementComp->IsFalling();

		// Check if the player try to move?
		bTryToMove = movementComp->GetCurrentAcceleration().SizeSquared() > 0;

		currentSpeed = movementComp->Velocity.Size();

		mAccelerationDirection = movementComp->GetCurrentAcceleration();
		mAccelerationDirection.Normalize();
		Acceleration_bodySpace
			= UKismetMathLibrary::InverseTransformDirection(mCharacter->GetTransform(), mAccelerationDirection);
		
		turn = FMath::RadiansToDegrees(FMath::Atan2(Acceleration_bodySpace.Y, Acceleration_bodySpace.X));

		if (!bRotationRateOverrideByAnim)
		{
			// the more of the angle between forward vector and acceleration, the more rotation speed

			if (ActivatedEquipment == EEquipType::Travel)
				movementComp->RotationRate.Yaw
				= UKismetMathLibrary::MapRangeClamped(FMath::Abs(turn), 0, 180.0f, 
					mCharacter->GetMinTurnRateForTravel(),
					mCharacter->GetMaxTurnRateForTravel());
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
		}

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

			case EAttackState::BeAttacked:		
				break;
			case EAttackState::Dodging:
			case EAttackState::PostDodging:
			{
				if (bIsFocused)
					overrideVelocity = GetFocusDodgeDirection() * GetCurveValue("Speed");
				else
					overrideVelocity = mSpeedOverrideDirection * GetCurveValue("Speed");
				//UE_LOG(LogTemp, Warning, TEXT("Dodge Direction x: %f, y: %f, z: %f"),
				//	mSpeedOverrideDirection.X, mSpeedOverrideDirection.Y, mSpeedOverrideDirection.Z);
				movementComp->Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, Z);
				break;
			}
			}
		}
	
		// Head Track
		HeadTrack();
	}
	else
	{
	}	
}

void UHero_AnimInstance::OnPostEvaluate()
{
}

void UHero_AnimInstance::OnFocus()
{
	//Do nothing and leave to derived class to handle
}

void UHero_AnimInstance::StartOverrideSpeed()
{
	bVelocityOverrideByAnim = true;
}

void UHero_AnimInstance::StopOverrideSpeed()
{

	bool ignore = AttackState == EAttackState::Dodging || AttackState == EAttackState::BeAttacked
		|| AttackState == EAttackState::Attacking;
	if (ignore)
		return;	

	bVelocityOverrideByAnim = false;
	mCharacter->GetCharacterMovement()->MaxWalkSpeed 
		= (bIsSprinting)? mCharacter->GetSprintSpeed() : mCharacter->GetJogSpeed();
}

void UHero_AnimInstance::EnableJump()
{
	bEnableJump = true;
}

void UHero_AnimInstance::DisableJump()
{
	bEnableJump = false;
}

float UHero_AnimInstance::PlayMontage(UAnimMontage * _animMontage, float _rate, FName _startSectionName)
{
	if (_animMontage)
	{
		float const duration = this->Montage_Play(_animMontage, _rate);
		if (duration > 0.f)
		{
			if (_startSectionName != NAME_None)
			{
				this->Montage_JumpToSection(_startSectionName, _animMontage);
			}
		}
		return duration;
	}
	return 0.f;
}

#pragma region Attack && Combo
bool UHero_AnimInstance::OnAttack()
{
	bool ignore = Attack_Montage == nullptr || bIsInAir;

	// Apply Input Filter
	if (!ignore)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Attack ! - UHero_AnimInstance"));
		// when attack during travel mode
		if (ActivatedEquipment == EEquipType::Travel)
			SkipEquip();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack ignore - UHero_AnimInstance"));
	}
	return !ignore;

}

void UHero_AnimInstance::OnEnableWeapon(bool _bIsright, bool _bIsAll)
{
	AttackState = EAttackState::Attacking;
	mCharacter->GetHeroStatsComp()->EnableWeapon(_bIsright, _bIsAll);
}

void UHero_AnimInstance::OnDisableWeapon(bool _bIsright, bool _bIsAll)
{
	mCharacter->GetHeroStatsComp()->DisableWeapon(_bIsright, _bIsAll);
}

void UHero_AnimInstance::OnNextAttack()
{
	// Let derived Class to handle
	// AttackState = EAttackState::ReadyForNext;
}

void UHero_AnimInstance::OnResetCombo()
{
	AttackState = EAttackState::None;
	NextAction = EActionType::None;

}

#pragma endregion

#pragma region Dodge Event Start, Init, Combination, Finish
bool UHero_AnimInstance::OnDodge()
{
	// Apply Input Filter
	bool ignore
		= (FMath::Abs(turn) > DodgeMinTurnThreshold && !bIsFocused) ||
		bIsInAir || Dodge_Montage  == nullptr;

	if (!ignore)
	{
		if (ActivatedEquipment == EEquipType::Travel)
			SkipEquip();
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Dodge is ignored due to inAir or Montage Null"), turn);
		return false;
	}
}

void UHero_AnimInstance::LaunchDodge()
{
	UE_LOG(LogTemp, Warning, TEXT("Dodge"));

	// Update State
	AttackState = EAttackState::Dodging;
	// Clear Next Action Marker
	NextAction = EActionType::None;

	bVelocityOverrideByAnim = true;
}

void UHero_AnimInstance::OnDodgePost()
{
	// let derived class to handle
}

void UHero_AnimInstance::OnDodgeFinish()
{
	bVelocityOverrideByAnim = false;

	bRotationRateOverrideByAnim = false;
	AttackState = EAttackState::None;
}

#pragma endregion

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
		//bVelocityOverrideByAnim = false;

		if (ActivatedEquipment != CurrentEquipment)
		{
			// if we are sprinting then stop
			if (bIsSprinting)
			{
				mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetJogSpeed();;
				bIsSprinting = false;
			}
		}
		else
		{
			/** No need to be in focus mode, if we already sheath our weapon*/
			if (bIsFocused)
			{
				OnFocus();
				UE_LOG(LogTemp, Warning, TEXT("Unequip during focus, unfocus is implemented automatically"));
			}

			// Sprint recover is moved to sheathWeapon for animation refine :)
		}

		// let derived class to handle animation
		return true;
	}

}

void UHero_AnimInstance::OnEquipWeapon()
{
	ActivatedEquipment = CurrentEquipment;
	mCharacter->GetHeroStatsComp()->OnEquipWeapon();
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
}

#pragma endregion


void UHero_AnimInstance::OnJumpStart()
{
	if (bEnableJump)
	{
		bTryToJump = true;
		mCharacter->Jump();
	}
}

void UHero_AnimInstance::OnJumpStop()
{
	bTryToJump = false;
	mCharacter->StopJumping();
}

void UHero_AnimInstance::OnBeingHit(const class AActor* const _attacker)
{
	FVector forward = mCharacter->GetActorForwardVector();
	FVector right = mCharacter->GetActorRightVector();
	FVector away = _attacker->GetActorLocation() - mCharacter->GetActorLocation();
	away = away.GetUnsafeNormal();

	float vert = FVector::DotProduct(forward, away);

	FName sectionName;
	if (vert >= 0.7f)
		sectionName = TEXT("HitCenter");
	else if (vert <= -0.7f)
		sectionName = TEXT("HitBack");
	else
	{
		float hor = FVector::DotProduct(right, away);
		if (hor > 0)
			sectionName = TEXT("HitRight");
		else
			sectionName = TEXT("HitLeft");
	}

	if (Hit_Montage)
		this->PlayMontage(Hit_Montage, 1.0f, sectionName);
	else
		UE_LOG(LogTemp, Error, TEXT("Hit_Montage is nullptr - UHero_AnimInstance::OnBeingHit"));
}

void UHero_AnimInstance::OnActionInterrupt()
{
	bVelocityOverrideByAnim = false;
	bRotationRateOverrideByAnim = false;
	AttackState = EAttackState::None;
	NextAction = EActionType::None;
}

void UHero_AnimInstance::SetTryToSprint(bool _val)
{
	bTryToSprint = _val;
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

void UHero_AnimInstance::ToggleFocusMode(bool _IsOn)
{
	bIsFocused = _IsOn;
	if (bIsFocused)
		mCharacter->GetCameraBoom()->CameraRotationLagSpeed = 5.0f;
	mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = _IsOn;
	mCharacter->GetCharacterMovement()->bOrientRotationToMovement = !_IsOn;
	bIsFocusEnterPending = false;
	bIsFocusExitPending = false;
}

FVector UHero_AnimInstance::GetFocusDodgeDirection() const
{
	switch (FocusDodgeDirection)
	{
	case EFocusDodgeDirection::Forward:
		return mCharacter->GetActorForwardVector();

	case EFocusDodgeDirection::Right45:
		return (.5 * mCharacter->GetActorForwardVector() +  mCharacter->GetActorRightVector()).GetUnsafeNormal();

	case EFocusDodgeDirection::Left45:
		return (.5f *  mCharacter->GetActorForwardVector() -  mCharacter->GetActorRightVector()).GetUnsafeNormal();

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