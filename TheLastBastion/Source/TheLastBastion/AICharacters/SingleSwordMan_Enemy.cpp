// Fill out your copyright notice in the Description page of Project Settings.

#include "SingleSwordMan_Enemy.h"
#include "Animation/AIMelee_AnimInstance.h"
#include "Combat/PawnStatsComponent.h"
#include "Combat/Gear.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "AudioManager.h"
#include "VfxManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


bool ASingleSwordMan_Enemy::OnParry(const struct FDamageInfo* const _damageInfo, 
	const class UPawnStatsComponent* const _damageCauserPawnStats)
{
	//return false;

	//1. Check correct character type and endurance
	bool accept =
		CharacterType == ECharacterType::LanTrooper_Power &&
		Endurance <= 0;
	
	if (!accept)
		return false;

	//2. Check correct weapon type
	AGear* damageCauserGear = _damageCauserPawnStats->GetCurrentRightHandWeapon();
	EGearType gearType;

	if (damageCauserGear)
		gearType = damageCauserGear->GetGearType();
	else
	{
		UE_LOG(LogTemp, Error, TEXT("damageCauserGear == nullptr,ASingleSwordMan_Enemy::OnParry "));
		return false;
	}

	bool weaponTypeThatAbleToParry = gearType == EGearType::LongSword || gearType == EGearType::DoubleHandWeapon;

	if (weaponTypeThatAbleToParry == false)
		return false;


	// 3. Action check, is this ai 's current action is fit for parrying ?
	UAIMelee_AnimInstance* animRef = Cast<UAIMelee_AnimInstance>(GetAnimInstanceRef());
	if (animRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("animRef == nullptr, AHeavyMelee::OnParry"));
		return false;
	}

	EAIActionState currentState = animRef->GetCurrentActionState();
	bool isRightState = currentState == EAIActionState::None ||
		currentState == EAIActionState::Defend ||
		currentState == EAIActionState::MeleePreAttack ||
		currentState == EAIActionState::MeleePostAttack;

	if (isRightState)
	{
		// 4. direction check, less than 45 from forward vector
		float forwardDot = FVector::DotProduct(GetActorForwardVector(), _damageInfo->hitDirection);

		//UE_LOG(LogTemp, Warning, TEXT("forwardDot: %f - AHeavyMelee::OnParry"), forwardDot);

		if (forwardDot >= 0.707f)
		{
			ATheLastBastionBaseAIController* baseAICtrl
				= Cast<ATheLastBastionBaseAIController>(GetController());
			if (baseAICtrl == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("baseAICtrl is nullptr - ASingleSwordMan_Enemy::OnParry"));
				return false;
			}
			baseAICtrl->SetIsPaused_BBC(true);
			accept = true;

			animRef->OnParry(GetParrySectionName(_damageInfo), AM_SH_Parry);
			// fx

			UParticleSystem * sparkVFX = UVfxManager::GetVfx(EVfxType::metalImpact_sputtering);
			USoundCue* sparkSFX = UAudioManager::GetSFX(ESoundEffectType::EMeleeCounterAttackImpact);

			if (sparkSFX && sparkVFX)
			{
				UWorld* world = GetWorld();
				UGameplayStatics::SpawnEmitterAtLocation(world, sparkVFX, _damageInfo->hitResult.ImpactPoint);
				UGameplayStatics::PlaySoundAtLocation(world, sparkSFX, _damageInfo->hitResult.ImpactPoint);
			}

			// reset the endurance for next parry
			Endurance = GetParryEndurance();

			return true;
		}
	}
	else
	{
		return accept;
	}

	return false;
}

int ASingleSwordMan_Enemy::GetMeleeComboSel(bool _bIsMoving) const
{

	switch (CharacterType)
	{
	case ECharacterType::LanTrooper_Rookie:
		return GetMeleeComboSel_Rookie(_bIsMoving);
	case ECharacterType::LanTrooper_Fast:
		return GetMeleeComboSel_Fast(_bIsMoving);
	case ECharacterType::LanTrooper_Power:
		return GetMeleeComboSel_Power(_bIsMoving);
	case ECharacterType::LanTrooper_Shield:
		return GetMeleeComboSel_Shield(_bIsMoving);
	case ECharacterType::LanTrooper_HeavyShield:
		return GetMeleeComboSel_HeavyShield(_bIsMoving);
	default:
		return Super::GetMeleeComboSel(_bIsMoving);
	}
}

FName ASingleSwordMan_Enemy::GetParrySectionName(const struct FDamageInfo* const _damageInfo) const
{

	// figure out the anim section to play
	FName animSection;

	FVector impactPoint = _damageInfo->hitResult.ImpactPoint;

	FVector hitDir = (impactPoint - GetActorLocation());
	hitDir.Z = 0.0f;
	hitDir = hitDir.GetUnsafeNormal();

	FVector pelvisLocation = GetMesh()->GetBoneLocation(TEXT("pelvis"));
	float dirDotFor = FVector::DotProduct(hitDir, GetActorForwardVector());

	if (impactPoint.Z >= pelvisLocation.Z)
	{
		FVector chestLocation = GetMesh()->GetBoneLocation(TEXT("spine_03"));

		if (impactPoint.Z > chestLocation.Z)
		{
			// top hit
			if (dirDotFor >= 0.86f)
				animSection = TEXT("Parry_Top_Mid");
			else
			{
				float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
				if (dirDotRight > 0)
					animSection = TEXT("Parry_Top_Right");
				else
					animSection = TEXT("Parry_Top_Left");
			}

		}
		else
		{
			// Upper body hit
			if (dirDotFor >= 0.86f)
				animSection = TEXT("Parry_Upper_Mid");
			else
			{
				float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
				if (dirDotRight > 0)
					animSection = TEXT("Parry_Upper_Right");
				else
					animSection = TEXT("Parry_Upper_Left");
			}

		}
	}
	else
	{
		// hit on lower body
		if (dirDotFor >= 0.86f)
			animSection = TEXT("Parry_Down_Mid");
		else
		{
			float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
			if (dirDotRight > 0)
				animSection = TEXT("Parry_Down_Right");
			else
				animSection = TEXT("Parry_Down_Left");
		}
	}



	return animSection;
}

int ASingleSwordMan_Enemy::GetMeleeComboSel_Rookie(bool _bIsMoving) const
{

	if (_bIsMoving)
		return bAttackFromRight ? FMath::RandRange(SH_Roo_Move_Right_Min, SH_Roo_Move_Right_Max) :
		FMath::RandRange(SH_Roo_Move_Left_Min, SH_Roo_Move_Left_Max);
	else
		return bAttackFromRight ? FMath::RandRange(SH_Roo_InPlace_Right_Min, SH_Roo_InPlace_Right_Max) :
		FMath::RandRange(SH_Roo_InPlace_Left_Min, SH_Roo_InPlace_Left_Max);
}

int ASingleSwordMan_Enemy::GetMeleeComboSel_Fast(bool _bIsMoving) const
{
	if (_bIsMoving)
		return FMath::RandRange(SH_Fast_Move_Min, SH_Fast_Move_Max);
	else
		return bAttackFromRight ? FMath::RandRange(SH_Fast_InPlace_Right_Min, SH_Fast_InPlace_Right_Max) :
		FMath::RandRange(SH_Fast_InPlace_Left_Min, SH_Fast_InPlace_Left_Max);
}

int ASingleSwordMan_Enemy::GetMeleeComboSel_Power(bool _bIsMoving) const
{
	if (_bIsMoving)
		return bAttackFromRight ? FMath::RandRange(SH_Pow_Move_Right_Min, SH_Pow_Move_Right_Max) :
		FMath::RandRange(SH_Pow_Move_Left_Min, SH_Pow_Move_Left_Max);
	else
		return bAttackFromRight ? FMath::RandRange(SH_Pow_InPlace_Right_Min, SH_Pow_InPlace_Right_Max) :
		FMath::RandRange(SH_Pow_InPlace_Left_Min, SH_Pow_InPlace_Left_Max);
}

int ASingleSwordMan_Enemy::GetMeleeComboSel_Shield(bool _bIsMoving) const
{
	return 0;
}

int ASingleSwordMan_Enemy::GetMeleeComboSel_HeavyShield(bool _bIsMoving) const
{
	return 0;
}




