// Fill out your copyright notice in the Description page of Project Settings.

#include "Hero_AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TheLastBastionHeroCharacter.h"
#include "Combat/HeroStatsComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "Engine.h"

#pragma region Class Base Interface

UHero_AnimInstance::UHero_AnimInstance(const FObjectInitializer& _objectInitalizer) :Super(_objectInitalizer)
{
	HeadTrackRate = 3.0f;
	ActivatedEquipment = EEquipType::Travel;
	AttackState = EAttackState::None;
	bTryToSprint = false;
	bTryToMove = false;
	bIsSprinting = false;
	MomentumRatioByActor = 0.7f;
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

}

void UHero_AnimInstance::OnInit()
{
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

		// Head Track
		HeadTrack();
	}
}

void UHero_AnimInstance::OnPostEvaluate()
{
}

#pragma endregion


void UHero_AnimInstance::OnMiddleMouseButtonPressed()
{
	// Handled by child class
}

void UHero_AnimInstance::OnCorLAltPressed()
{
}

void UHero_AnimInstance::OnRightMouseButtonPressed()
{
}

void UHero_AnimInstance::OnRightMouseButtonReleased()
{
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
	if (ActivatedEquipment == EEquipType::Travel && !Montage_IsPlaying(Equip_Montage))
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


#pragma region Equip && Unequip

void UHero_AnimInstance::OnAttack()
{
	// Let child class handle
}

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
		return true;
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
	//UE_LOG(LogTemp, Warning, TEXT("DisableJump"));
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

		this->PlayMontage(Hit_Montage, 1.0f, sectionName);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Hit_Montage is nullptr - UHero_AnimInstance::OnBeingHit"));
}

void UHero_AnimInstance::ResetOnBeingHit()
{
	mCharacter->bIsMovementEnabled = false;
	AttackState = EAttackState::BeingHit;
	NextAction = EActionType::None;

	bVelocityOverrideByAnim = true;
 
	if (ActivatedEquipment == EEquipType::Travel)
	{
		AttachWeapon(); 
	}
	mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;

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
	mCharacter->bIsMovementEnabled = true;

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

