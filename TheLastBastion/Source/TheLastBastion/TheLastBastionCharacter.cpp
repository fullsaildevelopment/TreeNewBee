// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "TheLastBastionCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"

#include "Combat/PawnStatsComponent.h"
#include "Combat/Gear.h"

#include "Animation/Base_AnimInstance.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/AudioComponent.h "

#include "CustomType.h"
#include "Engine.h"

//////////////////////////////////////////////////////////////////////////
// ATheLastBastionCharacter


ATheLastBastionCharacter::ATheLastBastionCharacter()
{	
	bIsDead = false;
	bIsGodMode = false;
	bIsEnemy = false;

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

	// RagDoll
	bIsRagDoll = false;
	bIsRecoveringFromRagDoll = false;
	bCheckBodyVelocity = false;
	RagDollBlendWeight = 0.0f;
	newRagDollIndex = 0;
	oldRagDollIndex = 0;

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

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AUDIOComp"));
	AudioComp->SetupAttachment(GetCapsuleComponent());
	AudioComp->RelativeLocation = FVector::ZeroVector;
	AudioComp->bAllowSpatialization = true;
	AudioComp->bOverrideAttenuation = false;
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


void ATheLastBastionCharacter::KnockOut(const FVector& dir, const AActor* _damageCauser, FName _boneName)
{
	//GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RagDollBlendWeight = 1.0f;
	GetMesh()->SetAllBodiesBelowSimulatePhysics(TEXT("pelvis"), true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(TEXT("pelvis"), RagDollBlendWeight);

	FVector Impulse = FVector::ZeroVector;

	const ATheLastBastionCharacter* damageCauser = Cast<ATheLastBastionCharacter>(_damageCauser);
	if (damageCauser)
	{
		AGear* currentWeapon = damageCauser->GetCurrentWeapon();
		if (currentWeapon)
		{

			bool isHeadShot = _boneName.Compare("head") == 0 || _boneName.Compare("neck_01") == 0;
			float force = (isHeadShot)?currentWeapon->GetCriticalForce() : currentWeapon->GetForce();
			Impulse =  dir.GetSafeNormal() * force;
		}
	}

	//UE_LOG(LogTemp, Log, TEXT("implse: x, %f, y, %f, z, %f"), Impulse.X, Impulse.Y, Impulse.Z);
	GetMesh()->AddImpulse(Impulse, _boneName, true);

	// if this character is dead, then dont bother to get up
	if (bIsDead == false)
	{
		// 
		bIsRagDoll = true;
		bCheckBodyVelocity = false;
		GetWorldTimerManager().ClearTimer(mRagDollTimer);
		newRagDollIndex++;

		UE_LOG(LogTemp, Log, TEXT("ATheLastBastionCharacter::KnockOut"));

		GetWorldTimerManager().SetTimer(mRagDollTimer, this, 
			&ATheLastBastionCharacter::ToggleBodyStopCheck, 1.0f, false, RagDoll_MinimumGetUpTime);
	}

	bIsRecoveringFromRagDoll = false;
	mBaseAnimRef->AnimInstanceResetOnRagDoll();

}

void ATheLastBastionCharacter::OnDead(const FVector& dir, const AActor* _damageCauser, FName _boneName)
{
	KnockOut(dir, _damageCauser, _boneName);

	PawnStats->SetEnableWeapon(false, true, true);

	//UE_LOG(LogTemp, Warning, TEXT("%s is Dead - ATheLastBastionCharacter::OnDead"), *GetName());
	// tell all the ai that target this actor to re choose target

	OnBecomeUnvailbleTargetEvent.Broadcast();
}

void ATheLastBastionCharacter::Kill()
{
}

void ATheLastBastionCharacter::ClampCapsuleToMesh()
{
	FVector pelvisLocation = GetMesh()->GetSocketLocation(TEXT("pelvis"));
	FVector capLocation = GetCapsuleComponent()->GetComponentLocation();


	FVector traceStart = pelvisLocation;
	FVector traceEnd = traceStart + FVector::UpVector * -500.0f;
	FHitResult Hit;
	bool const IsHit = GetWorld()->LineTraceSingleByObjectType(Hit, traceStart, traceEnd, ECollisionChannel::ECC_WorldStatic);
	FVector newLocation;
	if (IsHit)
	{
		newLocation = Hit.ImpactPoint + FVector::UpVector * 90.0f;
	}
	else
		newLocation = FVector(pelvisLocation.X, pelvisLocation.Y, capLocation.Z);

	GetCapsuleComponent()->SetWorldLocation(newLocation);
}

void ATheLastBastionCharacter::ToggleBodyStopCheck()
{
	bCheckBodyVelocity = true;
}

void ATheLastBastionCharacter::OnGetUp()
{

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	bIsRecoveringFromRagDoll = true;
	bCheckBodyVelocity = false;
	oldRagDollIndex = newRagDollIndex;

	//GetMesh()->SetAllBodiesBelowSimulatePhysics(TEXT("pelvis"), false);
	//GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//GetMesh()->RelativeLocation = FVector(0, 0, -90);
	//GetMesh()->RelativeRotation = FRotator(0, 270, 0);


	RagDollBlendWeight = 1.0f;
	FName pelvis = TEXT("pelvis");
	// Check whether this character is face up or face down
	FRotator pelvisRotator = GetMesh()->GetSocketRotation(pelvis);

	FVector traceStart = GetMesh()->GetSocketLocation(pelvis);
	FVector traceEnd = traceStart + UKismetMathLibrary::GetRightVector(pelvisRotator) * 100.0f;
	FHitResult Hit;
	bool const IsHit = GetWorld()->LineTraceSingleByObjectType(Hit, traceStart, traceEnd, ECollisionChannel::ECC_WorldStatic);  

	FRotator capRotator;
	if (IsHit)
		capRotator = FRotator(0, pelvisRotator.Yaw, 0);
	else
		capRotator = FRotator(0, 180 + pelvisRotator.Yaw, 0);

	GetCapsuleComponent()->SetWorldRotation(capRotator);


	mBaseAnimRef->OnGetUp(IsHit);
}

void ATheLastBastionCharacter::DuringRagDoll()
{
	ClampCapsuleToMesh();
	float velocitySqr = GetMesh()->GetPhysicsLinearVelocity().SizeSquared();
	//UE_LOG(LogTemp, Log, TEXT("Body SpeedSqr %f - ATheLastBastionCharacter::DuringRagDoll"), velocitySqr);
	if (velocitySqr <= 5.0f && bCheckBodyVelocity)
	{
		OnGetUp();
	}
}

void ATheLastBastionCharacter::RagDollRecoverOnFinish()
{
	bIsRagDoll = false;
	bIsRecoveringFromRagDoll = false;
	UE_LOG(LogTemp, Log, TEXT("ATheLastBastionCharacter::RagDollRecoverOnFinish"));
}

void ATheLastBastionCharacter::DuringRagDollRecovering(float _deltaTime)
{

	RagDollBlendWeight = FMath::FInterpTo(RagDollBlendWeight, 0.0f, _deltaTime, RagDoll_RecoverLerpSpeed);

	if (RagDollBlendWeight <= 0.05f)
	{
		RagDollBlendWeight = 0.0f;
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(TEXT("pelvis"), RagDollBlendWeight);
		GetMesh()->SetAllBodiesBelowSimulatePhysics(TEXT("pelvis"), false);
	}
	else
	{

		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(TEXT("pelvis"), RagDollBlendWeight);
	}
}

bool ATheLastBastionCharacter::IsOnDefend() const
{
	if (mBaseAnimRef)
	{
		return mBaseAnimRef->IsOnDefend();
	}
	return false;
}

bool ATheLastBastionCharacter::OnCounterAttack(const FVector& _damageCauserRelative)
{
	if (mBaseAnimRef)
	{
		return mBaseAnimRef->OnCounterAttack(_damageCauserRelative);
	}
	return false;
}

float ATheLastBastionCharacter::GetCurrentMaxSpeed() const
{
	return GetCharacterMovement()->MaxWalkSpeed;
}

AArmor * ATheLastBastionCharacter::GetCurrentArmor() const
{
	if (PawnStats)
	{
		return PawnStats->GetCurrentArmor();
	}

	return nullptr;
}

AGear * ATheLastBastionCharacter::GetCurrentWeapon() const
{
	if (PawnStats)
	{
		return PawnStats->GetCurrentRightHandWeapon();
	}
	else
	{
		return nullptr;
	}
}

AGear * ATheLastBastionCharacter::GetCurrentSecondaryWeapon() const
{
	if (PawnStats)
	{
		return PawnStats->GetCurrentLeftHandWeapon();
	}
	else
	{
		return nullptr;
	}
}


