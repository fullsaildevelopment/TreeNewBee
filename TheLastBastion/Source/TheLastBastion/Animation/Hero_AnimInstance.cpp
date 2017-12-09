// Fill out your copyright notice in the Description page of Project Settings.

#include "Hero_AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TheLastBastionCharacter.h"

void UHero_AnimInstance::OnBeginPlay()
{
	APawn* pawn = TryGetPawnOwner();
	ATheLastBastionCharacter* hero = Cast<ATheLastBastionCharacter>(pawn);
	if (hero == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("The MeleeHero can only assigned to ATheLastBastionCharacter - UMeleeHero_AnimInstance "));
		return;
	}
	mMovementComp = hero->GetCharacterMovement();

	UE_LOG(LogTemp, Warning, TEXT("UHero_AnimInstance Call BeginPlay"));
}

void UHero_AnimInstance::OnInit()
{
}

void UHero_AnimInstance::OnUpdate(float _deltaTime)
{
	ensure(mMovementComp != nullptr);
	float maxSpeed = 0, rotationSpeed = 0;

	if (!GetCurveValue("Speed"), maxSpeed) {
		UE_LOG(LogTemp, Warning, TEXT("UHero_AnimInstance Unable to get value from Curve Speed"));
	}
	if (!GetCurveValue("Rotation"), rotationSpeed) {
		UE_LOG(LogTemp, Warning, TEXT("UHero_AnimInstance Unable to get value from Curve Speed"));
	}


	mMovementComp->MaxWalkSpeed = maxSpeed * GetOwningComponent()->GetPlayRate();
	mMovementComp->RotationRate.Yaw = rotationSpeed * GetOwningComponent()->GetPlayRate();


	
}

void UHero_AnimInstance::OnPostEvaluate()
{
}
