// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Combat/PawnStatsComponent.h"
#include "TheLastBastionCharacter.h"
#include "DrawDebugHelpers.h"

#define SPHERERADIUS 1.5f

#define ECC_EnemyBody ECollisionChannel::ECC_GameTraceChannel3
#define ECC_HeroBody  ECollisionChannel::ECC_GameTraceChannel1

#define ECC_EnemyWeapon ECollisionChannel::ECC_GameTraceChannel4
#define ECC_HeroWeapon  ECollisionChannel::ECC_GameTraceChannel2


// Sets default values
AProjectile::AProjectile()
{   
	// Create A Default Root Component as a container
	RootComp= CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	// Attach Mesh component to root component
	ProjectileMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComp"));
	ProjectileMeshComp->SetupAttachment(RootComp);
	ProjectileMeshComp->SetCollisionProfileName(TEXT("NoCollision"));
	ProjectileMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DamageSphereHolder = CreateDefaultSubobject<USceneComponent>(TEXT("sphereCastPosition"));
	DamageSphereHolder->SetupAttachment(ProjectileMeshComp);
	DamageSphereHolder->SetActive(false);
	DamageSphereHolder->RelativeLocation = FVector(0, 8, 0);
	

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->UpdatedComponent = RootComp;
	ProjectileMovementComp->InitialSpeed = 3000.f;
	ProjectileMovementComp->MaxSpeed = 3000.f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bShouldBounce = false;
	ProjectileMovementComp->bAutoActivate = false;

	// Die after 3 seconds by default
	InitialLifeSpan = 20.0f;
	PenetrateLevel = 1; 
	CurrentHitCount = 0;

	PrimaryActorTick.bCanEverTick = true;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	AGear* RangeWeapon = Cast<AGear>(GetOwner());
	if (RangeWeapon)
	{
		//CombineDamage(RangeWeapon);
		GearOwner = RangeWeapon->GetGearOwner();
		ProjectileMovementComp->SetActive(true);
		SetDamageIsEnabled(true);
		RootComponent->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));

		ObjectParams.AddObjectTypesToQuery(ECC_EnemyBody);
		ObjectParams.AddObjectTypesToQuery(ECC_HeroBody);
		ObjectParams.AddObjectTypesToQuery(ECC_EnemyWeapon);
		ObjectParams.AddObjectTypesToQuery(ECC_HeroWeapon);
		ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RangeWeapon is NULL - AProjectile::BeginPlay"));
	}

}

bool AProjectile::ShouldDestroy() const
{
	//UE_LOG(LogTemp, Log, TEXT("PenetrateLevel : %d, CurrentHitCount: %d"), PenetrateLevel, CurrentHitCount);
	return PenetrateLevel <= CurrentHitCount;
}

void AProjectile::Tick(float _deltaTime)
{
	if (bDamageIsEnable)
	{
		FVector ArrowPosition = DamageSphereHolder->GetComponentLocation();

		// if penetrate arrow, makes it wont hurt the same target
		FCollisionQueryParams Params;
		if (IgnoredActors.Num() > 0)
			Params.AddIgnoredActors(IgnoredActors);

		Params.bReturnPhysicalMaterial = true;
		Params.bTraceComplex = true;

		//ObjectParams.AddObjectTypesToQuery(ECC_EnemyBody);
		//ObjectParams.AddObjectTypesToQuery(ECC_HeroBody);
		//ObjectParams.AddObjectTypesToQuery(ECC_EnemyWeapon);
		//ObjectParams.AddObjectTypesToQuery(ECC_HeroWeapon);
		//ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);

		if (ObjectParams.IsValid() == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid object types -  AProjectile::Tick"));
			return;
		}

		FDamageInfo DamageInfo;
		DamageInfo.applyDamageType = EApplyDamageType::Point;
		DamageInfo.damageType = DamageType;

		UWorld* World = GetWorld();
		bool const bHit = World ? World->SweepSingleByObjectType(DamageInfo.hitResult, 
			ArrowPosition, ArrowPosition, FQuat::Identity, ObjectParams, FCollisionShape::MakeSphere(SPHERERADIUS), Params) : false;


		DrawDebugSphere(World, ArrowPosition, SPHERERADIUS, 8, FColor::Red);

		if (bHit)
		{
			AActor* damagedActor = DamageInfo.hitResult.GetActor();
			CurrentHitCount++;
			IgnoredActors.Add(damagedActor);

			DamageInfo.hitDirection = GearOwner->GetActorLocation() - DamageInfo.hitResult.GetActor()->GetActorLocation();
			UPawnStatsComponent* pSC = GearOwner->GetPawnStatsComp();

			if (pSC != nullptr)
				pSC->ApplyDamage(DamageInfo);
			else
				UE_LOG(LogTemp, Error, TEXT("UPawnStatsComponent is NuLL -- AWeapon::Tick "));

			if (PenetrateLevel <= CurrentHitCount)
			{

				FActorSpawnParameters spawnParam;
				spawnParam.Owner = damagedActor;
				spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				AProjectile* copyProjectile = GetWorld()->SpawnActor<AProjectile>(this->GetClass(), GetActorLocation(), GetActorRotation(), spawnParam);

				
				ATheLastBastionCharacter* Character = Cast<ATheLastBastionCharacter>(damagedActor);
				FAttachmentTransformRules attachRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);

				if (Character)
					copyProjectile->AttachToComponent(Character->GetMesh(), attachRules, DamageInfo.hitResult.BoneName);
				else
					copyProjectile->AttachToActor(damagedActor, attachRules);

				Destroy();
			}
			//UE_LOG(LogTemp, Log, TEXT("Hit on : %s"), *DamageInfo.hitResult.GetActor()->GetName());
		}
	}

}

void AProjectile::SetDamageIsEnabled(bool _val)
{
	bDamageIsEnable = _val;
}
