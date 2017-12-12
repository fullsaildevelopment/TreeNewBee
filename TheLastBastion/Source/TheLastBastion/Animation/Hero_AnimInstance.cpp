// Fill out your copyright notice in the Description page of Project Settings.

#include "Hero_AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TheLastBastionCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine.h"


UHero_AnimInstance::UHero_AnimInstance(const FObjectInitializer& _objectInitalizer) :Super(_objectInitalizer)
{
	headTrackRate = 3.0f;
	activatedEquipment = EEquipType::Travel;
}

void UHero_AnimInstance::OnBeginPlay()
{
	APawn* pawn = TryGetPawnOwner();
	mCharacter = Cast<ATheLastBastionCharacter>(pawn);
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
		acceleration_bodySpace
			= UKismetMathLibrary::InverseTransformDirection(mCharacter->GetTransform(), acceleration);
		
		turn = FMath::RadiansToDegrees(FMath::Atan2(acceleration_bodySpace.Y, acceleration_bodySpace.X));

		// the more of the angle between forward vector and acceleration, the more rotation speed
		movementComp->RotationRate.Yaw 
			= UKismetMathLibrary::MapRangeClamped(FMath::Abs(turn), 0, 180.0f, mCharacter->GetMinTurnRate(), mCharacter->GetMaxTurnRate());

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
		= (mCharacter->IsSprinting())? mCharacter->GetSprintSpeed() : mCharacter->GetJogSpeed();
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

void UHero_AnimInstance::OnAttack()
{
	// Travel mode to combat mode on attack
	if (activatedEquipment == EEquipType::Travel)
		activatedEquipment = currentEquipment;

	UE_LOG(LogTemp, Warning, TEXT("Attack ! - UHero_AnimInstance"));
}

void UHero_AnimInstance::OnEquip()
{
	if (activatedEquipment != currentEquipment)
		// Equip
		activatedEquipment = currentEquipment;
	else
		// Unequip
		activatedEquipment = EEquipType::Travel;
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

	headTrackYaw = FMath::FInterpTo(headTrackYaw, headTrack_yaw_target, GetWorld()->DeltaTimeSeconds, headTrackRate);
	headTrackPitch = FMath::FInterpTo(headTrackPitch, headTrack_pitch_target, GetWorld()->DeltaTimeSeconds, headTrackRate);

}
