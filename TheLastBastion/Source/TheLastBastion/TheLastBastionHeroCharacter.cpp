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
#include "Components/CapsuleComponent.h"

#include "GI_TheLastBastion.h"
#include "UI/InGameHUD.h"
#include "PCs/SinglePlayerPC.h"

#include "Kismet/GameplayStatics.h"
#include "AIGroupBase.h"
#include "AI/TheLastBastionGroupAIController.h"
#include "UI/InGameFloatingText.h"
#include "DrawDebugHelpers.h"


#define  COMMANDRANGE 10000
#define  HOLD_POSITION_BWD_OFFSET 100

ATheLastBastionHeroCharacter::ATheLastBastionHeroCharacter()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->RelativeLocation = FVector(0, 0, 60);

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

	GetMesh()->SetCollisionProfileName("HeroBody");

	//CapHalfSize = 90.0f;
	//CapRadius = 34.0f;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 90.0f);

	bUsePreviousMovementAxis = false;
	bIsYawControllDisabled = false;
	bIsMovementDisabled = false;
	bIsInCommandMode = false;

	Focus_CamRotationLagging = 15.0f;
	Unfocus_CamRotationLagging = 30.0f;

	HeroStats = CreateDefaultSubobject<UHeroStatsComponent>(TEXT("Stats"));
	PawnStats = HeroStats;	
}

void ATheLastBastionHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	CameraBoom->CameraRotationLagSpeed = Unfocus_CamRotationLagging;

	// Get Anim Bp Reference
	mAnimInstanceRef = Cast<UHero_AnimInstance>(this->GetMesh()->GetAnimInstance());
	if (mAnimInstanceRef == nullptr) { UE_LOG(LogTemp, Warning, TEXT("ATheLastBastionCharacter can not take other AnimInstance other than UHero_AnimInstance, - ATheLastBastionCharacter")); return; }

	ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(GetController());
	if (pc)
	{
		mInGameHUD = pc->GetInGameHUD();
		if (mInGameHUD)
		{
			mInGameHUD->InitStats(HeroStats);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("mInGameHUD is NULL - ATheLastBastionHeroCharacter::BeginPlay"));
	}
	else
		UE_LOG(LogTemp, Error, TEXT("pc is NULL - ATheLastBastionHeroCharacter::BeginPlay"));
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
	PlayerInputComponent->BindAction("Focus", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnMiddleMouseButtonPressed);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCorLAltPressed);

	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnRightMouseButtonPressed);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &ATheLastBastionHeroCharacter::OnRightMouseButtonReleased);

	PlayerInputComponent->BindAction("TabMeleeRange", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnTABPressed);

	PlayerInputComponent->BindAction("UseRangeWeapon", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnUseRangeWeapon);
	PlayerInputComponent->BindAction("UseSHWeapon",    IE_Pressed, this, &ATheLastBastionHeroCharacter::OnUseSHWeapon);
	PlayerInputComponent->BindAction("UseTHWeapon",    IE_Pressed, this, &ATheLastBastionHeroCharacter::OnUseTHWeapon);
	PlayerInputComponent->BindAction("UseHeavyWeapon", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnUseHeavyWeapon);

	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnPause);

	PlayerInputComponent->BindAction("Command", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandPressed);
	PlayerInputComponent->BindAction("Command", IE_Released, this, &ATheLastBastionHeroCharacter::OnCommandReleased);

	PlayerInputComponent->BindAction("MarchCommand", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandMarch);
	PlayerInputComponent->BindAction("Command_Hold", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandHold);
	PlayerInputComponent->BindAction("Command_Follow", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandFollowing);
	PlayerInputComponent->BindAction("Command_Reform", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandReform);
	PlayerInputComponent->BindAction("Command_Distribute", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandDistribute);
	PlayerInputComponent->BindAction("Command_Forward", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandForward);
	PlayerInputComponent->BindAction("Command_Backward", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandBackward);


}

#pragma region On Player Input
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

void ATheLastBastionHeroCharacter::MoveForward(float Value)
{

	if (bIsMovementDisabled)
		return;
	// catch the forward axis
	if (!bUsePreviousMovementAxis)
		MoveForwardAxis = Value;
	else
	{
		// the default previous movement axis is forward, i.e W is pressed
		if (MoveRightAxis == 0.0f && MoveForwardAxis == 0.0f)
			MoveForwardAxis = -1.0f;
	}

	if ((Controller != NULL) && (MoveForwardAxis != 0.0f))
	{

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, MoveForwardAxis);
	}
}

void ATheLastBastionHeroCharacter::MoveRight(float Value)
{
	if (bIsMovementDisabled)
		return;

	// Catch the right axis 
	if (!bUsePreviousMovementAxis)
		MoveRightAxis = Value;

	if ((Controller != NULL) && (MoveRightAxis != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, MoveRightAxis);
	}
}

void ATheLastBastionHeroCharacter::OnSprintPressed()
{

	mAnimInstanceRef->OnSprintPressed();
	//UE_LOG(LogTemp, Warning, TEXT("OnSprintPressed"));
}

void ATheLastBastionHeroCharacter::OnSprintReleased()
{
	mAnimInstanceRef->OnSprintReleased();
	//UE_LOG(LogTemp, Warning, TEXT("OnSprintReleased"));
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

void ATheLastBastionHeroCharacter::OnMiddleMouseButtonPressed()
{
	// figure out which target to focus
	mAnimInstanceRef->OnMiddleMouseButtonPressed();
}

void ATheLastBastionHeroCharacter::OnCorLAltPressed()
{
	mAnimInstanceRef->OnCorLAltPressed();
}

void ATheLastBastionHeroCharacter::OnRightMouseButtonPressed()
{
	mAnimInstanceRef->OnRightMouseButtonPressed();
}

void ATheLastBastionHeroCharacter::OnRightMouseButtonReleased()
{
	mAnimInstanceRef->OnRightMouseButtonReleased();
}

void ATheLastBastionHeroCharacter::AddControllerYaw(float _yaw)
{
	if (!bIsYawControllDisabled)
	{
		this->AddControllerYawInput(_yaw);
	}
}

void ATheLastBastionHeroCharacter::OnUseRangeWeapon()
{
	mAnimInstanceRef->OnChangeWeapon(EEquipType::CrossBow);
}

void ATheLastBastionHeroCharacter::OnUseSHWeapon()
{
	mAnimInstanceRef->OnChangeWeapon(EEquipType::ShieldSword);
}

void ATheLastBastionHeroCharacter::OnUseTHWeapon()
{
	mAnimInstanceRef->OnChangeWeapon(EEquipType::TwoHandSword);
}

void ATheLastBastionHeroCharacter::OnUseHeavyWeapon()
{
	mAnimInstanceRef->OnChangeWeapon(EEquipType::HeavyWeapon);
}

void ATheLastBastionHeroCharacter::OnPause()
{
	ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(GetController());
	if (pc)
	{
		pc->OnPauseButtonIsPressed();
	}
}

void ATheLastBastionHeroCharacter::OnCommandMarch()
{
	FVector EyesLocation;
	FRotator EyesRotation;
	GetActorEyesViewPoint(EyesLocation, EyesRotation);

	FVector ShotDirection = EyesRotation.Vector();
	FVector TraceEnd = EyesLocation + (ShotDirection * COMMANDRANGE);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult Hit;
	bool const IsHit = GetWorld()->LineTraceSingleByChannel(Hit, EyesLocation, TraceEnd, ECollisionChannel::ECC_Visibility);
	if (IsHit)
	{
		FVector ImpactLocation = Hit.ImpactPoint;
		UE_LOG(LogTemp, Log, TEXT("%s"), *Hit.GetActor()->GetName())
		DrawDebugSphere(GetWorld(), ImpactLocation, 50.0f, 8, FColor::Green, false, 5.0f);
		if (CommandedGroup)
		{
			CommandedGroup->SetMarchLocation(ImpactLocation, GC_GOTOLOCATION);
		}
	}



}

void ATheLastBastionHeroCharacter::OnCommandHold()
{
	if (CommandedGroup)
	{
		FVector targetLocation = GetActorLocation() - GetActorForwardVector() * HOLD_POSITION_BWD_OFFSET;
		CommandedGroup->SetMarchLocation(targetLocation, GC_HOLDLOCATION);

		DrawDebugSphere(GetWorld(), targetLocation, 50.0f, 8, FColor::Green, false, 5.0f);

	}

}

/** Switch from between scatter and compact */
void ATheLastBastionHeroCharacter::OnCommandDistribute()
{
	if (CommandedGroup)
	{
		FVector targetLocation = CommandedGroup->GetActorLocation();// +CommandedGroup->GetActorForwardVector() * 500.0f;
		CommandedGroup->SetMarchLocation(targetLocation, GC_DISTRIBUTE);
		DrawDebugSphere(GetWorld(), targetLocation, 50.0f, 8, FColor::Green, false, 5.0f);
	}
}

void ATheLastBastionHeroCharacter::OnCommandReform()
{

}

void ATheLastBastionHeroCharacter::OnCommandForward()
{
	if (CommandedGroup)
	{
		FVector targetLocation = CommandedGroup->GetActorLocation() + CommandedGroup->GetActorForwardVector() * 500.0f;
		CommandedGroup->SetMarchLocation(targetLocation, GC_FORWARD);
		DrawDebugSphere(GetWorld(), targetLocation, 50.0f, 8, FColor::Green, false, 5.0f);
	}

}

void ATheLastBastionHeroCharacter::OnCommandBackward()
{
}

void ATheLastBastionHeroCharacter::OnCommandFollowing()
{
}

void ATheLastBastionHeroCharacter::OnCommandPressed()
{
	if (mAnimInstanceRef->GetActivatedEquipmentType() != EEquipType::CrossBow)
	{
		mInGameHUD->ToggleFireMode(true);
	}
	bIsInCommandMode = true;

}

void ATheLastBastionHeroCharacter::OnCommandReleased()
{
	if (mAnimInstanceRef->GetActivatedEquipmentType() != EEquipType::CrossBow)
	{
		mInGameHUD->ToggleFireMode(false);
	}
	bIsInCommandMode = false;
}

void ATheLastBastionHeroCharacter::OnTABPressed()
{
	mAnimInstanceRef->OnSwapBetweenMeleeAndRange();
}

#pragma endregion

FVector ATheLastBastionHeroCharacter::GetPawnViewLocation() const
{
	if (FollowCamera != nullptr)
	{
		return FollowCamera->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();

}

void ATheLastBastionHeroCharacter::ToggleFireMode(bool _val)
{
	mInGameHUD->ToggleFireMode(_val);
}

AGear * ATheLastBastionHeroCharacter::GetCurrentWeapon() const
{
	return HeroStats->GetCurrentRightHandWeapon();
}

void ATheLastBastionHeroCharacter::OnTakeAnyDamageHandle(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
}

void ATheLastBastionHeroCharacter::OnTakePointDamageHandle(AActor * DamagedActor, float Damage, AController * InstigatedBy, FVector HitLocation, UPrimitiveComponent * FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType * DamageType, AActor * DamageCauser)
{

	if (bIsDead)
		return;

	bool isCritical = false, isStun = false;

	float totalDamage = HeroStats->CalculateDamage(Damage, DamageCauser, isCritical, isStun);

	/// update HUD hp bar
	mInGameHUD->SetHpOnHealthChange(HeroStats);

	/// pop floating number
	TSubclassOf<UUserWidget> fT_WBP = HeroStats->GetFloatingText_WBP();

	if (fT_WBP == nullptr)
	{
		UE_LOG(LogTemp, Error,
			TEXT("fT_WBP == nullptr - ATheLastBastionEnemyCharacter::OnTakePointDamageHandle"));
	}

	FVector2D screenPos;
	UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(this, 0), HitLocation, screenPos);
	UWorld* world = GetWorld();
	if (world && fT_WBP)
	{
		UInGameFloatingText* damageFT = Cast<UInGameFloatingText>(CreateWidget<UUserWidget>(world, fT_WBP));
		if (damageFT)
		{
			damageFT->SetInGameFTProperty(FText::AsNumber((int)totalDamage));
			damageFT->SetRenderTranslation(screenPos);
			if (isCritical)
				damageFT->SetFontSize(FontSize_Critical);
			damageFT->SetStyle(EFloatingTextStyle::Enemy);
			damageFT->AddToViewport();
		}

	}

	mAnimInstanceRef->OnBeingHit(BoneName, ShotFromDirection, HitLocation);
}
