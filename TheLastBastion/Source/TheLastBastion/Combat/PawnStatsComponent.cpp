// Fill out your copyright notice in the Description page of Project Settings.

#include "PawnStatsComponent.h"
#include "GameFramework/Character.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GI_TheLastBastion.h"
#include "Kismet/GameplayStatics.h"

#include "Combat/Weapon.h"
#include "Combat/Armor.h"
#include "CustomType.h"
#include "TheLastBastionCharacter.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

const float RangerInitHp = 230.0f;
const float BuilderInitHp = 180.0f;

// Sets default values for this component's properties
UPawnStatsComponent::UPawnStatsComponent()
{	
}

// Called when the game starts
void UPawnStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	mCharacter = Cast<ATheLastBastionCharacter>(this->GetOwner());

	Level = 1;

	UWorld* world = GetWorld();

	// Equip the owner character
	if (world)
	{
		if (LeftHandWeapon_ClassBp)
		{
			LeftHandWeapon = world->SpawnActor<AWeapon>(LeftHandWeapon_ClassBp);
			LeftHandWeapon->Equip(mCharacter->GetMesh());
			//LeftHandWeapon->AttachToComponent(mCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Shield"));
		}

		if (RightHandWeapon_ClassBp)
		{
			RightHandWeapon = world->SpawnActor<AWeapon>(RightHandWeapon_ClassBp);
			RightHandWeapon->Equip(mCharacter->GetMesh());
			//RightHandWeapon->AttachToComponent(mCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("SHSwordEquip"));
		}

		if (Armor_ClassBp)
		{
			Armor = world->SpawnActor<AArmor>(Armor_ClassBp);
			//Armor->AttachToComponent(mCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Root"));
			Armor->Equip(mCharacter->GetMesh());
		}
	}

	GenerateRawStatsByLevel(1);
}

// Called every frame
void UPawnStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPawnStatsComponent::EnableWeapon(bool _bIsRightHand, bool _bIsAll)
{

	//if (_bIsAll)
	//{
	//	LeftHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//	RightHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//}
	//else
	//{
	//	if (_bIsRightHand)
	//		RightHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//	else
	//		LeftHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//}
	UE_LOG(LogTemp, Warning, TEXT("Enable Weapon"));
	RightHandWeapon->SetDamageIsEnabled(true);
}

void UPawnStatsComponent::DisableWeapon(bool _bIsRightHand, bool _bIsAll)
{
	RightHandWeapon->SetDamageIsEnabled(false);

	//UE_LOG(LogTemp, Warning, TEXT("disable Weapon"));

	//if (_bIsAll)
	//{
	//	LeftHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//	RightHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//}
	//else
	//{
	//	if (_bIsRightHand)
	//		RightHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//	else
	//		LeftHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//}

}

void UPawnStatsComponent::OnEquipWeapon()
{
	RightHandWeapon->Arm(mCharacter->GetMesh());
}

void UPawnStatsComponent::OnSheathWeapon()
{
	RightHandWeapon->Equip(mCharacter->GetMesh());

	//RightHandWeapon->AttachToComponent(mCharacter->GetMesh(),
	//	FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("SHSwordEquip"));
}




#pragma region Stats Generatrion
void UPawnStatsComponent::GenerateRawStatsByLevel(int Level)
{

	Level--;
	// Just Avoid negative
	if (Level < 0) Level = 0;

	StaminaRaw = -1;

	switch (mCharacter->GetCharacterType())
	{
	case ECharacterType::None:
	default:
		break;
	case ECharacterType::Ranger:
	{
		HpRaw = 230.0f + Level * 15;
		StaminaRaw = 100.0f;
		break;
	}
	case ECharacterType::Builder:
	{
		HpRaw = 180.0f + Level * 11.5f;
		StaminaRaw = 100.0f;
		break;
	}
	}

	GenerateMaxStats();
}

void UPawnStatsComponent::GenerateMaxStats()
{
	float
		factorHp = 1,
		factorStamina = 1;
	if (LeftHandWeapon)
	{
		factorHp += LeftHandWeapon->GetHpBonus();
		factorStamina += LeftHandWeapon->GetStaminaBonus();
	}

	if (RightHandWeapon)
	{
		factorHp += RightHandWeapon->GetHpBonus();
		factorStamina += RightHandWeapon->GetStaminaBonus();
	}

	if (Armor)
	{
		factorHp += Armor->GetHpBonus();
		factorStamina += Armor->GetStaminaBonus();
	}


	HpMax = factorHp * HpRaw;
	StaminaMax = factorStamina * StaminaRaw;
	DivByHpMax = 1 / HpMax;
	DivByStaminaMax = 1 / StaminaMax;
}

void UPawnStatsComponent::LevelUp()
{
	Level++;
	GenerateRawStatsByLevel(Level);
	HpCurrent = HpMax;
	StaminaCurrent = StaminaMax;
}

void UPawnStatsComponent::Born()
{
	Level = 1;
	GenerateRawStatsByLevel(Level);
	HpCurrent = HpMax;
	StaminaCurrent = StaminaMax;
}
#pragma endregion


#pragma region  Damage Calculation

float UPawnStatsComponent::CalculateHealth(AActor * _otherActor)
{

	AGear* const AttackerWeapon = Cast<AGear>(_otherActor);
	if (AttackerWeapon == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackerWeapon is not a gear - UPawnStatsComponent::CalculateHealth "));
		return 0.0f;
	}
	UPawnStatsComponent* const AttackerStats = AttackerWeapon->GetGearOwner()->GetPawnStatsComp();
	float damage = 0.0f;
	HpCurrent = HpCurrent - damage;
	
	float damagePercentage = damage * DivByHpMax;
	return damagePercentage;
}

float UPawnStatsComponent::CalculateDamage(const FDamageInfo & _hit)
{

	if (_hit.hitResult.BoneName.Compare(TEXT("neck_01")) == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Head Hit"));
	}
	else if (_hit.hitResult.BoneName.Compare(TEXT("spine_03")) == 0 || _hit.hitResult.BoneName.Compare(TEXT("spine_01")) == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Chest Hit"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Common Hit"));
	}

	return 	FMath::RandRange(50, 100);
}

void UPawnStatsComponent::ApplyDamage(const FDamageInfo & _damageInfo)
{
	// calculate the damage based on Gears
	float damage = CalculateDamage(_damageInfo);

	// apply damage type based on weapon 
	switch (_damageInfo.applyDamageType)
	{
		case EApplyDamageType::Common:
		default:
		{
			UGameplayStatics::ApplyDamage(
				_damageInfo.hitResult.GetActor(), damage,
				mCharacter->GetInstigatorController(), mCharacter, _damageInfo.damageType);
			break;
		}
		case EApplyDamageType::Point:
		{
			break;
		}
		case EApplyDamageType::Radius:
		{
			break;
		}
		case EApplyDamageType::RadiusFallOf:
		{
			break;
		}

	}

	// apply VFX based on surface
	UParticleSystem* vfxSelected = nullptr;

	UGI_TheLastBastion* gi = Cast<UGI_TheLastBastion>(UGameplayStatics::GetGameInstance(GetWorld()));
	EPhysicalSurface surefaceType = UPhysicalMaterial::DetermineSurfaceType(_damageInfo.hitResult.PhysMaterial.Get());
	switch (surefaceType)
	{
	case SURFACE_FLESH:
		vfxSelected = gi->GetVFX_BloodImpact();
		break;
	case SURFACE_METAL:
		break;
	default:
		break;
	}

	if (vfxSelected)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), vfxSelected, _damageInfo.hitResult.Location);

}



#pragma endregion





