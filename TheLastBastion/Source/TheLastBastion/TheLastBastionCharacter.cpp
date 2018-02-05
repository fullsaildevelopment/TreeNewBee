// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "TheLastBastionCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Combat/PawnStatsComponent.h"

#include "Components/CapsuleComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "CustomType.h"
#include "Engine.h"

//////////////////////////////////////////////////////////////////////////
// ATheLastBastionCharacter


ATheLastBastionCharacter::ATheLastBastionCharacter()
{	
	bReplicates = true;
	bIsDead = false;
	bIsGodMode = false;

	SprintSpeed = 850.0f;
	JogSpeed = 595.0f;
	walkSpeed = 255.0f;
	minTurnRate_Travel = 180.0f;
	maxTurnRate_Travel = 630.0f;
	minTurnRate_Combat = 240.0f;

	maxTurnRate_Combat = 1080.0f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->MaxWalkSpeed = JogSpeed; // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 800.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	GetCapsuleComponent()->bGenerateOverlapEvents = true;


	GetMesh()->RelativeLocation = FVector(0, 0, -90);
	GetMesh()->RelativeRotation = FRotator(0, -90, 0);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->bGenerateOverlapEvents = true;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}


void ATheLastBastionCharacter::BeginPlay()
{
	Super::BeginPlay();

	//if (PawnStats)
	//{
	//	PawnStats->OnHealthChanged.AddDynamic(this, &ATheLastBastionCharacter::OnHealthChangedHandle);
	//}

	CharacterCustomInit();

	OnTakeAnyDamage.AddDynamic(this, &ATheLastBastionCharacter::OnTakeAnyDamageHandle);
	OnTakePointDamage.AddDynamic(this, &ATheLastBastionCharacter::OnTakePointDamageHandle);

}

void ATheLastBastionCharacter::CharacterCustomInit()
{

	switch (CharacterType)
	{
	case ECharacterType::LanTrooper_T0:
	case ECharacterType::LanCB_T0:
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		break;
	default:
		break;
	}
}

void ATheLastBastionCharacter::OnTakeAnyDamageHandle(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
}

void ATheLastBastionCharacter::OnTakePointDamageHandle(AActor * DamagedActor, float Damage, AController * InstigatedBy, FVector HitLocation, UPrimitiveComponent * FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType * DamageType, AActor * DamageCauser)
{

	//UE_LOG(LogTemp, Log, TEXT("Take Point Damage - ATheLastBastionCharacter::OnTakePointDamageHandle"));

}

float ATheLastBastionCharacter::GetCurrentMaxSpeed() const
{
	return GetCharacterMovement()->MaxWalkSpeed;
}


