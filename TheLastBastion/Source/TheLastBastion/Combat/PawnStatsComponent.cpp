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

#include "Animation/Base_AnimInstance.h"

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


#define QueenGuard_BaseHp 2000
#define QueenGuard_HpOnLevelUp 250
#define QueenGuard_DamageOnLevelUp 100

static TSubclassOf<class UUserWidget> FloatingText_WBP;

// Sets default values for this component's properties
UPawnStatsComponent::UPawnStatsComponent()
{	
	PrimaryComponentTick.bCanEverTick = false;
	bGenerateRawStatsAtBeginPlay = true;
	bArmedFromBeginPlay = false;
	CurrentWeapon_Index = 0;
	Level = 1;

	StaminaRaw = 120.0f;
	DpCurrent = 0;

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

	USkeletalMeshComponent* mesh = mCharacter->GetMesh();
	if (mCharacter && mesh)
	{
		UBase_AnimInstance* animRef = Cast<UBase_AnimInstance>(mesh->GetAnimInstance());
		if (animRef == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("animRef == nullptr - UPawnStatsComponent::BeginPlay"));
		}
		else
		{
			if (GetCurrentRightHandWeapon())
				animRef->UpdateAnimationSetOnWeaponChange(GetCurrentRightHandWeapon()->GetGearType());
		}
	}

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

void UPawnStatsComponent::GenerateRawStatsByLevel(int _level, float& _baseDamage, float& _hpRaw) 
{
	Level = _level;

	if (mCharacter)
	{
		CalculateRawStatsByType(Level, mCharacter->GetCharacterType(),_baseDamage, _hpRaw);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("mCharacter is Null - UPawnStatsComponent::GenerateRawStatsByLevel"));
}

void UPawnStatsComponent::CalculateRawStatsByType(int _level, ECharacterType _type, float & _damage, float & _hp)
{
	switch (_type)
	{
	default:
	{
		_hp = 500.0f + _level * 30.0f;
		_damage = 25.0f * _level;
	}
	break;
	case ECharacterType::Lan_QueenGuard:
	{
		_hp = QueenGuard_BaseHp + _level * QueenGuard_HpOnLevelUp;
		_damage = QueenGuard_DamageOnLevelUp * _level;
		DpCurrent = _hp;
	}
	break;
	case ECharacterType::Ranger:
	{
		_hp = RangerInitHp + _level * 15;
		_damage = 15.0f * _level;
	}
	break;
	}
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


	HpMax = (1.0f + factorHp * 0.01f) * HpRaw + HpAdd;
	StaminaMax = (1.0f + factorStamina * 0.01f) * StaminaRaw + SpAdd;
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
	// Update the row value, so the gear merits can add on
	GenerateRawStatsByLevel(Level, BaseDamage, HpRaw);
	// apply the gear merits
	GenerateMaxStats(true);
}

void UPawnStatsComponent::Born()
{
	Level = 1;
	GenerateRawStatsByLevel(Level, BaseDamage, HpRaw);
	HpCurrent = HpMax;
	StaminaCurrent = StaminaMax;
}

void UPawnStatsComponent::GenerateStatsAtBeginPlay()
{
	if (bGenerateRawStatsAtBeginPlay)
	{
		GenerateRawStatsByLevel(Level, BaseDamage, HpRaw);
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
		AudioComp->SetIntParameter(TEXT("SurfaceType"), 0);
		AudioComp->SetIntParameter(TEXT("DamagedActorType"), (int32)_damagedCharacter->GetCharacterVoiceType());
		AudioComp->AttenuationSettings = _sfx->AttenuationSettings;
		AudioComp->Play();
		break;
	}

	case SURFACE_METAL:
	{   
		UAudioComponent* AudioComp = _damagedCharacter->GetAudioComp();
		AudioComp->SetSound(_sfx);
		AudioComp->SetIntParameter(TEXT("SurfaceType"), 1);
		AudioComp->SetIntParameter(TEXT("DamagedActorType"), (int32)_damagedCharacter->GetCharacterVoiceType());
		AudioComp->AttenuationSettings = _sfx->AttenuationSettings;
		AudioComp->Play();
		break;
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

	if (damageActor == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("damageActor == nullptr, UPawnStatsComponent::ApplyDamage"));
		return false;;
	}
	// Check if this damage is caused by melee and been countered
	if (_damageInfo.bIsProjectile == false)
	{
		// if this is not projectile, then it maybe countered
		if (damageActor->OnCounterAttack(_damageInfo.hitDirection))
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
		else if (damageActor->OnParry(&_damageInfo, this))
		{
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


