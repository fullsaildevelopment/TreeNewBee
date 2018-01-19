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

#include "Net/UnrealNetwork.h"

const float RangerInitHp = 230.0f;
const float BuilderInitHp = 180.0f;
static const int MaxWeaponSlot = 2;

static TSubclassOf<class UUserWidget> FloatingText_WBP;

// Sets default values for this component's properties
UPawnStatsComponent::UPawnStatsComponent()
{	
	PrimaryComponentTick.bCanEverTick = false;
	bGenerateRawStatsAtBeginPlay = true;
	bArmedFromBeginPlay = false;
	CurrentWeapon_Index = 0;

	if (!FloatingText_WBP)
		UCustomType::FindClass<UUserWidget>(FloatingText_WBP, TEXT("/Game/UI/In-Game/WBP_FloatingNumber"));
	
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

	mCharacter->OnTakeAnyDamage.AddDynamic(this, &UPawnStatsComponent::OnTakeAnyDamageHandle);
	mCharacter->OnTakePointDamage.AddDynamic(this, &UPawnStatsComponent::OnTakePointDamageHandle);

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
	//RightHandWeapon->Equip(mCharacter->GetMesh());
	WeaponSlots[CurrentWeapon_Index].RightHand->Equip(mCharacter->GetMesh());
	if (WeaponSlots[CurrentWeapon_Index].RightHand)
		WeaponSlots[CurrentWeapon_Index].RightHand->Equip(mCharacter->GetMesh());

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
		factorStamina = 1;

	AGear* LeftHandWeapon = WeaponSlots[CurrentWeapon_Index].LeftHand;
	AGear* RightHandWeapon = WeaponSlots[CurrentWeapon_Index].RightHand;

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

float UPawnStatsComponent::CalculateDamage()
{
	return 	FMath::RandRange(10, 100);
}

float UPawnStatsComponent::GetBaseDamage()
{
	return 5.0f;
}
#pragma endregion



///*** OnAuthority
/** Generate Raw Stats, equip geat, and Add Gear buff on raw stats*/
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

		// use the weapon at the first slot at the beginning
		CurrentWeapon_Index = 0;
		if (WeaponWheels.Num() > 0)
		{
			TSubclassOf<AGear> RightHandWeapon_Class = WeaponWheels[CurrentWeapon_Index];
			if (RightHandWeapon_Class)
			{
				WeaponSlots[CurrentWeapon_Index].RightHand = world->SpawnActor<AGear>(RightHandWeapon_Class, spawnParam);
				TSubclassOf<AGear> LeftHandWeapon_Class = WeaponSlots[CurrentWeapon_Index].RightHand->GetLeftHandGear();
				if (LeftHandWeapon_Class)
					WeaponSlots[CurrentWeapon_Index].LeftHand = world->SpawnActor<AGear>(LeftHandWeapon_Class, spawnParam);

				if (bArmedFromBeginPlay)
				{
					WeaponSlots[CurrentWeapon_Index].RightHand->Arm(mCharacter->GetMesh());
					if (WeaponSlots[CurrentWeapon_Index].LeftHand)
						WeaponSlots[CurrentWeapon_Index].LeftHand->Arm(mCharacter->GetMesh());
				}
				else
				{
					WeaponSlots[CurrentWeapon_Index].RightHand->Equip(mCharacter->GetMesh());
					if (WeaponSlots[CurrentWeapon_Index].LeftHand)
						WeaponSlots[CurrentWeapon_Index].LeftHand->Equip(mCharacter->GetMesh());
				}
			}
			
			int NextWeaponIndex = CurrentWeapon_Index + 1;
			// Equip our SecondHand Weapon
			if (WeaponWheels.Num() > 1)
			{
				TSubclassOf<AGear> SecRightHandWeapon_Class = WeaponWheels[NextWeaponIndex];
				if (SecRightHandWeapon_Class)
				{
					WeaponSlots[NextWeaponIndex].RightHand = world->SpawnActor<AGear>(SecRightHandWeapon_Class, spawnParam);
					TSubclassOf<AGear> SecLeftHandWeapon_Class = WeaponSlots[NextWeaponIndex].RightHand->GetLeftHandGear();
					if (SecLeftHandWeapon_Class)
						WeaponSlots[NextWeaponIndex].LeftHand = world->SpawnActor<AGear>(SecLeftHandWeapon_Class, spawnParam);

						WeaponSlots[NextWeaponIndex].RightHand->Equip(mCharacter->GetMesh());
						if (WeaponSlots[NextWeaponIndex].LeftHand)
							WeaponSlots[NextWeaponIndex].LeftHand->Equip(mCharacter->GetMesh());
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s does not equip with anyweapon - UPawnStatsComponent::GenerateStatsAtBeginPlay"), *mCharacter->GetName())
		}

		if (Armor_ClassBp)
		{
			Armor = world->SpawnActor<AArmor>(Armor_ClassBp, spawnParam);
			Armor->Equip(mCharacter->GetMesh());
		}
		GenerateMaxStats();
	}
}


UInGameFloatingText* UPawnStatsComponent::GenerateFloatingText(const FVector& _worldPos)
{
	FVector2D screenPos;
	UGameplayStatics::ProjectWorldToScreen(
		UGameplayStatics::GetPlayerController(this, 0),
		_worldPos, screenPos);

	UInGameFloatingText* out = nullptr;
	UWorld* world = GetWorld();

	if (world)
		out = Cast<UInGameFloatingText>(CreateWidget<UUserWidget>(world, FloatingText_WBP));

	if (out == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Create Floating Text Failed"));
		return out;
	}

	out->SetRenderTranslation(screenPos);
	return out;
}

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

///*** OnAuthority
void UPawnStatsComponent::OnTakeAnyDamageHandle(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{

}

///*** OnAuthority
void UPawnStatsComponent::OnTakePointDamageHandle(AActor * DamagedActor, float Damage, AController * InstigatedBy, FVector HitLocation, UPrimitiveComponent * FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType * DamageType, AActor * DamageCauser)
{

	if (mCharacter->GetIsDead())
	{
		return;
	}

	float totalDamage = Damage + CalculateDamage();

	if (!mCharacter->GetIsGodMode())
	{
		HpCurrent = HpCurrent - totalDamage;
	}
	HpCurrent = FMath::Clamp(HpCurrent, 0.0f, HpMax);
	// Let Character class to handle Updating the HUD display
	OnHealthChanged.Broadcast(this, totalDamage, DamageType, BoneName, ShotFromDirection, HitLocation);
	

	// Create Floating damage point
	UInGameFloatingText* floatingDamage = GenerateFloatingText(HitLocation);
	if (floatingDamage)
	{
		floatingDamage->SetInGameFTProperty(FText::AsNumber((int)totalDamage));
		floatingDamage->AddToViewport();
	}
}


int UPawnStatsComponent::GetMaxWeaponSlot()
{
	return MaxWeaponSlot;
}


