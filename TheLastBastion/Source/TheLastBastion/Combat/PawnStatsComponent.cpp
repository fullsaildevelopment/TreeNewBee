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
#include "Sound/SoundCue.h"
#include "VfxManager.h"
#include "AudioManager.h"
#include "Components/AudioComponent.h"



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
	//UE_LOG(LogTemp, Log, TEXT("SetEnableWeapon(_bIsEnabled %d, _bIsRightHand  %d, _bIsAll  %d)"), _bIsEnabled, _bIsRightHand, _bIsAll);
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

		if (WeaponSlots[CurrentWeapon_Index].LeftHand)
		{
			WeaponSlots[CurrentWeapon_Index].LeftHand->Equip(mCharacter->GetMesh());
		}
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
			DpCurrent = 0;
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
	//DpCurrent = 0;

	HpCurrent = (HpCurrent > HpMax) ? HpMax : HpCurrent;
	DpCurrent = (DpCurrent > HpMax) ? HpMax : DpCurrent;
	StaminaCurrent = (StaminaCurrent > StaminaMax) ? StaminaMax : StaminaCurrent;

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

	const AGear* dcRightHandWeapon = dCPawnStats->GetCurrentRightHandWeapon();
	const AGear* dcLeftHandWeapon = dCPawnStats->GetCurrentLeftHandWeapon();
	if (dcRightHandWeapon == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("dcRightHandWeapon == nullptr - UPawnStatsComponent::CalculateDamage"));
		return 0;
	}

	// For now we only consider physical damage
	float weaponDamage = dcRightHandWeapon->GetPhysicalDamage();

	if (dcLeftHandWeapon)
		weaponDamage += dcLeftHandWeapon->GetPhysicalDamage();

	float totalDamage = baseDamage + FMath::RandRange(10, 100) + weaponDamage;

	if (DpCurrent > 0)
	{
		// if we have defence point, we are going to go reduce health
		// total damage will be applied by dp Physical damage reduction
		totalDamage *= DpPhysicalDamageReduction;
		DpCurrent = DpCurrent - totalDamage;
		DpCurrent = FMath::Clamp(DpCurrent, 0.0f, HpMax);
	}
	else
	{
		if (!mCharacter->GetIsGodMode())
			HpCurrent = HpCurrent - totalDamage;

		HpCurrent = FMath::Clamp(HpCurrent, 0.0f, HpMax);
	}

	//UE_LOG(LogTemp, Log, TEXT("Enemy::OnTakePointDamageHandle, critical: %f, stun: %f"), criticalRate, stunRate);

	return totalDamage;
}

float UPawnStatsComponent::GetBaseDamage()
{
	return 5.0f;
}
#pragma endregion

void UPawnStatsComponent::PlaySFXForImpact(USoundCue* _sfx, int _surfaceType, ATheLastBastionCharacter* _damagedCharacter) const
{   
	if (_damagedCharacter->GetIsDead() == true)
		return;
	
	switch (_surfaceType)
	{
	case SURFACE_FLESH:
	{
		UAudioComponent* AudioComp = _damagedCharacter->GetAudioComp();
		AudioComp->SetSound(_sfx);
		AudioComp->SetIntParameter(TEXT("DamagedActorType"), (int)_damagedCharacter->GetCharacterVoiceType());
		AudioComp->SetIntParameter(TEXT("SurfaceType"), 0);
		AudioComp->AttenuationSettings = _sfx->AttenuationSettings;
		AudioComp->Play();
	}

	case SURFACE_METAL:
	{
		UAudioComponent* AudioComp = _damagedCharacter->GetAudioComp();
		AudioComp->SetSound(_sfx);
		AudioComp->SetIntParameter(TEXT("DamagedActorType"), (int)_damagedCharacter->GetCharacterVoiceType());
		AudioComp->SetIntParameter(TEXT("SurfaceType"), 1);
		AudioComp->AttenuationSettings = _sfx->AttenuationSettings;
		AudioComp->Play();
	}

	case SURFACE_LightShield:
	{
		UAudioComponent* AudioComp = _damagedCharacter->GetAudioComp();
		AudioComp->SetSound(_sfx);
		AudioComp->SetIntParameter(TEXT("SurfaceType"), 0);
		AudioComp->AttenuationSettings = _sfx->AttenuationSettings;
		AudioComp->Play();
	}

	case SURFACE_HeavyShield:
	{
		UAudioComponent* AudioComp = _damagedCharacter->GetAudioComp();
		AudioComp->SetSound(_sfx);
		AudioComp->SetIntParameter(TEXT("SurfaceType"), 1);
		AudioComp->AttenuationSettings = _sfx->AttenuationSettings;
		AudioComp->Play();
	}

	default:
		break;
	}
}


bool UPawnStatsComponent::ApplyDamage(const FDamageInfo& _damageInfo)
{
	// calculate the damage based on Gears
	float damage = GetBaseDamage();


	// apply VFX based on surface
	UParticleSystem* vfxSelected = nullptr;
	USoundCue* sfxSelected = nullptr;

	// Use damageInfo find damagedActor and hitted surface
	ATheLastBastionCharacter* damageActor = Cast<ATheLastBastionCharacter>(_damageInfo.hitResult.GetActor());
	EPhysicalSurface surfaceType = UPhysicalMaterial::DetermineSurfaceType(_damageInfo.hitResult.PhysMaterial.Get());

	// Check if this damage is caused by melee and been countered
	if (_damageInfo.bIsProjectile == false)
	{
		// if this is not projectile, then it maybe countered
		if (damageActor && damageActor->OnCounterAttack(_damageInfo.hitDirection))
		{
			// counter attack
			vfxSelected = UVfxManager::GetVfx(EVfxType::metalImpact_sputtering);
			sfxSelected = UAudioManager::GetSFX(ESoundEffectType::EMeleeCounterAttackImpact);

			// play effects
			if (vfxSelected && sfxSelected)		
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), vfxSelected, _damageInfo.hitResult.Location);
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), sfxSelected, _damageInfo.hitResult.Location);
			}
			return false;
		}
		else
		{
			// counter attack fail, melee damage
			vfxSelected = UVfxManager::GetVfxBySurfaceType(surfaceType);

			sfxSelected = UAudioManager::GetMeleeWeaponImpactSFXByGearType(GetCurrentRightHandWeapon()->GetGearType());
		}
	}
	else
	{
		// this is projectile, and can not be countered
		EPhysicalSurface surfaceType = UPhysicalMaterial::DetermineSurfaceType(_damageInfo.hitResult.PhysMaterial.Get());

		sfxSelected = UAudioManager::GetProjectileImpactByMaterial(surfaceType);
		vfxSelected = UVfxManager::GetVfxBySurfaceType(surfaceType);
	}


	// play effects
	if (vfxSelected && sfxSelected)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), vfxSelected, _damageInfo.hitResult.Location);
		PlaySFXForImpact(sfxSelected, surfaceType, damageActor);
	}

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

	return true;
}

AArmor * UPawnStatsComponent::GetCurrentArmor() const
{
	return Armor;
}


