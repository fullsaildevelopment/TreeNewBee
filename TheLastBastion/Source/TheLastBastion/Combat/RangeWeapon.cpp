// Fill out your copyright notice in the Description page of Project Settings.

#include "RangeWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "Projectile.h"
#include "DrawDebugHelpers.h"

#define ECC_EnemyBody ECollisionChannel::ECC_GameTraceChannel3
#define ECC_HeroBody  ECollisionChannel::ECC_GameTraceChannel1

static FName MuzzleSocketName = "MuzzleSocket";
// Sets default values
ARangeWeapon::ARangeWeapon()
{
	// Intialize Components
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	// initialize variables
	ShootingRange = 10000.0f;
	BulletSpeed = 1500.0f;
}

void ARangeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARangeWeapon::Fire()
{   
	UE_LOG(LogTemp, Warning, TEXT("Fire in the hole"));

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

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GearOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;
		FHitResult Hit;

		bool isEnemyCharacter = (Cast<ATheLastBastionEnemyCharacter>(GearOwner) != nullptr);

		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery((isEnemyCharacter) ? ECC_HeroBody : ECC_EnemyBody);

		if (ObjectParams.IsValid() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid object types"));
			return;
		}

		if (GetWorld()->LineTraceSingleByObjectType(Hit, EyesLocation, TraceEnd, ObjectParams, QueryParams))
		{
			// Override TracerEndPoint when bullet get blocked
			TraceEnd = Hit.ImpactPoint;
		}

		// Draw a bebug line when linetrace hits something
		DrawDebugLine(GetWorld(), EyesLocation, TraceEnd, FColor::Red, false, 2.0f, 0, 1.0f);
	}
}
