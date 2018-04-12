// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "Combat/PawnStatsComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AudioManager.h"
#include "Kismet/GameplayStatics.h"
#include "VfxManager.h"
#include "Particles/ParticleSystemComponent.h"


AWeapon::AWeapon() 
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Appearence"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = Mesh;
	DamageEdgeOffset_start = FVector(0, 0, 95.0f);
	DamageEdgeOffset_end = FVector(0, 0, 15.0f);
	DamageVolumnExtend = FVector(1.0f, 1.0f, 0.0f);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bDamageIsEnable = false;
	bEnableCutOpenDamage = false;
	bShowBounding = false;

	// Particle System Component
	ParticleSystemComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComp"));
	ParticleSystemComp->SetupAttachment(Mesh);
	ParticleSystemComp->RelativeLocation = FVector::ZeroVector;

	//
	//bIsWeaponOnFire = false;
	//ParticleEffectCylinderHeight = 0.0f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	//Calculate Cylinder Height
	FVector BladeBottomLocation = Mesh->GetSocketLocation(TEXT("TrailStart"));
	FVector BladeTopLocation = Mesh->GetSocketLocation(TEXT("TrailEnd"));
	ParticleEffectCylinderHeight = (BladeBottomLocation.X - BladeTopLocation.X) +
		(BladeBottomLocation.Y - BladeTopLocation.Y) + (BladeBottomLocation.Z - BladeTopLocation.Z);
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

	//UE_LOG(LogTemp, Log, TEXT("AWeapon::Tick - %s"), *GetName());

	if (bShowBounding)
	{
		FVector startPosition, endPosition;

 		GetRayCastPosition(startPosition, endPosition);

		FHitResult _hit;
		const TArray<AActor* > ignoreActors;
		UKismetSystemLibrary::BoxTraceSingle(GetWorld(), startPosition, endPosition, DamageVolumnExtend_Current, GetActorRotation(),
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
		bool IsHit = world->SweepSingleByObjectType(DamageInfo.hitResult, startPosition, endPosition, GetActorQuat(), ObjectParams,
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

void AWeapon::GetRayCastPosition(FVector & _start, FVector & _end)
{

	switch (GearType)
	{
	case EGearType::LongSword:
	case EGearType::WarAxe:
	case EGearType::Mace:
	{
		_start = GetActorLocation() + GetActorUpVector() * DamageEdgeOffset_start_Current.Z;
		_end = GetActorLocation() + GetActorUpVector() * DamageEdgeOffset_end_Current.Z;
		//_rotator = this->GetActorRotation();
		break;
	}
	case EGearType::DoubleHandWeapon:
	{
		_start = GetActorLocation() + GetActorRightVector() * DamageEdgeOffset_start_Current.Y + GetActorUpVector() * DamageEdgeOffset_start_Current.Z;;
		_end = GetActorLocation() + GetActorRightVector() * DamageEdgeOffset_end_Current.Y;
		//_rotator = this->GetActorRotation();
		break;
	}
	case EGearType::GreatSword:
	case EGearType::BattleAxe:
	case EGearType::Hammer:
	{

		//_start = GetActorLocation() + GetActorRightVector() * DamageEdgeOffset_start.Y + GetActorUpVector() * DamageEdgeOffset_start.Z;;
		//_end = GetActorLocation() + GetActorRightVector() * DamageEdgeOffset_end.Y;
		////_rotator = this->GetActorRotation();

		_start = GetActorLocation() + GetActorRightVector() * DamageEdgeOffset_start_Current.Y   
			+ GetActorForwardVector() * DamageEdgeOffset_start_Current.X;
		_end = GetActorLocation()   + GetActorRightVector() * DamageEdgeOffset_end_Current.Y    
			+ GetActorForwardVector() * DamageEdgeOffset_end_Current.X;
		//_rotator = FRotator(this->GetActorRotation().Pitch, -this->GetActorRotation().Yaw, this->GetActorRotation().Roll);
		break;
	}
	case EGearType::TwinBlade:
	{
		_start = GetActorLocation() + GetActorForwardVector() * DamageEdgeOffset_start_Current;
		_end = GetActorLocation() + GetActorForwardVector() * DamageEdgeOffset_end_Current;
	}
	case EGearType::Shield:
	{
		_start = GetActorLocation() + DamageEdgeOffset_start_Current;
		_end = GetActorLocation()   + DamageEdgeOffset_end_Current;
		break;
	}
	default:
		break;
	}

}

UParticleSystemComponent * AWeapon::OnWeaponEnchant()
{

	UParticleSystem* FireEffect = UVfxManager::GetVfx(EVfxType::WeaponFireEnchantment);
	if (FireEffect)
	{
		FRotator relativeRotator = FRotator::ZeroRotator;
		switch (GearType)
		{

		case EGearType::DoubleHandWeapon:
			relativeRotator = FRotator(0, 0, 90);
			break;
		case EGearType::GreatSword:
		case EGearType::BattleAxe:
		case EGearType::Hammer:
			relativeRotator = FRotator(0, 0, -90);
			break;
		default:
			break;
		}
		UParticleSystemComponent* WeaponEnchantment_PSC = UGameplayStatics::SpawnEmitterAttached(FireEffect, Mesh, TEXT("EffectCenter"), FVector::ZeroVector, relativeRotator);
		WeaponEnchantment_PSC->SetFloatParameter(TEXT("CylinderHeight"), ParticleEffectCylinderHeight);
		return WeaponEnchantment_PSC;
	}
	else
	{
		return nullptr;
	}
}

void AWeapon::MaintainScale()
{
	FVector scale = GetGearOwner()->GetActorScale3D();

	DamageEdgeOffset_start_Current = DamageEdgeOffset_start * scale;
	DamageEdgeOffset_end_Current   = DamageEdgeOffset_end * scale;
	DamageVolumnExtend_Current     = DamageVolumnExtend * scale;
}

//void AWeapon::StartWeaponFireEnchantment()
//{   
//
//	// Calculate Cylinder Height
//	FVector BladeBottomLocation = Mesh->GetSocketLocation(TEXT("TrailStart"));
//	FVector BladeTopLocation = Mesh->GetSocketLocation(TEXT("TrailEnd"));
//	float ParticleEffectCylinderHeight = (BladeBottomLocation.X - BladeTopLocation.X) + 
//		(BladeBottomLocation.Y - BladeTopLocation.Y) + (BladeBottomLocation.Z - BladeTopLocation.Z);
//
//
//	UParticleSystem* FireEffect = UVfxManager::GetVfx(EVfxType::WeaponFireEnchantment);
//	if (FireEffect)
//	{
//		FRotator relativeRotator = FRotator::ZeroRotator;
//		switch (GearType)
//		{
//
//		case EGearType::DoubleHandWeapon:
//			relativeRotator = FRotator(0, 0, 90);
//			break;
//		case EGearType::GreatSword:
//		case EGearType::BattleAxe:
//		case EGearType::Hammer:
//			relativeRotator = FRotator(0, 0, -90);
//			break;
//		default:
//			break;
//		}
//		//WeaponEnchantment_PSC = UGameplayStatics::SpawnEmitterAttached(FireEffect, Mesh, TEXT("EffectCenter"), FVector::ZeroVector, relativeRotator);
//		//WeaponEnchantment_PSC->SetFloatParameter(TEXT("CylinderHeight"), ParticleEffectCylinderHeight);
//	}
//}

//void AWeapon::EndWeaponFireEnchantment()
//{
//	if (WeaponEnchantment_PSC)
//	{   
//		WeaponEnchantment_PSC->DestroyComponent();
//	}
//}



