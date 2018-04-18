// Fill out your copyright notice in the Description page of Project Settings.

#include "HeavySheild_Enemy.h"
#include "Combat/PawnStatsComponent.h"
#include "Combat/Gear.h"
#include "Animation/AIMelee_AnimInstance.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "AudioManager.h"
#include "VfxManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"



bool AHeavySheild_Enemy::OnParry(const struct FDamageInfo* const _damageInfo,
	const class UPawnStatsComponent* const _damageCauserPawnStats)
{

	bool accept = 
		(CharacterType == ECharacterType::LanTrooper_Shield) ? 
		IsParrySuccess(_damageCauserPawnStats) : IsParrySuccess_Ulti(_damageCauserPawnStats);


	// 2. Action check, is this ai 's current action is fit for parrying ?
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

	//if (!isRightState)
	//	return false;

	// 4. direction check, less than 45 from forward vector
	float forwardDot = FVector::DotProduct(GetActorForwardVector(), _damageInfo->hitDirection);

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

		animRef->OnParry(GetParrySectionName(_damageInfo));// , AM_SH_Parry);
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
		ParryEndurance = GetParryEndurance();

		return true;
	}
	else
		return false;

}


int AHeavySheild_Enemy::GetMeleeComboSel(bool _bIsMoving) const
{
	return (CharacterType == ECharacterType::LanTrooper_Shield)? 
		GetMeleeComboSel_Guardian(_bIsMoving) : GetMeleeComboSel_UltiGuardian(_bIsMoving);
}

FName AHeavySheild_Enemy::GetParrySectionName(const FDamageInfo * const _damageInfo) const
{

	// figure out the anim section to play
	FName animSection;

	FVector impactPoint = _damageInfo->hitResult.ImpactPoint;

	FVector hitDir = (impactPoint - GetActorLocation());
	hitDir.Z = 0.0f;
	hitDir = hitDir.GetUnsafeNormal();

	FVector pelvisLocation = GetMesh()->GetBoneLocation(TEXT("pelvis"));

	if (impactPoint.Z >= pelvisLocation.Z)
	{
		FVector chestLocation = GetMesh()->GetBoneLocation(TEXT("spine_03"));

		if (impactPoint.Z > chestLocation.Z)
		{
			// top hit
			float dirDotFor = FVector::DotProduct(hitDir, GetActorForwardVector());

			if (dirDotFor >= 0.86f)
				animSection = TEXT("Block_Top_Up_Mid");
			else
			{
				float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
				if (dirDotRight > 0)
					animSection = TEXT("Block_Top_Down_Right");
				else
					animSection = TEXT("Parry_Top_Left");
			}

		}
		else
		{
			// Upper body hit
			float dirDotFor = FVector::DotProduct(hitDir, GetActorForwardVector());

			if (dirDotFor >= 0.86f)
				animSection = TEXT("Block_Top_Up_Mid");
			else
			{
				float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
				if (dirDotRight > 0)
					animSection = TEXT("Parry_Up_Right");
				else
					animSection = TEXT("Parry_Up_Left");
			}

		}
	}
	else
	{
		float dirDotFor = FVector::DotProduct(hitDir, GetActorForwardVector());

		if (dirDotFor >= 0.86f)
			animSection = TEXT("Block_Down");
		else
		{
			float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
			if (dirDotRight > 0)
				animSection = (FMath::RandRange(-5, 5)> 0) ? TEXT("Block_Down"): TEXT("Block_Top_Down_Right");
			else
				animSection = TEXT("Block_Down");
		}
	}



	return animSection;
}

FName AHeavySheild_Enemy::GetCounterAttackSectionName(const FDamageInfo * const _damageInfo) const
{
	return FName();
}

int AHeavySheild_Enemy::GetMeleeComboSel_Guardian(bool _bIsMoving) const
{
	//if (_bIsMoving)
	//	return bAttackFromRight ? FMath::RandRange(SH_Roo_Move_Right_Min, SH_Roo_Move_Right_Max) :
	//	FMath::RandRange(SH_Roo_Move_Left_Min, SH_Roo_Move_Left_Max);
	//else
	//	return bAttackFromRight ? FMath::RandRange(SH_Roo_InPlace_Right_Min, SH_Roo_InPlace_Right_Max) :
	//	FMath::RandRange(SH_Roo_InPlace_Left_Min, SH_Roo_InPlace_Left_Max);
	return 0;
}

int AHeavySheild_Enemy::GetMeleeComboSel_UltiGuardian(bool _bIsMoving) const
{
	bool shouldDoShieldBash = FMath::RandRange(0, 10) < 3;

	if (shouldDoShieldBash)
		return Sns_Ulti_ShieldBash;

	if (_bIsMoving)
		return bAttackFromRight ? FMath::RandRange(Sns_Ulti_Move_Right_Min, Sns_Ulti_Move_Right_Max) :
		FMath::RandRange(Sns_Ulti_Move_Left_Min, Sns_Ulti_Move_Left_Max);
	else
		return bAttackFromRight ? FMath::RandRange(Sns_Ulti_InPlace_Right_Min, Sns_Ulti_InPlace_Right_Max) :
		FMath::RandRange(Sns_Ulti_InPlace_Left_Min, Sns_Ulti_InPlace_Left_Max);
}

bool AHeavySheild_Enemy::IsParrySuccess(const class UPawnStatsComponent* const _damageCauserPawnStats) const
{
	return true;
}

bool AHeavySheild_Enemy::IsParrySuccess_Ulti(const class UPawnStatsComponent* const _damageCauserPawnStats) const
{
	return true;

	if (CharacterType != ECharacterType::LanTrooper_Power)
		return false;


	bool accept = ParryEndurance <= 0;
	if (!accept)
		return false;


	//3. Check correct weapon type
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

}
