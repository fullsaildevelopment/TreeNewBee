// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ALobbyPawn::ALobbyPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsCheckingRanger = true;
	InspectorCameraSwitchRate = 5.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraRotationLag = true;
	SetRootComponent(CameraBoom);

	InspectorCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	InspectorCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	InspectorCamera->bUsePawnControlRotation = false;

}

// Called when the game starts or when spawned
void ALobbyPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ALobbyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector targetLocation = (bIsCheckingRanger) ? RangerAvatarPosition : BuilderAvatarPosition;
	FVector newLocation = FMath::VInterpTo(GetActorLocation(), targetLocation, DeltaTime, InspectorCameraSwitchRate);
	SetActorLocation(newLocation);

}

// Called to bind functionality to input
void ALobbyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

}


