// Fill out your copyright notice in the Description page of Project Settings.

#include "Hero_AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TheLastBastionHeroCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Combat/HeroStatsComponent.h"
#include "Engine.h"
#include "GameFramework/SpringArmComponent.h"


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
			case EAttackState::Dodging:
			case EAttackState::PostDodging:
				overrideVelocity = mSpeedOverrideDirection * GetCurveValue("Speed");
				//UE_LOG(LogTemp, Warning, TEXT("Dodge Direction x: %f, y: %f, z: %f"),
				//	mSpeedOverrideDirection.X, mSpeedOverrideDirection.Y, mSpeedOverrideDirection.Z);
				movementComp->Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, Z);
				break;
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



bool UHero_AnimInstance::OnAttack()
{
	bool ignore = Attack_Montage == nullptr;

	// Apply Input Filter
	if (!ignore)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack ! - UHero_AnimInstance"));
		// when attack during travel mode
		if (ActivatedEquipment == EEquipType::Travel)
		{
			// switch to attack 
			ActivatedEquipment = CurrentEquipment;
			// attach weapon 
			mCharacter->GetHeroStatsComp()->OnEquipWeapon();

			mCharacter->GetCameraBoom()->bEnableCameraRotationLag = true;
			if (bIsSprinting)
			{
				// Once Attack, we activate combat mode, hence we have to slow our speed down
				// if we are sprinting then stop
				mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetJogSpeed();;
				bIsSprinting = false;
			}
		}
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


#pragma region Dodge Event Start, Init, Combination, Finish
bool UHero_AnimInstance::OnDodge()
{
	// Apply Input Filter
	bool ignore
		= FMath::Abs(turn) > DodgeMinTurnThreshold ||
		bIsInAir || Dodge_Montage == nullptr;

	if (ignore)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Dodge is ignored due to inAir, turn %f, travel or Montage Null"), turn);
	}
	return !ignore;
}

void UHero_AnimInstance::LaunchDodge()
{
	UE_LOG(LogTemp, Warning, TEXT("Dodge"));

	// Update State
	AttackState = EAttackState::Dodging;
	// Clear Next Action Marker
	NextAction = EActionType::None;

	bRotationRateOverrideByAnim = true;
	mCharacter->GetCharacterMovement()->RotationRate.Yaw = 0;

	bVelocityOverrideByAnim = true;

	if (!bIsFocused)
	{
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

void UHero_AnimInstance::OnEquip()
{
	if (ActivatedEquipment != CurrentEquipment)
	{
		// Equip
		ActivatedEquipment = CurrentEquipment;
		// if we are sprinting then stop
		if (bIsSprinting)
		{
			mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetJogSpeed();;
			bIsSprinting = false;
		}
		mCharacter->GetCameraBoom()->bEnableCameraRotationLag = true;
	}
	else
	{
		if (bIsFocused)
		{
			OnFocus();
			UE_LOG(LogTemp, Warning, TEXT("Unequip during focus, unfocus is implemented automatically"));
		}
		// Unequip
		ActivatedEquipment = EEquipType::Travel;
		mCharacter->GetCameraBoom()->bEnableCameraRotationLag = false;
		// After we sheath our weapon, and the sprint button is not release, we will restore sprinting
		if (bTryToSprint)
		{
			mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetSprintSpeed();
			bIsSprinting = true;
		}
	}
	bVelocityOverrideByAnim = false;

}

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

void UHero_AnimInstance::OnEquipWeapon()
{
	mCharacter->GetHeroStatsComp()->OnEquipWeapon();
}

void UHero_AnimInstance::OnSheathWeapon()
{
	mCharacter->GetHeroStatsComp()->OnSheathWeapon();

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
