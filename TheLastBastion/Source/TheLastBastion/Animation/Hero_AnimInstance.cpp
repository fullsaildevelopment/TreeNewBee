// Fill out your copyright notice in the Description page of Project Settings.

#include "Hero_AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TheLastBastionHeroCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Combat/PawnStatsComponent.h"
#include "Engine.h"


UHero_AnimInstance::UHero_AnimInstance(const FObjectInitializer& _objectInitalizer) :Super(_objectInitalizer)
{
	HeadTrackRate = 3.0f;
	ActivatedEquipment = EEquipType::Travel;
	AttackState = EAttackState::None;
	bTryToAttack = false;
	bTryToSprint = false;
	bIsSprinting = false;
	bRotationRateOverrideByAnim = false;
	bSpeedOverrideByAnim = false;

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
		UCharacterMovementComponent* movementComp = mCharacter->GetCharacterMovement();

		// Check if the play is falling?
		bIsInAir = movementComp->IsFalling();

		// Check if the player try to move?
		bTryToMove = movementComp->GetCurrentAcceleration().SizeSquared() > 0;

		currentSpeed = movementComp->Velocity.Size();

		FVector acceleration = movementComp->GetCurrentAcceleration();
		acceleration.Normalize();
		Acceleration_bodySpace
			= UKismetMathLibrary::InverseTransformDirection(mCharacter->GetTransform(), acceleration);
		
		turn = FMath::RadiansToDegrees(FMath::Atan2(Acceleration_bodySpace.Y, Acceleration_bodySpace.X));

		// the more of the angle between forward vector and acceleration, the more rotation speed
		if (!bRotationRateOverrideByAnim)
		{
			movementComp->RotationRate.Yaw
				= UKismetMathLibrary::MapRangeClamped(FMath::Abs(turn), 0, 180.0f, mCharacter->GetMinTurnRate(), mCharacter->GetMaxTurnRate());
		}

		if (bSpeedOverrideByAnim)
		{
			// we restore the velocity from gravity, and override the x,y component
			float Z = movementComp->Velocity.Z;
			FVector overrideVelocity = mCharacter->GetActorForwardVector() * GetCurveValue("Speed");
			movementComp->Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, Z);
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

void UHero_AnimInstance::StartOverrideSpeed()
{
	bSpeedOverrideByAnim = true;
}

void UHero_AnimInstance::StopOverrideSpeed()
{
	bSpeedOverrideByAnim = false;
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

void UHero_AnimInstance::OnEnableWeapon(bool _bIsright, bool _bIsAll)
{
	AttackState = EAttackState::Attacking;
	mCharacter->GetPawnStatsComp()-> EnableWeapon(_bIsright, _bIsAll);
}

void UHero_AnimInstance::OnDisableWeapon(bool _bIsright, bool _bIsAll)
{
	mCharacter->GetPawnStatsComp()->DisableWeapon(_bIsright, _bIsAll);
}

void UHero_AnimInstance::OnNextAttack()
{
	AttackState = EAttackState::ReadyForNext;
}

void UHero_AnimInstance::OnResetCombo()
{
	AttackState = EAttackState::None;
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

void UHero_AnimInstance::OnAttack()
{
	// when attack during travel mode
	if (ActivatedEquipment == EEquipType::Travel)
	{
		// switch to attck 
		ActivatedEquipment = CurrentEquipment;
		// attach weapon 
		mCharacter->GetPawnStatsComp()->OnEquipWeapon();
	}

	bTryToAttack = true;

	// Once Attack, we activate combat mode, hence we have to slow our speed down
	// if we are sprinting then stop

	if (bIsSprinting)
	{
		mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetJogSpeed();;
		bIsSprinting = false;
	}
	// Increase the turn rate for flexible control
	mCharacter->SetMaxTurnRate(mCharacter->GetMaxTurnRateForCombat());
	UE_LOG(LogTemp, Warning, TEXT("Attack ! - UHero_AnimInstance"));
}

void UHero_AnimInstance::OnEquip()
{
	if (ActivatedEquipment != CurrentEquipment)
	{
		// Equip
		ActivatedEquipment = CurrentEquipment;
		mCharacter->SetMaxTurnRate(mCharacter->GetMaxTurnRateForCombat());
		// if we are sprinting then stop
		if (bIsSprinting)
		{
			mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetJogSpeed();;
			bIsSprinting = false;
		}
	}
	else
	{
		// Unequip
		ActivatedEquipment = EEquipType::Travel;
		mCharacter->SetMaxTurnRate(mCharacter->GetMaxTurnRateForTravel());
		// After we sheath our weapon, and the sprint button is not release, we will restore sprinting
		if (bTryToSprint)
		{
			mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetSprintSpeed();
			bIsSprinting = true;
		}

	}

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
	mCharacter->GetPawnStatsComp()->OnEquipWeapon();
}

void UHero_AnimInstance::OnSheathWeapon()
{
	mCharacter->GetPawnStatsComp()->OnSheathWeapon();

}

void UHero_AnimInstance::OnBeingHit(const class AActor* const _attacker)
{


	//bool bNoHitAnimIsPlaying = this->Montage_IsActive(Hit_Montage) == false;
	//if (bNoHitAnimIsPlaying)
	//{
	//}

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

void UHero_AnimInstance::OnComboInterrupt()
{
	bSpeedOverrideByAnim = false;
	bRotationRateOverrideByAnim = false;
	AttackState = EAttackState::None;
}

void UHero_AnimInstance::SetTryToSprint(bool _val)
{
	bTryToSprint = _val;
}

void UHero_AnimInstance::OnSprintPressed()
{
	bTryToSprint = true;

	bool ableToSprint
		= !bSpeedOverrideByAnim &&
		ActivatedEquipment == EEquipType::Travel;

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
