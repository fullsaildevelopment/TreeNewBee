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



bool AHeavySheild_Enemy::OnCounterAttack(const struct FDamageInfo* const _damageInfo,
	const class UPawnStatsComponent* const _damageCauserPawnStats)
{

	// 1. CharacterType Check 
	if (CharacterType == ECharacterType::LanTrooper_Shield)
		return false;

	UAIMelee_AnimInstance* animRef = Cast<UAIMelee_AnimInstance>(GetAnimInstanceRef());
	if (animRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("animRef == nullptr, AHeavyMelee::OnParry"));
		return false;
	}

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

	bool weaponCanNotCounter = gearType == EGearType::GreatSword 
		|| gearType == EGearType::Hammer 
		|| gearType == EGearType::BattleAxe;

	if (weaponCanNotCounter == false && CounterEndurance <= 0)
	{
		// 3. direction check, less than 45 from forward vector
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

			animRef->OnCounterAttack(GetCounterAttackSectionName(_damageInfo));

			UParticleSystem * sparkVFX = UVfxManager::GetVfx(EVfxType::metalImpact_sputtering);

			USoundCue* sparkSFX = UAudioManager::GetSFX(ESoundEffectType::EMeleeCounterAttackImpact);

			if (sparkSFX && sparkVFX)
			{
				UWorld* world = GetWorld();
				UGameplayStatics::SpawnEmitterAtLocation(world, sparkVFX, _damageInfo->hitResult.ImpactPoint);
				UGameplayStatics::PlaySoundAtLocation(world, sparkSFX, _damageInfo->hitResult.ImpactPoint);
			}

			CounterEndurance = GetCounterEndurance();
			return true;
		}
		else
			return false;
	}
	else
		return false;


}

bool AHeavySheild_Enemy::OnParry(const struct FDamageInfo* const _damageInfo,
	const class UPawnStatsComponent* const _damageCauserPawnStats)
{

	// 1. get the current animation state
	UAIMelee_AnimInstance* animRef = Cast<UAIMelee_AnimInstance>(GetAnimInstanceRef());
	if (animRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("animRef == nullptr, AHeavyMelee::OnParry"));
		return false;
	}

	EAIActionState currentState = animRef->GetCurrentActionState();

	// 2. condition check based on the character type
	bool accept = 
		(CharacterType == ECharacterType::LanTrooper_Shield) ? 
		IsParrySuccess(_damageCauserPawnStats, currentState) : IsParrySuccess_Ulti(_damageCauserPawnStats, currentState);

	if (accept == false)
		return false;

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


		int parrySectionName = GetParrySectionNameIndex(_damageInfo);
		
		animRef->OnParry(GetParrySectionName(parrySectionName));

		UParticleSystem * sparkVFX = UVfxManager::GetVfx(EVfxType::metalImpact_sputtering);

		USoundCue* sparkSFX = (parrySectionName >= Block_Top_Down_Right)? 
			UAudioManager::GetSFX(ESoundEffectType::EShieldBashImpact) : UAudioManager::GetSFX(ESoundEffectType::EMeleeCounterAttackImpact);

		if (sparkSFX && sparkVFX)
		{
			UWorld* world = GetWorld();
			UGameplayStatics::SpawnEmitterAtLocation(world, sparkVFX, _damageInfo->hitResult.ImpactPoint);
			UGameplayStatics::PlaySoundAtLocation(world, sparkSFX, _damageInfo->hitResult.ImpactPoint);
		}

		// reset the endurance for next parry
		ParryEndurance = GetParryEndurance();
		CounterEndurance--;

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

void AHeavySheild_Enemy::ClearEndurance()
{
	CounterEndurance = GetCounterEndurance();
}

int AHeavySheild_Enemy::GetParrySectionNameIndex(const FDamageInfo * const _damageInfo) const
{

	// figure out the anim section to play
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
				return Block_Top_Up_Mid;
			else
			{
				float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
				if (dirDotRight > 0)
					return Block_Top_Down_Right;
				else
					return Parry_Top_Left;
			}

		}
		else
		{
			// Upper body hit
			float dirDotFor = FVector::DotProduct(hitDir, GetActorForwardVector());

			if (dirDotFor >= 0.86f)
				return Block_Top_Up_Mid;
			else
			{
				float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
				if (dirDotRight > 0)
					return Parry_Up_Right;
				else
					return Parry_Up_Left;
			}

		}
	}
	else
	{
		float dirDotFor = FVector::DotProduct(hitDir, GetActorForwardVector());

		if (dirDotFor >= 0.86f)
			return Block_Down;
		else
		{
			float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
			if (dirDotRight > 0)
				return (FMath::RandRange(-5, 5) > 0) ? Block_Down : Block_Top_Down_Right;
			else
				return Block_Down;
		}
	}
}

FName AHeavySheild_Enemy::GetCounterAttackSectionName(const FDamageInfo * const _damageInfo) const
{
	// figure out the anim section to play
	FVector impactPoint = _damageInfo->hitResult.ImpactPoint;

	FVector hitDir = (impactPoint - GetActorLocation());
	hitDir.Z = 0.0f;
	hitDir = hitDir.GetUnsafeNormal();

	float dirDotFor = FVector::DotProduct(hitDir, GetActorForwardVector());
	
	if (dirDotFor >= 0.86f)
		return TEXT("Sns_Front");
	else
	{
		float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
		if (dirDotRight > 0)
			return TEXT("Sns_Right");
		else
			return TEXT("Sns_Left");
	}

}

void AHeavySheild_Enemy::UpdateEnduranceOnBeingHit(const AActor* const _damageCauser)
{
	const ATheLastBastionCharacter* damageCauser = Cast<ATheLastBastionCharacter>(_damageCauser);
	if (damageCauser)
	{
		EGearType damageCauserGear = damageCauser->GetPawnStatsComp_Const()->GetCurrentRightHandWeapon()->GetGearType();

		// if hit by heavy weapon, endurance will not grow
		if (damageCauserGear == EGearType::GreatSword 
			|| damageCauserGear == EGearType::BattleAxe 
			|| damageCauserGear == EGearType::Hammer)
			return;

		ParryEndurance--;
	}
	else
		UE_LOG(LogTemp, Error, TEXT("damageCauser == nullptr - AHeavySheild_Enemy::UpdateEnduranceOnBeingHit"));
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
		return FMath::RandRange(Sns_Ulti_Move_Min, Sns_Ulti_Move_Max);
	else
		return bAttackFromRight ? FMath::RandRange(Sns_Ulti_InPlace_Right_Min, Sns_Ulti_InPlace_Right_Max) :
		FMath::RandRange(Sns_Ulti_InPlace_Left_Min, Sns_Ulti_InPlace_Left_Max);
}

bool AHeavySheild_Enemy::IsParrySuccess(const class UPawnStatsComponent* const _damageCauserPawnStats, EAIActionState _currentActionState) const
{
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

	return true;
}

bool AHeavySheild_Enemy::IsParrySuccess_Ulti(const class UPawnStatsComponent* const _damageCauserPawnStats, EAIActionState _currentActionState) const
{
	// parry happen whenever the right animatino state or parry endurance less equal to zero

	EGearType gearType = _damageCauserPawnStats->GetCurrentRightHandWeapon()->GetGearType();
	bool damageCauserHoldingHeavyWeapon
		= gearType == EGearType::GreatSword || gearType == EGearType::Hammer || gearType == EGearType::BattleAxe;

	// it can parry during attack if the attack did not damage him with heavy weapon
	bool isRightState =
		_currentActionState == EAIActionState::None ||
		_currentActionState == EAIActionState::Defend ||
		_currentActionState == EAIActionState::MeleePreAttack ||
		_currentActionState == EAIActionState::MeleePostAttack || 
		(!damageCauserHoldingHeavyWeapon && _currentActionState == EAIActionState::MeleeAttack);

	bool accept = ParryEndurance <= 0 || isRightState;

	return accept;
}
