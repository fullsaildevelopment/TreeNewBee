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
	BulletSpread = 5.0f;
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
		DrawDebugLine(GetWorld(), EyesLocation, TraceEnd, FColor::Red, false, 2.0f, 0, 1.0f);

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
		DrawDebugLine(GetWorld(), LaunchLocation, TraceEnd, FColor::Green, false, 2.0f, 0, 1.0f);
	}
}

void ARangeWeapon::NPCFire(const AActor* _target)
{

	GearOwner = Cast<ATheLastBastionCharacter>(GetOwner());

	// Trace the world from pawn eyes to crosshair location
	if (GearOwner != nullptr && ProjectileClassBP != nullptr)
	{
		const AActor* TargetActor = _target;
		// Correctly Spawn the projectile
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);

		// Create and Initialize the spawn parameters for the projectile
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = this;

		AProjectile* CrossbowProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClassBP, MuzzleLocation, MuzzleRotation, SpawnParams);

		// Calculate the velocity for the projectile
		FVector FlyDir = (TargetActor->GetActorLocation() - MuzzleLocation).GetSafeNormal();

		// Bullet Spread
		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		FlyDir = FMath::VRandCone(FlyDir, HalfRad, HalfRad);

		CrossbowProjectile->GetProjectileMovementComp()->Velocity = FlyDir * BulletSpeed;

	}





	//ATheLastBastionEnemyCharacter* RangeEnemyCharacter = Cast<ATheLastBastionEnemyCharacter>(GearOwner);

	//// Check if the gearowner is a enemy character
	//if (RangeEnemyCharacter)
	//{
	//	// Get the AI Controller from the enemy
	//	ATheLastBastionBaseAIController* EnemyAIController = Cast<ATheLastBastionBaseAIController>(RangeEnemyCharacter->GetController());
	//	if (EnemyAIController)
	//	{
	//		// Get Enemy's target actor location to initialize velocity for projectile
	//		UBehaviorTreeComponent* BehaviorTreeComp = EnemyAIController->GetBTComp();
	//		if (BehaviorTreeComp)
	//		{
	//			UBlackboardComponent* BlackboardComp = BehaviorTreeComp->GetBlackboardComponent();
	//			if (BlackboardComp)
	//			{
	//			}
	//			else
	//			{
	//				UE_LOG(LogTemp, Warning, TEXT("Can't get Blackboard Component from BehaviorTree Component"));
	//				return;
	//			}

	//		}
	//		else
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("Can't find Enemy AI BehaviorTreeComp"));
	//			return;
	//		}

	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Can't find Enemy AI Controller"));
	//		return;
	//	}
	//}
}



