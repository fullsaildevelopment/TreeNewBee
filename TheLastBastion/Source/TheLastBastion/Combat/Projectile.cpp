// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
AProjectile::AProjectile()
{
	// Create a CapsuleComponent as a simple collision representation
	RootComp = CreateEditorOnlyDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	// Attach Mesh component to root component
	ProjectileMeshComp = CreateEditorOnlyDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComp"));
	if (ProjectileMeshComp)
	{
		ProjectileMeshComp->SetupAttachment(RootComp);
	}

	CollisionComp = CreateEditorOnlyDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	if (CollisionComp)
	{
		CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
		CollisionComp->CanCharacterStepUpOn = ECB_No;
		CollisionComp->SetupAttachment(ProjectileMeshComp);
	}

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->UpdatedComponent = CollisionComp;
	ProjectileMovementComp->InitialSpeed = 3000.f;
	ProjectileMovementComp->MaxSpeed = 3000.f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bShouldBounce = false;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}
