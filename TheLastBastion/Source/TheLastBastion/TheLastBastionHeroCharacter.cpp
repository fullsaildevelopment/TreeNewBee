// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionHeroCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/Hero_AnimInstance.h"
#include "CustomType.h"
#include "Combat/Weapon.h"
#include "Combat/Armor.h"
#include "Combat/HeroStatsComponent.h"
#include "Components/SphereComponent.h"

#include "GI_TheLastBastion.h"
#include "UI/InGameHUD.h"

ATheLastBastionHeroCharacter::ATheLastBastionHeroCharacter() : Super()
{
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraRotationLag = true;

	LockOn_CamRotationLagging = 10.0f;
	NonLockOn_CamRotationLagging = 50.0f;

												// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;
	// Camera does not rotate relative to arm

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;


	TargetDetector = CreateDefaultSubobject<USphereComponent>(TEXT("MeleeTargetDetector"));
	TargetDetector->SetupAttachment(GetMesh(), TEXT("Root"));
	TargetDetector->InitSphereRadius(1200);
	TargetDetector->SetCanEverAffectNavigation(false);
	TargetDetector->bGenerateOverlapEvents = true;
	TargetDetector->bHiddenInGame = false;
	TargetDetector->SetCollisionProfileName("EnemyDetector");

	HeroStats = CreateDefaultSubobject<UHeroStatsComponent>(TEXT("Stats"));
	PawnStats = HeroStats;	
}

void ATheLastBastionHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CameraBoom->CameraRotationLagSpeed = NonLockOn_CamRotationLagging;

	// Get Anim Bp Reference
	mAnimInstanceRef = Cast<UHero_AnimInstance>(this->GetMesh()->GetAnimInstance());
	if (mAnimInstanceRef == nullptr) { UE_LOG(LogTemp, Warning, TEXT("ATheLastBastionCharacter can not take other AnimInstance other than UHero_AnimInstance, - ATheLastBastionCharacter")); return; }

	UE_LOG(LogTemp, Warning, TEXT("Try Get GameInstance"));

	UWorld* world = GetWorld();
	if (world == nullptr)
		return;
	//UGI_TheLastBastion* const gi = Cast<UGI_TheLastBastion>(world->GetGameInstance());
	//if (gi)
	//	gi->ShowInGameHUD();
}

void ATheLastBastionHeroCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATheLastBastionHeroCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATheLastBastionHeroCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &ATheLastBastionHeroCharacter::AddControllerYaw);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATheLastBastionHeroCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATheLastBastionHeroCharacter::LookUpAtRate);


	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnJumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ATheLastBastionHeroCharacter::OnJumpReleased);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnSprintPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATheLastBastionHeroCharacter::OnSprintReleased);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnAttackPressed);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnEquipPressed);
	PlayerInputComponent->BindAction("Focus", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnFocusPressed);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnDodgePressed);

}

void ATheLastBastionHeroCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());

}

void ATheLastBastionHeroCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATheLastBastionHeroCharacter::OnSprintPressed()
{
	mAnimInstanceRef->OnSprintPressed();
	UE_LOG(LogTemp, Warning, TEXT("OnSprintPressed"));
}

void ATheLastBastionHeroCharacter::OnSprintReleased()
{
	mAnimInstanceRef->OnSprintReleased();
	UE_LOG(LogTemp, Warning, TEXT("OnSprintReleased"));
}

void ATheLastBastionHeroCharacter::OnJumpPressed()
{
	mAnimInstanceRef->OnJumpStart();

}

void ATheLastBastionHeroCharacter::OnJumpReleased()
{
	mAnimInstanceRef->OnJumpStop();
}

void ATheLastBastionHeroCharacter::OnAttackPressed()
{
	mAnimInstanceRef->OnAttack();
}

void ATheLastBastionHeroCharacter::OnEquipPressed()
{
	mAnimInstanceRef->OnEquip();
}

void ATheLastBastionHeroCharacter::OnFocusPressed()
{
	// figure out which target to focus
	HeroStats->OnFocus();

	mAnimInstanceRef->OnFocus();
}

void ATheLastBastionHeroCharacter::OnDodgePressed()
{
	mAnimInstanceRef->OnDodge();
}

void ATheLastBastionHeroCharacter::AddControllerYaw(float _yaw)
{
	if (!mAnimInstanceRef->GetIsFocus())
		this->AddControllerYawInput(_yaw);
}

void ATheLastBastionHeroCharacter::MoveForward(float Value)
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
	MoveForwardAxis = Value;

}

void ATheLastBastionHeroCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
	MoveRightAxis = Value;

}
