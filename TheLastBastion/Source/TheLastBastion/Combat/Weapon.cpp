// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "Combat/PawnStatsComponent.h"


#define ECC_EnemyBody ECollisionChannel::ECC_GameTraceChannel3
#define ECC_HeroBody  ECollisionChannel::ECC_GameTraceChannel1

AWeapon::AWeapon() 
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Appearence"));
	Mesh->SetCollisionProfileName("EnemyWeapon");
	RootComponent = Mesh;
	DamageEdgeOffset_start = 95.0f;
	DamageEdgeOffset_end = 15.0f;
	DamageVolumnExtend = FVector(3.0f, 3.0f, 0.0f);
	PrimaryActorTick.bCanEverTick = true;
	bDamageIsEnable = false;
	bDisableCutOpenDamage = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Arm(USkeletalMeshComponent * const _skeletonMeshComponent)
{
	if (GearOwner == nullptr)
	{
		GearOwner = Cast<ATheLastBastionCharacter>(_skeletonMeshComponent->GetOwner());
		SetOwner(GearOwner);
	}

	FName SlotName;

	switch (GearType)
	{
	case EGearType::Armor:
	default:
		SlotName = TEXT("Root");
		break;
	case EGearType::Shield:
		SlotName = TEXT("Shield");
		break;
	case EGearType::SingleHandWeapon:
		SlotName = TEXT("SingleHandWeapon");
		break;
	case EGearType::DoubleHandWeapon:
		SlotName = TEXT("DHSwordEquip");
		break;
	case EGearType::CrossBow:
		SlotName = TEXT("CrossBow");
		break;
	case EGearType::CrossBowBolt:
		SlotName = TEXT("BoltsEquip");
		break;
	case EGearType::TwinBlade:
		SlotName = TEXT("ik_hand_r");
		break;

	}
	this->AttachToComponent(_skeletonMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, SlotName);
}

void AWeapon::SetDamageIsEnabled(bool _val)
{
	bDamageIsEnable = _val;
	if (!bDamageIsEnable)
	{
		// clear the ignore list for next attack
		IgnoredActors.Empty();
	}
}

void AWeapon::Tick(float _deltaTime)
{

	if (bDamageIsEnable)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Damage Enabled !!!!!!!!!!!!"));

		// Get Start End Position
		FVector startPosition, endPosition;
		switch (GearType)
		{
		case EGearType::SingleHandWeapon:
		case EGearType::DoubleHandWeapon:
		default:
		{
			startPosition = GetActorLocation() + GetActorUpVector() * DamageEdgeOffset_start;
			endPosition   = GetActorLocation() + GetActorUpVector() * DamageEdgeOffset_end;
			break;
		}

		case EGearType::TwinBlade:
		{
			startPosition = GetActorLocation() + GetActorForwardVector() * DamageEdgeOffset_start;
			endPosition   = GetActorLocation() + GetActorForwardVector() * DamageEdgeOffset_end;
			break;

		}
		}


		// Box Trace
		static const FName BoxTraceSingleName(TEXT("BoxTraceSingleForObjects"));

		FCollisionQueryParams Params;

		
		if (IgnoredActors.Num() > 0 && bDisableCutOpenDamage)
			Params.AddIgnoredActors(IgnoredActors);

		Params.bReturnPhysicalMaterial = true;
		Params.bTraceComplex = true;

		bool isEnemyCharacter = (Cast<ATheLastBastionEnemyCharacter>(GearOwner) != nullptr);

		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery((isEnemyCharacter)? ECC_HeroBody : ECC_EnemyBody);
		
		if (ObjectParams.IsValid() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid object types"));
			return;
		}

		FDamageInfo DamageInfo;
		DamageInfo.applyDamageType = EApplyDamageType::Point;
		DamageInfo.damageType = DamageType;

		UWorld* world = GetWorld();
		bool IsHit = world->SweepSingleByObjectType(DamageInfo.hitResult, startPosition, endPosition, FRotator::ZeroRotator.Quaternion(), ObjectParams,
			FCollisionShape::MakeBox(DamageVolumnExtend), Params);
		if (IsHit)
		{
			if (bDisableCutOpenDamage)
				IgnoredActors.Add(DamageInfo.hitResult.GetActor());
		
			DamageInfo.hitDirection = DamageInfo.hitResult.Location - DamageInfo.hitResult.GetActor()->GetActorLocation();
			UPawnStatsComponent* pSC = GearOwner->GetPawnStatsComp();
			if (pSC != nullptr)
				pSC->ApplyDamage(DamageInfo);
			else
				UE_LOG(LogTemp, Error, TEXT("UPawnStatsComponent is NuLL -- AWeapon::Tick "));
		}
	}

}


