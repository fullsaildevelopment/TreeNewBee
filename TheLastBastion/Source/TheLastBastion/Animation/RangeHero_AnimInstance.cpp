// Fill out your copyright notice in the Description page of Project Settings.

#include "RangeHero_AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "TheLastBastionHeroCharacter.h"
#include "Combat/HeroStatsComponent.h"
#include "Combat/RangeWeapon.h"

#define MontageSection_Equip TEXT("EquipHeroCB")
#define MontageSection_Unequip TEXT("UnEquipHeroCB")
#define MontageSection_FireOnce TEXT("FireOnce")

URangeHero_AnimInstance::URangeHero_AnimInstance(const FObjectInitializer& _objectInitalizer) : Super(_objectInitalizer)
{
	CurrentEquipment = EEquipType::CrossBow;
	CameraZoomInRate = 10.0f;
	CameraShiftRate = 3.0f;
	bTryToZoomIn = false;

	CameraEquipOffset = FVector(100, 50, 0);

	CameraZoomOffset = FVector(200, 50, 10);
}


void URangeHero_AnimInstance::OnBeginPlay()
{
	Super::OnBeginPlay();
}

void URangeHero_AnimInstance::OnInit()
{
	Super::OnInit();
}

void URangeHero_AnimInstance::OnUpdate(float _deltaTime)
{
	Super::OnUpdate(_deltaTime);

	if (mCharacter)
	{   
		UCharacterMovementComponent* movementComp = mCharacter->GetCharacterMovement();

		if (ActivatedEquipment == EEquipType::Travel)
		{
			mAccelerationDirection = movementComp->GetCurrentAcceleration().GetSafeNormal();
			mCharacter->GetFollowCamera()->RelativeLocation
				= UKismetMathLibrary::VInterpTo(mCharacter->GetFollowCamera()->RelativeLocation, FVector::ZeroVector, _deltaTime, CameraShiftRate);
		}
		else
		{
			mAccelerationDirection = UKismetMathLibrary::GetForwardVector(mCharacter->GetControlRotation());

			if (bTryToZoomIn)
			{
				mCharacter->GetFollowCamera()->RelativeLocation
					= UKismetMathLibrary::VInterpTo(mCharacter->GetFollowCamera()->RelativeLocation, CameraZoomOffset, _deltaTime, CameraZoomInRate);

				// walk in zoom in mode
				if (!bTryToSprint)
				{
					MoveForwardAxis *= 0.5f;
					MoveRightAxis  *= 0.5f;
					movementComp->MaxWalkSpeed = mCharacter->GetWalkSpeed();
				}
				else
					movementComp->MaxWalkSpeed = mCharacter->GetJogSpeed();

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


		Acceleration_bodySpace
			= UKismetMathLibrary::InverseTransformDirection(mCharacter->GetTransform(), mAccelerationDirection);

		turn = FMath::RadiansToDegrees(FMath::Atan2(Acceleration_bodySpace.Y, Acceleration_bodySpace.X));


		if (bVelocityOverrideByAnim)
		{
			float Z = movementComp->Velocity.Z;
			FVector overrideVelocity;
			overrideVelocity = mCharacter->GetActorForwardVector() * GetCurveValue("Speed");
			movementComp->Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, Z);
		}
	}

}

void URangeHero_AnimInstance::OnPostEvaluate()
{
	Super::OnPostEvaluate();
}

void URangeHero_AnimInstance::OnAttack()
{
	// Condition Check
	bool ignore = Montage_IsPlaying(Fire_Montage) || Fire_Montage == nullptr || bIsInAir || mCharacter == nullptr;

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
		ARangeWeapon* rangeWeapon = Cast<ARangeWeapon>(heroStats->GetRightHandWeapon());

		if (rangeWeapon)
		{
			rangeWeapon->Fire();
			PlayMontage(Fire_Montage, 1.0f, MontageSection_FireOnce);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("RangeWeapon Is Null -- URangeHero_AnimInstance::OnAttack"));
	}
}

bool URangeHero_AnimInstance::OnEquip()
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
			bRotationRateOverrideByAnim = false;
		}



		// Player Animation Montage based on activated equip type
		if (ActivatedEquipment == EEquipType::Travel)
		{
			// Equip
			this->PlayMontage(Equip_Montage, 1.0f, MontageSection_Equip);

		}
		else if (ActivatedEquipment == CurrentEquipment)
		{
			// Unequip
			this->PlayMontage(Equip_Montage, 1.0f, MontageSection_Unequip);
		}
		return true;
	}
	else
		return false;

}

void URangeHero_AnimInstance::OnRightMouseButtonPressed()
{
	if (ActivatedEquipment == EEquipType::Travel)
	{
		OnEquip();
	}
	bTryToZoomIn = true;
}

void URangeHero_AnimInstance::OnRightMouseButtonReleased()
{
	if (ActivatedEquipment == EEquipType::Travel)
		return;

	bTryToZoomIn = false;
}


