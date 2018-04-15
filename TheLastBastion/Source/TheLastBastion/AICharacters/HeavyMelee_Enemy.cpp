// Fill out your copyright notice in the Description page of Project Settings.

#include "HeavyMelee_Enemy.h"
#include "Animation/AIMelee_AnimInstance.h"
#include "Combat/PawnStatsComponent.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "AudioManager.h"
#include "VfxManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


bool AHeavyMelee_Enemy::OnParry(const struct FDamageInfo* const _damageInfo, const class UPawnStatsComponent* const _damageCauserPawnStats)
{

	bool accept = false;
	if (CharacterType != ECharacterType::Lan_QueenGuard)
	{
		return false;
		// 1. weapon check, queens guard can block attack from any melee weapon

		// 2. endurance check 
	}

	// 3. Action check, is this ai 's current action is fit for parrying ?
	UAIMelee_AnimInstance* animRef = Cast<UAIMelee_AnimInstance>(GetAnimInstanceRef());
	if (animRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("animRef == nullptr, AHeavyMelee::OnParry"));
		return accept;
	}

	EAIActionState currentState = animRef->GetCurrentActionState();
	//UE_LOG(LogTemp, Warning, TEXT("currentState: %d - AHeavyMelee::OnParry"), (int)currentState);
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
				UE_LOG(LogTemp, Error, TEXT("baseAICtrl is nullptr - AHeavyMelee::OnParry"));
				return false;
			}
			baseAICtrl->SetIsPaused_BBC(true);			
			accept = true;
			//
			animRef->OnParry(GetParrySectionName(_damageInfo), AM_HV_ParryDodge);
			//UE_LOG(LogTemp, Warning, TEXT("Parrying - AHeavyMelee::OnParry"));

			// fx

			UParticleSystem * sparkVFX = UVfxManager::GetVfx(EVfxType::metalImpact_sputtering);
			USoundCue* sparkSFX = UAudioManager::GetSFX(ESoundEffectType::EMeleeCounterAttackImpact);

			if (sparkSFX && sparkVFX)
			{
				UWorld* world = GetWorld();
				UGameplayStatics::SpawnEmitterAtLocation(world, sparkVFX, _damageInfo->hitResult.ImpactPoint);
				UGameplayStatics::PlaySoundAtLocation(world, sparkSFX, _damageInfo->hitResult.ImpactPoint);
			}

			return accept;			
		}
	}
	else
	{
		return accept;
	}


	return false;
}

int AHeavyMelee_Enemy::GetMeleeComboSel(bool _bIsMoving) const
{
	return 0;
	bool IsQueensGuard = CharacterType == ECharacterType::Lan_QueenGuard;
	if (_bIsMoving)
		return IsQueensGuard ? FMath::RandRange(HV_ComboSel_Move_Sr_Min, HV_ComboSel_Move_Sr_Max) : FMath::RandRange(HV_ComboSel_Move_Jr_Min, HV_ComboSel_Move_Jr_Max);
	else
		return IsQueensGuard ? FMath::RandRange(HV_ComboSel_InPlace_Sr_Min, HV_ComboSel_InPlace_Sr_Max) : FMath::RandRange(HV_ComboSel_InPlace_Jr_Min, HV_ComboSel_InPlace_Jr_Max);
}

FName AHeavyMelee_Enemy::GetParrySectionName(const struct FDamageInfo* const _damageInfo) const
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

bool AHeavyMelee_Enemy::ShouldPlayHitAnimation() const
{

	if (CharacterType == ECharacterType::Lan_QueenGuard)
		return mAnimInstanceRef->GetCurrentActionState() != EAIActionState::GettingHurt;
	else
		return true;

}
