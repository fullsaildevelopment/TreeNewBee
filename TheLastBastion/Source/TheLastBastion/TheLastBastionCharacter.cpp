// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "TheLastBastionCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"

#include "Components/CapsuleComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Combat/PawnStatsComponent.h"
#include "Engine.h"

//////////////////////////////////////////////////////////////////////////
// ATheLastBastionCharacter

ATheLastBastionCharacter::ATheLastBastionCharacter()
{	
	SprintSpeed = 850.0f;
	JogSpeed = 595.0f;
	walkSpeed = 255.0f;
	minTurnRate = 180.0f;
	maxTurnRate_Travel = 630.0f;
	maxTurnRate_Combat = 1080.0f;
	maxTurnRate = maxTurnRate_Travel;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->MaxWalkSpeed = JogSpeed; // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;


	// Init capsule size of each situations
	CapHalfSize = 90.0f;
	CapRadius = 34.0f;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(CapRadius, CapHalfSize);
	GetMesh()->RelativeLocation = FVector(0, 0, -CapRadius);
	
	PawnStats = CreateDefaultSubobject<UPawnStatsComponent>(TEXT("Stats"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}


void ATheLastBastionCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->RotationRate = FRotator(0.0f, maxTurnRate, 0.0f); 	
}

