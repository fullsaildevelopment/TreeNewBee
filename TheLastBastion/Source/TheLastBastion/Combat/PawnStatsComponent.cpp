// Fill out your copyright notice in the Description page of Project Settings.

#include "PawnStatsComponent.h"
#include "GameFramework/Character.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

#include "Combat/Weapon.h"
#include "Combat/Armor.h"
#include "CustomType.h"
#include "TheLastBastionCharacter.h"

const float RangerInitHp = 230.0f;
const float BuilderInitHp = 180.0f;


// Sets default values for this component's properties
UPawnStatsComponent::UPawnStatsComponent()
{
	
	mCharacter = Cast<ATheLastBastionCharacter>(this->GetOwner());

	Level = 1;

	if (mCharacter)
	{
		Head = mCharacter->GetHeadComp();
		Body = mCharacter->GetBodyComp();
	}

}

// Called when the game starts
void UPawnStatsComponent::BeginPlay()
{
	Super::BeginPlay();

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

	if (Head && Body) 
	{
		Head->OnComponentBeginOverlap.AddDynamic(this, &UPawnStatsComponent::OnHeadHit);
		Body->OnComponentBeginOverlap.AddDynamic(this, &UPawnStatsComponent::OnBodyHit);
	}

	if (LeftHandWeapon)
		LeftHandWeapon->GetWeaponMeshRef()
			->OnComponentBeginOverlap.AddDynamic(this, &UPawnStatsComponent::OnLeftHandWeaponHit);

	if (RightHandWeapon)
		RightHandWeapon->GetWeaponMeshRef()
		->OnComponentBeginOverlap.AddDynamic(this, &UPawnStatsComponent::OnRightHandWeaponHit);

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

	//UE_LOG(LogTemp, Warning, TEXT("enable Weapon"));
	if (_bIsAll)
	{
		LeftHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		RightHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		if (_bIsRightHand)
			RightHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		else
			LeftHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

}

void UPawnStatsComponent::DisableWeapon(bool _bIsRightHand, bool _bIsAll)
{
	//UE_LOG(LogTemp, Warning, TEXT("disable Weapon"));

	if (_bIsAll)
	{
		LeftHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RightHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		if (_bIsRightHand)
			RightHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		else
			LeftHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

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

void UPawnStatsComponent::SetDamageDetectorsCollsionProfile(FName _profileName)
{
	Head->SetCollisionProfileName(_profileName);
	Body->SetCollisionProfileName(_profileName);
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
	float damage = AttackerStats->GetDamage();;
	HpCurrent = HpCurrent - damage;
	
	float damagePercentage = damage * DivByHpMax;
	return damagePercentage;
}
float UPawnStatsComponent::GetDamage()
{
	return 	FMath::RandRange(50, 100);

}

#pragma endregion


#pragma region hit Event
void UPawnStatsComponent::OnBodyHit(UPrimitiveComponent * _overlappedComponent, AActor * _otherActor, UPrimitiveComponent * _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult & _SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Body hit by %s"), *_otherActor->GetName());

}

void UPawnStatsComponent::OnHeadHit(UPrimitiveComponent * _overlappedComponent, AActor * _otherActor, UPrimitiveComponent * _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult & _SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Head hit by %s"), *_otherActor->GetName());

}

void UPawnStatsComponent::OnRightHandWeaponHit(UPrimitiveComponent * _overlappedComponent, AActor * _otherActor, UPrimitiveComponent * _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult & _SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Right Hand Weapon hit %s"), *_otherActor->GetName());

}

void UPawnStatsComponent::OnLeftHandWeaponHit(UPrimitiveComponent * _overlappedComponent, AActor * _otherActor, UPrimitiveComponent * _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult & _SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Left Hand Weapon hit %s"), *_otherActor->GetName());

}

#pragma endregion




