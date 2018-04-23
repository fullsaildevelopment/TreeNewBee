// Fill out your copyright notice in the Description page of Project Settings.

#include "HeavyShield_Ally.h"
#include "Combat/PawnStatsComponent.h"
#include "Combat/Gear.h"
#include "Animation/AIMelee_AnimInstance.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "AudioManager.h"
#include "VfxManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"



bool AHeavyShield_Ally::OnCounterAttack(const struct FDamageInfo* const _damageInfo,
	const class UPawnStatsComponent* const _damageCauserPawnStats)
{

	// 1. CharacterType Check 
	if (CharacterType == ECharacterType::Wolf_Shield_Mace)
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

	bool  damageCauserHoldingHeavyWeapon = _damageCauserPawnStats->IsUsingHeavyWeapon();

	if (damageCauserHoldingHeavyWeapon == false && CounterEndurance <= 0)
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

bool AHeavyShield_Ally::OnParry(const struct FDamageInfo* const _damageInfo,
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
	bool damageCauserHoldingHeavyWeapon = _damageCauserPawnStats->IsUsingHeavyWeapon();

	// 2. condition check based on the character type
	bool accept =
		(CharacterType == ECharacterType::Wolf_Shield_Mace) ?
		IsParrySuccess(damageCauserHoldingHeavyWeapon, currentState)
		: IsParrySuccess_Ulti(damageCauserHoldingHeavyWeapon, currentState);

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

		int parrySectionName = GetParrySectionNameIndex(damageCauserHoldingHeavyWeapon, _damageInfo);

		animRef->OnParry(GetParrySectionName(parrySectionName));

		UParticleSystem * sparkVFX = UVfxManager::GetVfx(EVfxType::metalImpact_sputtering);

		USoundCue* sparkSFX = (parrySectionName >= Block_Top_Down_Right) ?
			UAudioManager::GetSFX(ESoundEffectType::EShieldBashImpact) : UAudioManager::GetSFX(ESoundEffectType::EMeleeCounterAttackImpact);

		if (sparkSFX && sparkVFX)
		{
			UWorld* world = GetWorld();
			UGameplayStatics::SpawnEmitterAtLocation(world, sparkVFX, _damageInfo->hitResult.ImpactPoint);
			UGameplayStatics::PlaySoundAtLocation(world, sparkSFX, _damageInfo->hitResult.ImpactPoint);
		}

		OnParrySuccess(_damageCauserPawnStats);

		return true;
	}
	else
		return false;

}

int AHeavyShield_Ally::GetMeleeComboSel(bool _bIsMoving) const
{
	return (CharacterType == ECharacterType::Wolf_Shield_Mace) ?
		GetMeleeComboSel_Guardian(_bIsMoving) : GetMeleeComboSel_UltiGuardian(_bIsMoving);
}

void AHeavyShield_Ally::ClearEndurance()
{

	if (CharacterType == ECharacterType::Wolf_Shield_Mace)
		ParryEndurance = GetParryEndurance();
	else
		CounterEndurance = GetCounterEndurance();
}

int AHeavyShield_Ally::GetParrySectionNameIndex(bool _damageByHeavyWeapon, const FDamageInfo * const _damageInfo) const
{

	if (CharacterType == ECharacterType::Wolf_Shield_Mace)
		return (ParryEndurance > Easy_Parry_Threshold) ? GetParrySectionNameIndex_Easy(_damageInfo) : GetParrySectionNameIndex_Weak(_damageInfo);
	else
		return (_damageByHeavyWeapon) ? GetParrySectionNameIndex_Weak(_damageInfo) : GetParrySectionNameIndex_Easy(_damageInfo);

}

int AHeavyShield_Ally::GetParrySectionNameIndex_Easy(const FDamageInfo * const _damageInfo) const
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

int AHeavyShield_Ally::GetParrySectionNameIndex_Weak(const FDamageInfo * const _damageInfo) const
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
				return Parry_Top_Mid_Weak;
			else
			{
				float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
				if (dirDotRight > 0)
					return Block_Right_Top_Weak;
				else
					return Block_Left_Top_Weak;
			}

		}
		else
		{
			// Upper body hit
			float dirDotFor = FVector::DotProduct(hitDir, GetActorForwardVector());

			if (dirDotFor >= 0.86f)
				return (FMath::RandRange(-5, 5) > 0) ? Parry_Left_Mid_Up_Weak : Block_Right_Mid_Up_Weak;
			else
			{
				float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
				if (dirDotRight > 0)
					return Block_Right_Mid_Up_Weak;
				else
					return Parry_Left_Mid_Up_Weak;
			}

		}
	}
	else
	{
		float dirDotFor = FVector::DotProduct(hitDir, GetActorForwardVector());

		if (dirDotFor >= 0.86f)
			return Parry_Left_Mid_Down_Weak;
		else
		{
			float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
			if (dirDotRight > 0)
				return Parry_Right_Down_Weak;
			else
				return Parry_Left_Mid_Down_Weak;
		}
	}
}


FName AHeavyShield_Ally::GetCounterAttackSectionName(const FDamageInfo * const _damageInfo) const
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

void AHeavyShield_Ally::UpdateEnduranceOnBeingHit(const AActor* const _damageCauser)
{

	if (CharacterType == ECharacterType::Wolf_Shield_Mace || IsStuned())
		return;

	const ATheLastBastionCharacter* damageCauser = Cast<ATheLastBastionCharacter>(_damageCauser);
	if (damageCauser)
	{

		bool damageCauserHoldingHeavyWeapon
			= damageCauser->IsUsingHeavyWeapon();

		// if hit by heavy weapon, endurance will not grow
		if (damageCauserHoldingHeavyWeapon)
			return;

		ParryEndurance--;
	}
	else
		UE_LOG(LogTemp, Error, TEXT("damageCauser == nullptr - AHeavySheild_Enemy::UpdateEnduranceOnBeingHit"));
}

int AHeavyShield_Ally::GetMeleeComboSel_Guardian(bool _bIsMoving) const
{
	if (_bIsMoving)
		return FMath::RandRange(Sns_Move_Min, Sns_Move_Max);
	else
		return FMath::RandRange(Sns_InPlace_Min, Sns_InPlace_Max);

}

int AHeavyShield_Ally::GetMeleeComboSel_UltiGuardian(bool _bIsMoving) const
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

void AHeavyShield_Ally::OnParrySuccess(const class UPawnStatsComponent* const _damageCauserPawnStats)
{

	if (CharacterType == ECharacterType::Wolf_Shield_Mace)
	{
		EGearType damageCauserGear = _damageCauserPawnStats->GetCurrentRightHandWeapon()->GetGearType();
		if (damageCauserGear == EGearType::LongSword || damageCauserGear == EGearType::DoubleHandWeapon)
			ParryEndurance--;
		else
			ParryEndurance -= 2;
	}
	else
	{
		// reset the endurance for next parry
		ParryEndurance = GetParryEndurance();
		CounterEndurance--;

	}
}

bool AHeavyShield_Ally::IsParrySuccess(bool _damageByHeavyWeapon, EAIActionState _currentActionState) const
{
	bool isRightState =
		_currentActionState == EAIActionState::None ||
		_currentActionState == EAIActionState::Defend ||
		_currentActionState == EAIActionState::MeleePreAttack ||
		_currentActionState == EAIActionState::MeleePostAttack ||
		_currentActionState == EAIActionState::MeleeAttack;

	if (!_damageByHeavyWeapon && isRightState && ParryEndurance > 0)
		return true;
	else
		return false;
}

bool AHeavyShield_Ally::IsParrySuccess_Ulti(bool _damageByHeavyWeapon, EAIActionState _currentActionState) const
{
	// parry happen whenever the right animatino state or parry endurance less equal to zero

	// it can parry during attack if the attack did not damage him with heavy weapon
	bool isRightState =
		_currentActionState == EAIActionState::None ||
		_currentActionState == EAIActionState::Defend ||
		_currentActionState == EAIActionState::MeleePreAttack ||
		_currentActionState == EAIActionState::MeleePostAttack ||
		(!_damageByHeavyWeapon && _currentActionState == EAIActionState::MeleeAttack);

	bool accept = ParryEndurance <= 0 || isRightState;

	return accept;
}
