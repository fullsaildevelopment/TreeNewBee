// Fill out your copyright notice in the Description page of Project Settings.

#include "Hero_AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TheLastBastionCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine.h"

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
	activatedStateMachine = 0;
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
			//movementComp->MaxWalkSpeed = GetCurveValue("Speed");
			movementComp->Velocity = mCharacter->GetActorForwardVector() * GetCurveValue("Speed");

		if (bRotationRateOverrideByAnim)
			movementComp->RotationRate.Yaw = GetCurveValue("Rotation");

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

void UHero_AnimInstance::OnAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack ! - UHero_AnimInstance"));
}
