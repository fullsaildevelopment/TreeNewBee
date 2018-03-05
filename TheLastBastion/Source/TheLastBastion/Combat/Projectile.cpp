// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Combat/PawnStatsComponent.h"

#include "Combat/RangeWeapon.h"
#include "Combat/Shield.h"

#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TheLastBastionHeroCharacter.h"
#include "DrawDebugHelpers.h"

#include "VfxManager.h"
#include "AudioManager.h"
#include "Kismet/GameplayStatics.h"



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
	bIsFlying = false;

	PrimaryActorTick.bCanEverTick = true;

}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (!bIsFlying)
		MakeStatic();
	else
		ProjectileMovementComp->SetActive(true);
}

bool AProjectile::ShouldDestroy() const
{
	//UE_LOG(LogTemp, Log, TEXT("PenetrateLevel : %d, CurrentHitCount: %d"), PenetrateLevel, CurrentHitCount);
	return PenetrateLevel <= CurrentHitCount;
}

void AProjectile::SetInitFireVelocity(const FVector & _hor, float flyTime)
{
	float vel_Z = flyTime * ProjectileMovementComp->GetGravityZ() * ProjectileMovementComp->ProjectileGravityScale;
	ProjectileMovementComp->Velocity = _hor - 2.0f * FVector::UpVector * vel_Z;
	//UE_LOG(LogTemp, Log, TEXT("%f, %f, %f - AProjectile::SetInitFireVelocity "), ProjectileMovementComp->Velocity.X, ProjectileMovementComp->Velocity.Y, ProjectileMovementComp->Velocity.Z);
}

void AProjectile::ProjectileOnFire(AGear * _rangeWeapon)
{
	if (_rangeWeapon)
	{
		bIsFlying = true;
		GearOwner = _rangeWeapon->GetGearOwner();
		SetDamageIsEnabled(true);
		//RootComponent->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
		if (GearOwner->IsA<ATheLastBastionHeroCharacter>())
		{
			ObjectParams.AddObjectTypesToQuery(ECC_HeroBody);
			ObjectParams.AddObjectTypesToQuery(ECC_EnemyBody);
		}
		else
			ObjectParams.AddObjectTypesToQuery((GearOwner->IsEnemy()) ? ECC_HeroBody : ECC_EnemyBody);

		ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);

		IgnoredActors.Add(this);
		IgnoredActors.Add(_rangeWeapon);
		IgnoredActors.Add(GearOwner);
	}
	else
	{
		bIsFlying = false;
	}
}

void AProjectile::MakeStatic()
{
	SetDamageIsEnabled(false);
	ProjectileMovementComp->SetActive(false);
	ProjectileMovementComp->Velocity = FVector::ZeroVector;
	SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	SetMesh(GetNonFlyingMesh());

}

void AProjectile::SetMesh(UStaticMesh * _mesh)
{
	if (_mesh)
		ProjectileMeshComp->SetStaticMesh(_mesh);

	//_mesh->StaticMaterials
	//for (int i = 0; i < _mesh->Materials.Num(); i++)
	//{
	//	ProjectileMeshComp->SetMaterial(i, _mesh->Materials[i].MaterialInterface);
	//	ProjectileMeshComp->Set
	//}
}

void AProjectile::Tick(float _deltaTime)
{

	//UE_LOG(LogTemp, Log, TEXT("Vel: %f"), ProjectileMovementComp->Velocity.SizeSquared());
	if (bDamageIsEnable)
	{

		// if penetrate arrow, makes it wont hurt the same target
		FCollisionQueryParams Params;
		if (IgnoredActors.Num() > 0)
			Params.AddIgnoredActors(IgnoredActors);

		Params.bReturnPhysicalMaterial = true;
		Params.bTraceComplex = true;

		if (ObjectParams.IsValid() == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid object types -  AProjectile::Tick"));
			return;
		}

		FDamageInfo DamageInfo;
		DamageInfo.applyDamageType = EApplyDamageType::Point;
		DamageInfo.damageType = DamageType;
		DamageInfo.bIsProjectile = true;



		FVector damageBoxLocation = GetActorLocation() + GetActorForwardVector() * Bullets_DamageBoxOffset.X;


		UWorld* World = GetWorld();
		bool const bHit = World ? World-> SweepSingleByObjectType(DamageInfo.hitResult, 
			damageBoxLocation, damageBoxLocation, GetActorQuat(), ObjectParams, FCollisionShape::MakeBox(Bullets_DamageBoxExtend), Params) : false;

		//DrawDebugSphere(World, ArrowPosition, SPHERERADIUS, 8, FColor::Red);

		if (bHit)
		{
			AActor* damagedActor = DamageInfo.hitResult.GetActor();
			// Check what do we hit

			// if it is a person or shield
			ATheLastBastionCharacter* Character = Cast<ATheLastBastionCharacter>(damagedActor);
			if (Character)
			{   
				CurrentHitCount++;

				FVector damageCauserRelative = GearOwner->GetActorLocation()
					- DamageInfo.hitResult.GetActor()->GetActorLocation();
				damageCauserRelative.Z = 0.0f;
				DamageInfo.hitDirection = damageCauserRelative.GetUnsafeNormal();
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
					FVector attackLocation = GetActorLocation() + this->GetActorForwardVector() * StabInDistance;
					AProjectile* copyProjectile = GetWorld()->SpawnActor<AProjectile>(this->GetClass(), attackLocation, GetActorRotation(), spawnParam);
					copyProjectile->AttachToComponent
					(Character->GetMesh(), FAttachmentTransformRules::KeepWorldTransform, DamageInfo.hitResult.BoneName);
					Destroy();
				}
				else
				{
					IgnoredActors.Add(damagedActor);
				}

			}
			else
			{
				AShield* Shield = Cast<AShield>(damagedActor);
				if (Shield)
				{
					CurrentHitCount++;
					FVector AttachLocation = Shield->GetArrowAttachLocation(GetActorLocation());
					if (PenetrateLevel <= CurrentHitCount)
					{
						UE_LOG(LogTemp, Log, TEXT("Hit on Shield"));
						FActorSpawnParameters spawnParam;
						spawnParam.Owner = damagedActor;
						spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;						
						AProjectile* copyProjectile = 
							GetWorld()->SpawnActor<AProjectile>(this->GetClass(), AttachLocation, GetActorRotation(), spawnParam);
						copyProjectile->AttachToComponent(Shield->GetMesh(), FAttachmentTransformRules::KeepWorldTransform);

						//copyProjectile->MakeStatic();
						Destroy();
					}
					else
					{
						IgnoredActors.Add(damagedActor);
					}

					USoundCue* sfx = nullptr;
					UParticleSystem* vfx = nullptr;

					EPhysicalSurface surfaceType = UPhysicalMaterial::DetermineSurfaceType(DamageInfo.hitResult.PhysMaterial.Get());
					sfx = UAudioManager::GetProjectileImpactByMaterial(surfaceType);
					vfx = UVfxManager::GetVfxBySurfaceType(surfaceType);

					if (sfx && vfx)
					{
						UE_LOG(LogTemp, Log, TEXT("sfx && vfx - AProjectile::Tick"));
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), vfx, AttachLocation);
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), sfx, AttachLocation);
					}					
				}
				else
				{
					FActorSpawnParameters spawnParam;
					spawnParam.Owner = damagedActor;
					spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

					AProjectile* copyProjectile = GetWorld()->SpawnActor<AProjectile>(this->GetClass(), GetActorLocation(), GetActorRotation(), spawnParam);
					copyProjectile->AttachToActor(damagedActor, FAttachmentTransformRules::KeepWorldTransform);
					Destroy();

				}

			}
		}
	}

}

void AProjectile::SetDamageIsEnabled(bool _val)
{
	bDamageIsEnable = _val;
}
