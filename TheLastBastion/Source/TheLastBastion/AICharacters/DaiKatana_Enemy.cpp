// Fill out your copyright notice in the Description page of Project Settings.

#include "DaiKatana_Enemy.h"
#include "Combat/PawnStatsComponent.h"
#include "Combat/Gear.h"
#include "Animation/AIMelee_AnimInstance.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "AudioManager.h"
#include "VfxManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

bool ADaiKatana_Enemy::OnCounterAttack(const FDamageInfo * const _damageInfo, const UPawnStatsComponent * const _damageCauserPawnStats)
{
	// 1. get the current animation state
	UAIMelee_AnimInstance* animRef = Cast<UAIMelee_AnimInstance>(GetAnimInstanceRef());
	if (animRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("animRef == nullptr, ADaiKatana_Enemy::OnParry"));
		return false;
	}

	if (CounterEndurance <= 0)
	{
		// 3. direction check, less than 45 from forward vector
		float forwardDot = FVector::DotProduct(GetActorForwardVector(), _damageInfo->hitDirection);

		if (forwardDot >= 0.707f)
		{
			ATheLastBastionBaseAIController* baseAICtrl
				= Cast<ATheLastBastionBaseAIController>(GetController());
			if (baseAICtrl == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("baseAICtrl is nullptr - ADaiKatana_Enemy::OnParry"));
				return false;
			}
			baseAICtrl->SetIsPaused_BBC(true);

			animRef->OnCounterAttack(FName("Katana"), 0.9f);

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


	return false;
}

bool ADaiKatana_Enemy::OnParry(const struct FDamageInfo* const _damageInfo,
	const class UPawnStatsComponent* const _damageCauserPawnStats)
{

	// 1. get the current animation state
	UAIMelee_AnimInstance* animRef = Cast<UAIMelee_AnimInstance>(GetAnimInstanceRef());
	if (animRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("animRef == nullptr, ADaiKatana_Enemy::OnParry"));
		return false;
	}

	// 2. cant parry during counter attack
	if (animRef->IsDoingCounterAttackAnimation())
		return false;

	EAIActionState currentState = animRef->GetCurrentActionState();
	bool damageCauserHoldingHeavyWeapon = _damageCauserPawnStats->IsUsingHeavyWeapon();

	// 3. condition check based on the character type
	bool wrongState = currentState == EAIActionState::GettingHurt || currentState == EAIActionState::GettingStuned;

	bool accept = !wrongState || ParryEndurance <= 0;
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
			UE_LOG(LogTemp, Error, TEXT("baseAICtrl is nullptr - ADaiKatana_Enemy::OnParry"));
			return false;
		}
		baseAICtrl->SetIsPaused_BBC(true);

		//int parrySectionName = GetParrySectionNameIndex(damageCauserHoldingHeavyWeapon, _damageInfo);

		animRef->OnParry(GetParrySectionName(damageCauserHoldingHeavyWeapon, _damageInfo));

		UParticleSystem * sparkVFX = UVfxManager::GetVfx(EVfxType::metalImpact_sputtering);

		USoundCue* sparkSFX = UAudioManager::GetSFX(ESoundEffectType::EMeleeCounterAttackImpact);

		if (sparkSFX && sparkVFX)
		{
			UWorld* world = GetWorld();
			UGameplayStatics::SpawnEmitterAtLocation(world, sparkVFX, _damageInfo->hitResult.ImpactPoint);
			UGameplayStatics::PlaySoundAtLocation(world, sparkSFX, _damageInfo->hitResult.ImpactPoint);
		}

		// reset parry endurance, and one less parry for counter 
		ParryEndurance = GetParryEndurance();
		CounterEndurance--;

		return true;
	}
	else
		return false;

}

bool ADaiKatana_Enemy::OnAutoDodge(const FDamageInfo * const _damageInfo, const UPawnStatsComponent * const _damageCauserPawnStats)
{

	// 1. Action check, is this ai 's current action is fit for dodging ?
	UAIMelee_AnimInstance* animRef = Cast<UAIMelee_AnimInstance>(GetAnimInstanceRef());
	if (animRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("animRef == nullptr, ADaiKatana_Enemy::OnParry"));
		return false;
	}

	EAIActionState currentState = animRef->GetCurrentActionState();
	bool isRightState =
		currentState == EAIActionState::MeleePreAttack ||
		currentState == EAIActionState::MeleePostAttack || 
		currentState == EAIActionState::None;

	bool accept = isRightState;

	if (!accept)
		return false;

	// 3. direction check, less than 45 from forward vector
	float forwardDot = FVector::DotProduct(GetActorForwardVector(), _damageInfo->hitDirection);

	if (forwardDot < 0.707f && forwardDot >= -0.5f)
	{
		ATheLastBastionBaseAIController* baseAICtrl
			= Cast<ATheLastBastionBaseAIController>(GetController());
		if (baseAICtrl == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("baseAICtrl is nullptr - ADaiKatana_Enemy::OnParry"));
			return false;
		}
		baseAICtrl->SetIsPaused_BBC(true);

		animRef->OnDodge(GetDodgeSectionName(_damageInfo));

		USoundCue* sparkSFX = UAudioManager::GetSFX(ESoundEffectType::ERoll);

		if (sparkSFX)
		{
			UWorld* world = GetWorld();
			UGameplayStatics::PlaySoundAtLocation(world, sparkSFX, _damageInfo->hitResult.ImpactPoint);
		}

		return true;
	}
	else
		return false;
}

FName ADaiKatana_Enemy::GetParrySectionName(bool _damageByHeavyWeapon, const FDamageInfo * const _damageInfo) const
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
				return TEXT("Parry_Top_Mid");
			else
			{
				float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
				if (dirDotRight > 0)
					return TEXT("Parry_Top_Right");
				else
					return TEXT("Parry_Top_Left");
			}

		}
		else
		{
			// Upper body hit
			float dirDotFor = FVector::DotProduct(hitDir, GetActorForwardVector());

			if (dirDotFor >= 0.86f)
				return TEXT("Parry_Up_Mid_Left");
			else
			{
				float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
				if (dirDotRight > 0)
					return TEXT("Parry_Up_Right");
				else
					return TEXT("Parry_Up_Mid_Left");
			}

		}
	}
	else
	{
		float dirDotFor = FVector::DotProduct(hitDir, GetActorForwardVector());

		if (dirDotFor >= 0.86f)
			return TEXT("Parry_Down_Mid_Left");
		else
		{
			float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
			if (dirDotRight > 0)
				return TEXT("Parry_Down_Right");
			else
				return TEXT("Parry_Down_Mid_Left");;
		}
	}
}

FName ADaiKatana_Enemy::GetDodgeSectionName(const FDamageInfo * const _damageInfo) const
{

	FName animSection;

	FVector impactPoint = _damageInfo->hitResult.ImpactPoint;

	FVector chestLocation = GetMesh()->GetBoneLocation(TEXT("spine_03"));

	// top hit
	if (impactPoint.Z > chestLocation.Z)
		animSection = TEXT("Dodge_From_Top");
	else
	{
		// if damageCauser on right
		float dotRight = FVector::DotProduct(GetActorRightVector(), _damageInfo->hitDirection);
		if (dotRight > 0)
			animSection = TEXT("Dodge_From_Right");
		else
			animSection = TEXT("Dodge_From_Left");
	}
		
	return animSection;
}

int ADaiKatana_Enemy::GetMeleeComboSel(bool _bIsMoving) const
{
	if (_bIsMoving)
		return bAttackFromRight ? FMath::RandRange(NK_Move_Right_Min, NK_Move_Right_Max) :
		FMath::RandRange(NK_Move_Left_Min, NK_Move_Left_Max);
	else
		return bAttackFromRight ? FMath::RandRange(NK_InPlace_Right_Min, NK_InPlace_Right_Max) :
		FMath::RandRange(NK_InPlace_Left_Min, NK_InPlace_Left_Max);
}

void ADaiKatana_Enemy::ClearEndurance()
{
	CounterEndurance = GetCounterEndurance();
	ParryEndurance = GetParryEndurance();
}

bool ADaiKatana_Enemy::ShouldPlayHitAnimation() const
{
	return mAnimInstanceRef->GetCurrentActionState() != EAIActionState::GettingHurt;
}
