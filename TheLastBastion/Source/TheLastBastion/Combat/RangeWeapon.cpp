// Fill out your copyright notice in the Description page of Project Settings.

#include "RangeWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "Projectile.h"
#include "DrawDebugHelpers.h"


#define CLOSESHOTDISTANCE 350
static FName MuzzleSocketName =TEXT( "MuzzleSocket");
static FName LaunchLocationName = TEXT("LaunchLocation");

// Sets default values
ARangeWeapon::ARangeWeapon()
{
	// Intialize Components
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	// initialize variables
	ShootingRange = 10000.0f;
	BulletSpeed = 1500.0f;
	MinVerticalAimOffset = -50.0f;
	MaxVerticalAimOffset = 50.0f;
	MinHorizontalAimOffset = -100.0f;
	MaxHorizontalAimOffset = 100.0f;
}

void ARangeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARangeWeapon::Fire()
{   
	//UE_LOG(LogTemp, Warning, TEXT("Fire in the hole"));

	GearOwner = Cast<ATheLastBastionCharacter>(GetOwner());

	// Trace the world from pawn eyes to crosshair location
	if (GearOwner != nullptr && ProjectileClassBP != nullptr)
	{
		// Find Player eyes location and rotation
		FVector EyesLocation;
		FRotator EyesRotation;
		GearOwner->GetActorEyesViewPoint(EyesLocation, EyesRotation);

		// Find the impact location
		FVector ShotDirection = EyesRotation.Vector();
		FVector TraceEnd = EyesLocation + (ShotDirection * ShootingRange);
		FVector ImpactPoint;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GearOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = false;
		FHitResult Hit;
		bool const IsHit = GetWorld()->LineTraceSingleByChannel(Hit, EyesLocation, TraceEnd, ECollisionChannel::ECC_Visibility);
		if (IsHit)
		{

			// Override TracerEndPoint when bullet get blocked
			ImpactPoint = Hit.ImpactPoint;
			UE_LOG(LogTemp, Log, TEXT("Hit on, %s"), *Hit.GetActor()->GetName());

		}


		// Draw a bebug line when linetrace hits something
		//DrawDebugLine(GetWorld(), EyesLocation, TraceEnd, FColor::Red, false, 2.0f, 0, 1.0f);

		// Correctly Spawn the projectile
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector LaunchLocation = MeshComp->GetSocketLocation(LaunchLocationName);
		
		// Create and Initialize the spawn parameters for the projectile
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = this;

		AProjectile* CrossbowProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClassBP, MuzzleLocation, EyesRotation, SpawnParams);

		// Calculate the velocity for the projectile
		// if enemy is too close, we are using weapon forward vector as fly dircection

		//UE_LOG(LogTemp, Log, TEXT("Hit Distance, %d"), Hit.Distance);


		TraceEnd = (IsHit && Hit.Distance > CLOSESHOTDISTANCE) ? ImpactPoint : TraceEnd;
		FVector FlyDir = (TraceEnd - LaunchLocation).GetSafeNormal();

		CrossbowProjectile->GetProjectileMovementComp()->Velocity = FlyDir * BulletSpeed;

		// Draw a bebug line from weapon to impact position
		//DrawDebugLine(GetWorld(), LaunchLocation, TraceEnd, FColor::Green, false, 2.0f, 0, 1.0f);
	}
}

void ARangeWeapon::NPCFire(const AActor* _target)
{

	GearOwner = Cast<ATheLastBastionCharacter>(GetOwner());
	// Trace the world from pawn eyes to crosshair location
	if (GearOwner != nullptr && ProjectileClassBP != nullptr)
	{
		const ACharacter* TargetActor = Cast<ACharacter>( _target);

		// Correctly Spawn the projectile
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);

		// Create and Initialize the spawn parameters for the projectile
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = this;

		AProjectile* CrossbowProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClassBP, MuzzleLocation, MuzzleRotation, SpawnParams);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GearOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = false;
		FHitResult Hit;

		// Get target location
		FVector TraceEnd = TargetActor->GetActorLocation();

		// AI fire predication
		FVector FlyDir;
		float FlyDistance; 
		(TraceEnd - MuzzleLocation).ToDirectionAndLength(FlyDir, FlyDistance);
		float FlyTime = FlyDistance / BulletSpeed;
		FVector PredictedPosition = TraceEnd + (TargetActor->GetVelocity()*FlyTime);

		// vertical offset
		float VerticalRandomOffset = FMath::RandRange(MinVerticalAimOffset, MaxVerticalAimOffset);

		// horizontal offset
		float HorizontalRandomOffset = FMath::RandRange(MinHorizontalAimOffset, MaxHorizontalAimOffset);

		// Override Predicted Location with offset value
		PredictedPosition += FVector(0, HorizontalRandomOffset, VerticalRandomOffset);

		// Finalize Projectile fly direction
		FlyDir = (PredictedPosition - MuzzleLocation).GetSafeNormal();

		bool const IsHit = GetWorld()->LineTraceSingleByChannel(Hit, MuzzleLocation, TraceEnd, ECollisionChannel::ECC_Visibility);
		if (IsHit)
		{
			//UE_LOG(LogTemp, Log, TEXT(""))
		}

		CrossbowProjectile->GetProjectileMovementComp()->Velocity = FlyDir * BulletSpeed;

	}

}



