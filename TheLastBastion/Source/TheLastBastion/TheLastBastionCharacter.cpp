// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "TheLastBastionCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/Hero_AnimInstance.h"


//////////////////////////////////////////////////////////////////////////
// ATheLastBastionCharacter

ATheLastBastionCharacter::ATheLastBastionCharacter()
{	

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	bIsSprinting = false;
	bTryToSprint = false;
	SprintSpeed = 850.0f;
	JogSpeed = 595.0f;
	walkSpeed = 255.0f;
	minTurnRate = 180.0f;
	maxTurnRate_Travel = 630.0f;
	maxTurnRate_Combat = 1440.0f;
	maxTurnRate = maxTurnRate_Travel;

	// Init capsule size of each situations
	CapHalfSize_Travel = 90.0f;
	CapRadius_Travel = 34.0f;
	CapHalfSize_ShieldSword = 80.0f;
	CapRadius_ShieldSword = 30.0;


	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(CapRadius_Travel, CapHalfSize_Travel);
	GetMesh()->RelativeLocation = FVector(0, 0, -CapRadius_Travel);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->MaxWalkSpeed = JogSpeed; // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;
	// Camera does not rotate relative to arm

	// Create a static mesh for right and left hand equipment
	LeftHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftHandWeapon"));
	LeftHand->SetupAttachment(GetMesh(), TEXT("Shield"));

	RightHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightHandWeapon"));
	RightHand->SetupAttachment(GetMesh(), TEXT("Weapon"));


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)


}


void ATheLastBastionCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->RotationRate = FRotator(0.0f, maxTurnRate, 0.0f); 
	// Get Anim Bp Reference
	mAnimInstanceRef = Cast<UHero_AnimInstance>(this->GetMesh()->GetAnimInstance());
	if (mAnimInstanceRef == nullptr) { UE_LOG(LogTemp, Warning, TEXT("ATheLastBastionCharacter can not take other AnimInstance other than UHero_AnimInstance, - ATheLastBastionCharacter")); return; }
}


//////////////////////////////////////////////////////////////////////////
// Input

void ATheLastBastionCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATheLastBastionCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATheLastBastionCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATheLastBastionCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATheLastBastionCharacter::LookUpAtRate);


	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATheLastBastionCharacter::OnJumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ATheLastBastionCharacter::OnJumpReleased);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATheLastBastionCharacter::OnSprintPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATheLastBastionCharacter::OnSprintReleased);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ATheLastBastionCharacter::OnAttackPressed);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ATheLastBastionCharacter::OnEquipPressed);

}

void ATheLastBastionCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATheLastBastionCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATheLastBastionCharacter::OnSprintPressed()
{
	bTryToSprint = true;
		 
	bool ableToSprint = !mAnimInstanceRef->IsSpeedOverrideByAnim()
		&& mAnimInstanceRef->GetActivatedEquipmentType() == EEquipType::Travel;
	if (ableToSprint)
	{
		StartSprint();
	}
	UE_LOG(LogTemp, Warning, TEXT("OnSprintPressed"));
}

void ATheLastBastionCharacter::OnSprintReleased()
{

	bTryToSprint = false;
	StopSprint();
}

void ATheLastBastionCharacter::StartSprint()
{
	this->GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	bIsSprinting = true;
}

void ATheLastBastionCharacter::StopSprint()
{
	this->GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	bIsSprinting = false;
}

void ATheLastBastionCharacter::SetCapsuleSizeToFitTravel()
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(CapHalfSize_Travel, false);
	GetCapsuleComponent()->SetCapsuleRadius(CapRadius_Travel);

	GetMesh()->RelativeLocation = FVector(0, 0, -CapHalfSize_Travel);
}

void ATheLastBastionCharacter::SetCapsuleSizeToFitSwordShield()
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(CapHalfSize_ShieldSword, false);
	GetCapsuleComponent()->SetCapsuleRadius(CapRadius_ShieldSword);

	GetMesh()->RelativeLocation = FVector(0, 0, -CapHalfSize_ShieldSword);
}


void ATheLastBastionCharacter::OnJumpPressed()
{
	if (mAnimInstanceRef->GetIsJumpEnable())
	{
		mAnimInstanceRef->SetIsJump(true);
		this->Jump();
	}
}

void ATheLastBastionCharacter::OnJumpReleased()
{
	mAnimInstanceRef->SetIsJump(false);
	this->StopJumping();
}

void ATheLastBastionCharacter::OnAttackPressed()
{

	mAnimInstanceRef->OnAttack();

	// Once Attack, we activate combat mode, hence we have to slow our speed down
	if (bIsSprinting)
		StopSprint();

	SetCapsuleSizeToFitSwordShield();
	// Increase the turn rate for flexible control
	GetCharacterMovement()->RotationRate.Yaw = maxTurnRate_Combat;
}

void ATheLastBastionCharacter::OnEquipPressed()
{
	// let animInstance handle equip
	mAnimInstanceRef->OnEquip();

	if (mAnimInstanceRef->GetActivatedEquipmentType() != EEquipType::Travel )
	{
		SetCapsuleSizeToFitSwordShield();
		GetCharacterMovement()->RotationRate.Yaw = maxTurnRate_Combat;
		if(bIsSprinting)
			StopSprint();
	}
	else
	{
		SetCapsuleSizeToFitTravel();
		GetCharacterMovement()->RotationRate.Yaw = maxTurnRate_Travel;
		if (bTryToSprint)
			StartSprint();
	}

}

void ATheLastBastionCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATheLastBastionCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
