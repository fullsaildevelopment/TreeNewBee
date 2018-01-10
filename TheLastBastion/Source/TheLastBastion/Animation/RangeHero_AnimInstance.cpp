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


URangeHero_AnimInstance::URangeHero_AnimInstance(const FObjectInitializer& _objectInitalizer) : Super(_objectInitalizer)
{
	CurrentEquipment = EEquipType::CrossBow;
	CameraZoomInRate = 10.0f;
	CameraShiftRate = 3.0f;
	bTryToZoomIn = false;
	ZoomedFOV = 65.0f;
	CameraRelativeLocation = FVector(100, 50, 0);
}


void URangeHero_AnimInstance::OnBeginPlay()
{
	Super::OnBeginPlay();

	// Store the camera default field of view
	if (mCharacter)
	{
		UCameraComponent* CameraComp = mCharacter->GetFollowCamera();
		if (CameraComp)
		{
			DefalutFOV = CameraComp->FieldOfView;
		}
	}
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
			mCharacter->GetFollowCamera()->RelativeLocation
				= UKismetMathLibrary::VInterpTo(mCharacter->GetFollowCamera()->RelativeLocation, CameraRelativeLocation, _deltaTime, CameraShiftRate);
			// Camera Zoom
			ZoomInCamera(_deltaTime);
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
	if (mCharacter)
	{
		UHeroStatsComponent* heroStats = mCharacter->GetHeroStatsComp();
		if (heroStats)
		{
			ARangeWeapon* rangeWeapon = Cast<ARangeWeapon>(heroStats->GetRightHandWeapon());

			if (rangeWeapon)
				rangeWeapon->Fire();
			else
				UE_LOG(LogTemp, Error, TEXT("RangeWeapon Is Null -- URangeHero_AnimInstance::OnAttack"));
		}
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
		return;
	bTryToZoomIn = true;
}

void URangeHero_AnimInstance::OnRightMouseButtonReleased()
{
	if (ActivatedEquipment == EEquipType::Travel)
		return;

	bTryToZoomIn = false;
}

void URangeHero_AnimInstance::ZoomInCamera(float DeltaTime)
{
	// Update Camera FOV smoothly
	float TargetFOV = bTryToZoomIn ? ZoomedFOV : DefalutFOV;

	UCameraComponent* CameraComp = mCharacter->GetFollowCamera();
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, CameraZoomInRate);

	CameraComp->SetFieldOfView(NewFOV);
}


//void URangeHero_AnimInstance::LerpCameraToTravelPosition(float _deltaTime)
//{
//
//
//	//float delta = cameraShiftRate * _deltaTime;
//	//mCharacter->GetCameraBoom()->RelativeLocation.X += 2 * delta;
//	//mCharacter->GetCameraBoom()->RelativeLocation.Y += delta;
//}
