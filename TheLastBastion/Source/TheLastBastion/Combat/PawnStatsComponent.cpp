// Fill out your copyright notice in the Description page of Project Settings.

#include "PawnStatsComponent.h"
#include "GameFramework/Character.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GI_TheLastBastion.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Combat/Weapon.h"
#include "Combat/Armor.h"
#include "CustomType.h"
#include "TheLastBastionCharacter.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "UI/InGameFloatingText.h"
#include "TheLastBastionHeroCharacter.h"


const float RangerInitHp = 230.0f;
const float BuilderInitHp = 180.0f;

static TSubclassOf<class UUserWidget> FloatingText_WBP;

// Sets default values for this component's properties
UPawnStatsComponent::UPawnStatsComponent()
{	
	PrimaryComponentTick.bCanEverTick = false;
	bGenerateRawStatsAtBeginPlay = true;
	bArmedFromBeginPlay = false;
	CurrentWeapon_Index = 0;

	if (!FloatingText_WBP)
		UCustomType::FindClass<UUserWidget>(FloatingText_WBP, TEXT("/Game/UI/In-Game/WBP_FloatingText"));
	
}

// Called when the game starts
void UPawnStatsComponent::BeginPlay()
{

	Super::BeginPlay();

	mCharacter = Cast<ATheLastBastionCharacter>(this->GetOwner());

	if (mCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error,
			TEXT("mCharacter must be a ATheLastBastionCharacter - UPawnStatsComponent::BeginPlay"));
		return;
	}

	//mCharacter->OnTakeAnyDamage.AddDynamic(this, &UPawnStatsComponent::OnTakeAnyDamageHandle);
	//mCharacter->OnTakePointDamage.AddDynamic(this, &UPawnStatsComponent::OnTakePointDamageHandle);

	GenerateStatsAtBeginPlay();
}

// Called every frame
void UPawnStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPawnStatsComponent::SetEnableWeapon(bool _bIsEnabled, bool _bIsRightHand, bool _bIsAll)
{

	AWeapon* rightWeapon = Cast<AWeapon>(WeaponSlots[CurrentWeapon_Index].RightHand);
	AWeapon* leftWeapon = Cast<AWeapon>(WeaponSlots[CurrentWeapon_Index].LeftHand);

	if (_bIsAll)
	{
		if (rightWeapon)
			rightWeapon->SetDamageIsEnabled(_bIsEnabled);

		if (leftWeapon)
			leftWeapon->SetDamageIsEnabled(_bIsEnabled);
	}
	else
	{
		if (_bIsRightHand && rightWeapon)
		{
			rightWeapon->SetDamageIsEnabled(_bIsEnabled);
		}
		else if (leftWeapon)
		{
			leftWeapon->SetDamageIsEnabled(_bIsEnabled);
		}
	}
}

void UPawnStatsComponent::OnEquipWeapon()
{
	//RightHandWeapon->Arm(mCharacter->GetMesh());
	WeaponSlots[CurrentWeapon_Index].RightHand->Arm(mCharacter->GetMesh());
	if (WeaponSlots[CurrentWeapon_Index].LeftHand)
		WeaponSlots[CurrentWeapon_Index].LeftHand->Arm(mCharacter->GetMesh());

}

void UPawnStatsComponent::OnSheathWeapon()
{
	if (WeaponSlots[CurrentWeapon_Index].RightHand)
	{
		WeaponSlots[CurrentWeapon_Index].RightHand->Equip(mCharacter->GetMesh());
		if (WeaponSlots[CurrentWeapon_Index].bHideWhenEquip)
			WeaponSlots[CurrentWeapon_Index].RightHand->ToggleVisibilty(false);
	}

}

void UPawnStatsComponent::OnKill()
{
	//if (RightHandWeapon)
	//	RightHandWeapon->Destroy();
	//if (LeftHandWeapon)
	//	LeftHandWeapon->Destroy();


	AGear* leftHand = nullptr, *rightHand = nullptr;
	for (int i = 0; i < 2; i++)
	{
		leftHand = WeaponSlots[i].LeftHand;
		rightHand = WeaponSlots[i].RightHand;
		if (leftHand)
			leftHand->Destroy();
		if (rightHand)
			rightHand->Destroy();
	}

	if (Armor)
		Armor->Destroy();
}

bool UPawnStatsComponent::OnSwapBetweenMeleeAndRange()
{
	return false;
}

#pragma region Stats Generatrion
void UPawnStatsComponent::GenerateRawStatsByLevel(int _level)
{
	Level = _level;
	_level--;
	// Just Avoid negative
	if (_level <= 0) _level = 0;

	StaminaRaw = -1;
	if (mCharacter)
	{
		switch (mCharacter->GetCharacterType())
		{
		case ECharacterType::None:
		default:
		{
			HpRaw = 500.0f + _level * 15;
		}
		break;
		case ECharacterType::Ranger:
		{
			HpRaw = RangerInitHp + _level * 15;
			StaminaRaw = 120.0f;
			break;
		}
		case ECharacterType::Builder:
		{
			HpRaw = BuilderInitHp + _level * 11.5f;
			StaminaRaw = 100.0f;
			break;
		}
		}
	}
	else
		UE_LOG(LogTemp, Error, TEXT("mCharacter is Null - UPawnStatsComponent::GenerateRawStatsByLevel"));


}

/** Generater Max stats after the gear is loaded, set current to max*/
void UPawnStatsComponent::GenerateMaxStats(bool _setCurrentToMax)
{
	float
		factorHp = 1,
		factorStamina = 1,
		HpAdd = 0,
		SpAdd = 0,
		criticalAdd = 0,
		stunAdd = 0;

	AGear* LeftHandWeapon = WeaponSlots[CurrentWeapon_Index].LeftHand;
	AGear* RightHandWeapon = WeaponSlots[CurrentWeapon_Index].RightHand;

	if (LeftHandWeapon)
	{
		factorHp += LeftHandWeapon->GetHpBonus();
		factorStamina += LeftHandWeapon->GetStaminaBonus();
		HpAdd += LeftHandWeapon->GetHpAdditive();
		SpAdd += LeftHandWeapon->GetSpAdditive();
		criticalAdd += LeftHandWeapon->GetCriticalChance();
		stunAdd += LeftHandWeapon->GetStunChance();
	}

	if (RightHandWeapon)
	{
		factorHp += RightHandWeapon->GetHpBonus();
		factorStamina += RightHandWeapon->GetStaminaBonus();
		HpAdd += RightHandWeapon->GetHpAdditive();
		SpAdd += RightHandWeapon->GetSpAdditive();
		criticalAdd += RightHandWeapon->GetCriticalChance();
		stunAdd += RightHandWeapon->GetStunChance();
	}

	if (Armor)
	{
		factorHp += Armor->GetHpBonus();
		factorStamina += Armor->GetStaminaBonus();
		HpAdd += Armor->GetHpAdditive();
		SpAdd += Armor->GetSpAdditive();
		criticalAdd += Armor->GetCriticalChance();
		stunAdd += Armor->GetStunChance();
	}


	HpMax = factorHp * HpRaw + HpAdd;
	StaminaMax = factorStamina * StaminaRaw + SpAdd;
	CriticalMax = CriticalRow + criticalAdd;
	StunMax = StunRow + stunAdd;

	if (_setCurrentToMax)
	{
		HpCurrent = HpMax;
		StaminaCurrent = StaminaMax;
	}
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

void UPawnStatsComponent::GenerateStatsAtBeginPlay()
{
	if (bGenerateRawStatsAtBeginPlay)
	{
		GenerateRawStatsByLevel(Level);
	}

	UWorld* world = GetWorld();
	// Equip the owner character
	if (world)
	{
		FActorSpawnParameters spawnParam;
		spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		spawnParam.Owner = mCharacter;

		if (Armor_ClassBp)
		{
			Armor = world->SpawnActor<AArmor>(Armor_ClassBp, spawnParam);
			Armor->Equip(mCharacter->GetMesh());
		}

		// use the weapon at the first slot at the beginning
		CurrentWeapon_Index = 0;
		if (WeaponSlots.Num() > 0)
		{
			for (int i = 0; i < WeaponSlots.Num(); i++)
			{
				// check if we have any weapon type on this slot
				TSubclassOf<AGear> RightHandWeapon_Class = WeaponSlots[i].WeaponClass;
				if (RightHandWeapon_Class)
				{
					WeaponSlots[i].RightHand = world->SpawnActor<AGear>(RightHandWeapon_Class, spawnParam);
					TSubclassOf<AGear> LeftHandWeapon_Class = WeaponSlots[i].RightHand->GetLeftHandGear();
					if (LeftHandWeapon_Class)
						WeaponSlots[i].LeftHand = world->SpawnActor<AGear>(LeftHandWeapon_Class, spawnParam);

					if (bArmedFromBeginPlay && i == CurrentWeapon_Index)
					{
						// Arm the first weapon if we try to arm this character from beginning
						WeaponSlots[i].RightHand->Arm(mCharacter->GetMesh());
						if (WeaponSlots[i].LeftHand)
							WeaponSlots[i].LeftHand->Arm(mCharacter->GetMesh());
					}
					else
					{
						// Equip the rest of other weapon, and toggle visibility
						WeaponSlots[i].RightHand->Equip(mCharacter->GetMesh());
						if (WeaponSlots[i].bHideWhenEquip)
							WeaponSlots[i].RightHand->ToggleVisibilty(false);
						if (WeaponSlots[i].LeftHand)
							WeaponSlots[i].LeftHand->Equip(mCharacter->GetMesh());
					}
				}
			}

			GenerateMaxStats();
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("%s does not equip with anyweapon - UPawnStatsComponent::GenerateStatsAtBeginPlay"), *mCharacter->GetName())


	}
}

#pragma endregion

#pragma region  Damage Calculation

TSubclassOf<class UUserWidget> UPawnStatsComponent::GetFloatingText_WBP()
{
	return FloatingText_WBP;
}

//float UPawnStatsComponent::CalculateHealth(AActor * _otherActor)
//{
//
//	AGear* const AttackerWeapon = Cast<AGear>(_otherActor);
//	if (AttackerWeapon == nullptr)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("AttackerWeapon is not a gear - UPawnStatsComponent::CalculateHealth "));
//		return 0.0f;
//	}
//	UPawnStatsComponent* const AttackerStats = AttackerWeapon->GetGearOwner()->GetPawnStatsComp();
//	float damage = 0.0f;
//	HpCurrent = HpCurrent - damage;
//	
//	float damagePercentage = damage * DivByHpMax;
//	return damagePercentage;
//}

float UPawnStatsComponent::CalculateDamage(float baseDamage, AActor * _damageCauser, bool & _isCritical, bool & _isStun)
{

	ATheLastBastionCharacter* damageCauser = Cast<ATheLastBastionCharacter>(_damageCauser); 
	if (damageCauser == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("damageCauser == nullptr - UPawnStatsComponent::CalculateDamage"));
	}
	UPawnStatsComponent* dCPawnStats = damageCauser->GetPawnStatsComp();
	if (dCPawnStats == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("dCPawnStats == nullptr - UPawnStatsComponent::CalculateDamage"));
	}

	float criticalRate = FMath::SRand();
	float stunRate = FMath::SRand();

	_isCritical = criticalRate< dCPawnStats->GetCriticalMax() * 0.01f;
	_isStun = stunRate < dCPawnStats->GetStunMax() * 0.01f;

	float totalDamage = baseDamage + FMath::RandRange(10, 100);


	if (!mCharacter->GetIsGodMode())
		HpCurrent = HpCurrent - totalDamage;

	HpCurrent = FMath::Clamp(HpCurrent, 0.0f, HpMax);

	UE_LOG(LogTemp, Log, TEXT("Enemy::OnTakePointDamageHandle, critical: %f, stun: %f"), criticalRate, stunRate);

	return totalDamage;
}

float UPawnStatsComponent::GetBaseDamage()
{
	return 5.0f;
}
#pragma endregion


///** Generate Raw Stats, equip geat, and Add Gear buff on raw stats*/
//void UPawnStatsComponent::GenerateFloatingText(const FVector& _worldPos, bool _isCritical, bool _isStun, bool _isHeadHit)
//{
//	FVector2D screenPos;
//	UGameplayStatics::ProjectWorldToScreen(
//		UGameplayStatics::GetPlayerController(this, 0),
//		_worldPos, screenPos);
//
//	UInGameFloatingText* damageFT = nullptr;
//	UWorld* world = GetWorld();
//
//	if (world)
//		damageFT = Cast<UInGameFloatingText>(CreateWidget<UUserWidget>(world, FloatingText_WBP));
//
//	if (damageFT == nullptr)
//	{
//		UE_LOG(LogTemp, Error, TEXT("Create Floating Text Failed"));
//		return;
//	}
//	damageFT->SetRenderTranslation(screenPos);
//}

void UPawnStatsComponent::ApplyDamage(const FDamageInfo& _damageInfo)
{
	// calculate the damage based on Gears
	float damage = GetBaseDamage();

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
			UGameplayStatics::ApplyPointDamage(
				_damageInfo.hitResult.GetActor(), damage, _damageInfo.hitDirection, _damageInfo.hitResult,
				mCharacter->GetInstigatorController(), mCharacter, _damageInfo.damageType);
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
/////*** DEPRECATED
//void UPawnStatsComponent::OnTakeAnyDamageHandle(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
//{
//
//}
/////*** DEPRECATED
//void UPawnStatsComponent::OnTakePointDamageHandle(AActor * DamagedActor, float Damage, AController * InstigatedBy, FVector HitLocation, UPrimitiveComponent * FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType * DamageType, AActor * DamageCauser)
//{
//
//	if (mCharacter->GetIsDead())
//	{
//		return;
//	}
//
//	//float totalDamage = Damage + CalculateDamage();
//
//	if (!mCharacter->GetIsGodMode())
//	{
//		//HpCurrent = HpCurrent - totalDamage;
//	}
//	HpCurrent = FMath::Clamp(HpCurrent, 0.0f, HpMax);
//
//	//mCharacter->OnTakingDamage(DamageCauser, BoneName, ShotFromDirection, HitLocation, DamageType);
//
//	//ATheLastBastionHeroCharacter* heroAttacker = Cast<ATheLastBastionHeroCharacter>(DamageCauser);
//	//bool isAttackByHero = heroAttacker != nullptr;
//	//ATheLastBastionHeroCharacter* damagedHero = Cast<ATheLastBastionHeroCharacter>(DamagedActor);
//	//bool isHeroBeAttacked = damagedHero != nullptr;
//
//
//	//// Let Character class to handle Updating the HUD display
//	//OnHealthChanged.Broadcast(this, totalDamage, DamageType, BoneName, ShotFromDirection, HitLocation);
//	//
//
//	// Create Floating damage point, if this is damage done by hero
//	//if (hero)
//	//{
//	//	UInGameFloatingText* floatingDamage = GenerateFloatingText(HitLocation);
//	//	if (floatingDamage)
//	//	{
//	//		floatingDamage->SetInGameFTProperty(FText::AsNumber((int)totalDamage));
//	//		floatingDamage->AddToViewport();
//	//	}
//
//	//	//mCharacter->OnHeal
//	//}
//}

AGear * UPawnStatsComponent::GetCurrentArmor() const
{
	return Armor;
}


