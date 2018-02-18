// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "Combat/PawnStatsComponent.h"
#include "Kismet/KismetSystemLibrary.h"



AWeapon::AWeapon() 
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Appearence"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = Mesh;
	DamageEdgeOffset_start = FVector(0, 0, 95.0f);
	DamageEdgeOffset_end = FVector(0, 0, 15.0f);
	DamageVolumnExtend = FVector(1.0f, 1.0f, 0.0f);
	PrimaryActorTick.bCanEverTick = true;
	bDamageIsEnable = false;
	bEnableCutOpenDamage = false;
	bShowBounding = false;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::SetDamageIsEnabled(bool _val)
{
	bDamageIsEnable = _val;
	if (bDamageIsEnable)
	{
		// clear the ignore list for next attack
		IgnoredActors.Empty();
	}
}

void AWeapon::Tick(float _deltaTime)
{
	if (bShowBounding)
	{
		FVector startPosition, endPosition;
		//FRotator rotation;

		//startPosition = GetActorLocation() + DamageEdgeOffset_start;
		//endPosition   = GetActorLocation() + DamageEdgeOffset_end;
		GetRayCastPosition(startPosition, endPosition);



		FHitResult _hit;
		const TArray<AActor* > ignoreActors;
		UKismetSystemLibrary::BoxTraceSingle(GetWorld(), startPosition, endPosition, DamageVolumnExtend, GetActorRotation(),
			ETraceTypeQuery::TraceTypeQuery1, false, ignoreActors, EDrawDebugTrace::ForOneFrame, _hit, true);
	}


	if (bDamageIsEnable)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Damage Enabled !!!!!!!!!!!!"));

		// Get Start End Position
		FVector startPosition, endPosition;
		//FRotator rotation;

		GetRayCastPosition(startPosition, endPosition);

		FCollisionQueryParams Params;	
		if (IgnoredActors.Num() > 0 && !bEnableCutOpenDamage)
			Params.AddIgnoredActors(IgnoredActors);

		Params.bReturnPhysicalMaterial = true;
		Params.bTraceComplex = true;

		bool isEnemyCharacter = (Cast<ATheLastBastionEnemyCharacter>(GearOwner) != nullptr);

		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery((isEnemyCharacter)? ECC_HeroBody : ECC_EnemyBody);
		
		if (ObjectParams.IsValid() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid object types - AWeapon::Tick"));
			return;
		}

		FDamageInfo DamageInfo;
		DamageInfo.applyDamageType = EApplyDamageType::Point;
		DamageInfo.damageType = DamageType;
		DamageInfo.bIsProjectile = false;

		UWorld* world = GetWorld();
		bool IsHit = world->SweepSingleByObjectType(DamageInfo.hitResult, startPosition, endPosition, GetActorRotation().Quaternion(), ObjectParams,
			FCollisionShape::MakeBox(DamageVolumnExtend), Params);
		if (IsHit)
		{
			if (!bEnableCutOpenDamage)
				IgnoredActors.Add(DamageInfo.hitResult.GetActor());
		

			FVector damageCauserRelative = GearOwner->GetActorLocation()
				- DamageInfo.hitResult.GetActor()->GetActorLocation();
			damageCauserRelative.Z = 0.0f;
			DamageInfo.hitDirection = damageCauserRelative.GetUnsafeNormal();

			UPawnStatsComponent* pSC = GearOwner->GetPawnStatsComp();
			if (pSC != nullptr)
				pSC->ApplyDamage(DamageInfo);
			else
				UE_LOG(LogTemp, Error, TEXT("UPawnStatsComponent is NuLL -- AWeapon::Tick "));
		}
	}
}

USceneComponent * AWeapon::GetMesh() const
{
	return Mesh;
}

void AWeapon::GetRayCastPosition(FVector & _start, FVector & _end)
{

	switch (GearType)
	{
	case EGearType::LongSword:
	case EGearType::WarAxe:
	case EGearType::Mace:
	{
		_start = GetActorLocation() + GetActorUpVector() * DamageEdgeOffset_start.Z;
		_end = GetActorLocation() + GetActorUpVector() * DamageEdgeOffset_end.Z;
		//_rotator = this->GetActorRotation();
		break;
	}
	case EGearType::DoubleHandWeapon:
	{
		_start = GetActorLocation() + GetActorRightVector() * DamageEdgeOffset_start.Y + GetActorUpVector() * DamageEdgeOffset_start.Z;;
		_end = GetActorLocation() + GetActorRightVector() * DamageEdgeOffset_end.Y;
		//_rotator = this->GetActorRotation();
		break;
	}
	case EGearType::GreatSword:
	case EGearType::BattleAxe:
	case EGearType::Hammer:
	{

		_start = GetActorLocation() + GetActorRightVector() * DamageEdgeOffset_start.Y + GetActorUpVector() * DamageEdgeOffset_start.Z;;
		_end = GetActorLocation() + GetActorRightVector() * DamageEdgeOffset_end.Y;
		//_rotator = this->GetActorRotation();

		//_start = GetActorLocation() - GetActorRightVector() * DamageEdgeOffset_start.Y   + GetActorForwardVector() * DamageEdgeOffset_start;
		//_end = GetActorLocation()   - GetActorRightVector() * DamageEdgeOffset_end.Y     + GetActorForwardVector() * DamageEdgeOffset_end;
		//_rotator = FRotator(this->GetActorRotation().Pitch, -this->GetActorRotation().Yaw, this->GetActorRotation().Roll);
		break;
	}
	case EGearType::TwinBlade:
	case EGearType::Shield:
	{
		_start = GetActorLocation() + GetActorForwardVector() * DamageEdgeOffset_start;
		_end = GetActorLocation()   + GetActorForwardVector() * DamageEdgeOffset_end;
		//_rotator = this->GetActorRotation();

		break;
	}
	default:
		break;
	}

}


