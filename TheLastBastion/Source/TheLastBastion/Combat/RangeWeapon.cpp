// Fill out your copyright notice in the Description page of Project Settings.

#include "RangeWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "Projectile.h"
#include "DrawDebugHelpers.h"
#include "AudioManager.h"


#define CLOSESHOTDISTANCE 350
static FName MuzzleSocketName =TEXT( "MuzzleSocket");
static FName LaunchLocationName = TEXT("LaunchLocation");

// Sets default values
ARangeWeapon::ARangeWeapon()
{
	// Intialize Components
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// initialize variables
	ShootingRange = 10000.0f;
	BulletSpeed = 1500.0f;
	MinVerticalAimOffset = 0.0f;
	MaxVerticalAimOffset = 50.0f;
	MinHorizontalAimOffset = -20.0f;
	MaxHorizontalAimOffset = 20.0f;
}

USceneComponent * ARangeWeapon::GetMesh() const
{
	return MeshComp;
}

void ARangeWeapon::BeginPlay()
{
	Super::BeginPlay();
	DivByBulletSpeed = 1 / BulletSpeed;
	
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
			//UE_LOG(LogTemp, Log, TEXT("Hit on, %s"), *Hit.GetActor()->GetName());
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
		if (CrossbowProjectile)
		{
			FVector FlyDir = (TraceEnd - LaunchLocation).GetSafeNormal();
			CrossbowProjectile->GetProjectileMovementComp()->Velocity = FlyDir * BulletSpeed;
			//UAudioManager::PlaySoundEffects(ESoundEffectType::ECrossBowFire, this);
		}

	}
}

void ARangeWeapon::NPCFire(const AActor* _target)
{

	GearOwner = Cast<ATheLastBastionCharacter>(GetOwner());
	// Trace the world from pawn eyes to crosshair location
	if (GearOwner != nullptr && ProjectileClassBP != nullptr)
	{
		const ATheLastBastionCharacter* TargetActor = Cast<ATheLastBastionCharacter>( _target);

		// Correctly Spawn the projectile
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);

		// Create and Initialize the spawn parameters for the projectile
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = this;


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
		float FlyTime = FlyDistance * DivByBulletSpeed;
		FVector PredictedPosition = TraceEnd + (TargetActor->GetVelocity()*FlyTime);

		// vertical offset
		float offsetZModder = FlyDistance * 0.01f;
		float VerticalRandomOffset = FMath::RandRange(MinVerticalAimOffset + offsetZModder, MaxVerticalAimOffset + offsetZModder);

		// horizontal offset
		float HorizontalRandomOffset_x = FMath::RandRange(MinHorizontalAimOffset, MaxHorizontalAimOffset);
		float HorizontalRandomOffset_y = FMath::RandRange(MinHorizontalAimOffset, MaxHorizontalAimOffset);

		// Override Predicted Location with offset value
		PredictedPosition += FVector(HorizontalRandomOffset_x, HorizontalRandomOffset_y, VerticalRandomOffset);

		// Finalize Projectile fly direction
		FlyDir = (PredictedPosition - MuzzleLocation).GetSafeNormal();

		bool const IsHit = GetWorld()->LineTraceSingleByChannel(Hit, MuzzleLocation, TraceEnd, ECollisionChannel::ECC_Visibility);


		AProjectile* CrossbowProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClassBP, MuzzleLocation, MuzzleRotation, SpawnParams);
		FVector horVel = FlyDir * BulletSpeed;
		CrossbowProjectile->SetInitFireVelocity(horVel, FlyTime);

	}

}



