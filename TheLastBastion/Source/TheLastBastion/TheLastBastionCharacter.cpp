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
#include "Engine.h"

//////////////////////////////////////////////////////////////////////////
// ATheLastBastionCharacter

ATheLastBastionCharacter::ATheLastBastionCharacter()
{	
	CharacterType = ECharacterType::None;

	SprintSpeed = 850.0f;
	JogSpeed = 595.0f;
	walkSpeed = 255.0f;
	minTurnRate_Travel = 180.0f;
	maxTurnRate_Travel = 630.0f;
	minTurnRate_Combat = 360.0f;

	maxTurnRate_Combat = 1440.0f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->MaxWalkSpeed = JogSpeed; // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 1500.0f;
	GetCharacterMovement()->AirControl = 0.2f;


	// Init capsule size of each situations
	CapHalfSize = 90.0f;
	CapRadius = 34.0f;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(CapRadius, CapHalfSize);
	GetMesh()->RelativeLocation = FVector(0, 0, -CapHalfSize);
	GetMesh()->RelativeRotation = FRotator(0, -90, 0);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		
	CreateStatsComponents();
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void ATheLastBastionCharacter::CreateStatsComponents()
{

	Head = CreateDefaultSubobject<USphereComponent>(TEXT("Head"));
	Head->SetupAttachment(GetMesh(), TEXT("head"));
	Head->InitSphereRadius(12);
	Head->RelativeLocation = FVector(5, 2.5f, 0);
	Head->bGenerateOverlapEvents = true;
	Head->SetCanEverAffectNavigation(false);

	Body = CreateDefaultSubobject<UBoxComponent>(TEXT("Body"));
	Body->SetupAttachment(GetMesh(), TEXT("spine_02"));
	Body->InitBoxExtent(FVector(40, 15, 25));
	Body->RelativeLocation = FVector(-10, 0, 0);
	Body->bGenerateOverlapEvents = true;
	Body->SetCanEverAffectNavigation(false);

}

void ATheLastBastionCharacter::BeginPlay()
{
	Super::BeginPlay();

}

