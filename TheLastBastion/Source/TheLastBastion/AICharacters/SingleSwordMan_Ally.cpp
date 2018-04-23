// Fill out your copyright notice in the Description page of Project Settings.

#include "SingleSwordMan_Ally.h"
#include "Animation/AIMelee_AnimInstance.h"
#include "Combat/PawnStatsComponent.h"
#include "Combat/Gear.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "AudioManager.h"
#include "VfxManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


bool ASingleSwordMan_Ally::OnParry(const struct FDamageInfo* const _damageInfo,
	const class UPawnStatsComponent* const _damageCauserPawnStats)
{
	//1. Check correct character type and endurance

	if (CharacterType != ECharacterType::Nord_Power_LS)
		return false;

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

	bool accept = ParryEndurance <= 0 || isRightState;
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

bool ASingleSwordMan_Ally::OnAutoDodge(const FDamageInfo * const _damageInfo, const UPawnStatsComponent * const _damageCauserPawnStats)
{
	//1. Check correct character type 
	if (CharacterType != ECharacterType::Wolf_Fast_LS && CharacterType != ECharacterType::Nord_Power_LS)
		return false;

	// 2. Action check, is this ai 's current action is fit for dodging ?
	UAIMelee_AnimInstance* animRef = Cast<UAIMelee_AnimInstance>(GetAnimInstanceRef());
	if (animRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("animRef == nullptr, AHeavyMelee::OnParry"));
		return false;
	}

	EAIActionState currentState = animRef->GetCurrentActionState();
	bool isRightState =
		currentState == EAIActionState::MeleePreAttack ||
		currentState == EAIActionState::MeleePostAttack || currentState == EAIActionState::None;

	bool accept = DodgeEndurance <= 0 || isRightState;

	if (!accept)
		return false;

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

		animRef->OnDodge(GetDodgeSectionName(_damageInfo));

		USoundCue* sparkSFX = UAudioManager::GetSFX(ESoundEffectType::ERoll);

		if (sparkSFX)
		{
			UWorld* world = GetWorld();
			UGameplayStatics::PlaySoundAtLocation(world, sparkSFX, _damageInfo->hitResult.ImpactPoint);
		}

		// reset the endurance for next parry
		DodgeEndurance = GetAutoDodgeEndurance();

		return true;
	}
	else
		return false;
}

int ASingleSwordMan_Ally::GetMeleeComboSel(bool _bIsMoving) const
{
	switch (CharacterType)
	{
	case ECharacterType::HillTribe_Rookie_LS:
		return GetMeleeComboSel_Rookie(_bIsMoving);
	case ECharacterType::Wolf_Fast_LS:
		return GetMeleeComboSel_Fast(_bIsMoving);
	case ECharacterType::Nord_Power_LS:
		return GetMeleeComboSel_Power(_bIsMoving);
	default:
		return Super::GetMeleeComboSel(_bIsMoving);
	}
}

FName ASingleSwordMan_Ally::GetParrySectionName(const struct FDamageInfo* const _damageInfo) const
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
				animSection = TEXT("Parry_Top_Mid");
			else
			{
				float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
				if (dirDotRight > 0)
					animSection = TEXT("Parry_Top_Right");
				else
					animSection = TEXT("Parry_Upper_Left");
			}

		}
		else
		{
			// Upper body hit
			float dirDotFor = FVector::DotProduct(hitDir, GetActorForwardVector());

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
		float dirDotRight = FVector::DotProduct(hitDir, GetActorRightVector());
		if (dirDotRight > 0)
			animSection = TEXT("Parry_Down_Right");
		else
			animSection = TEXT("Parry_Down_Left");
	}



	return animSection;
}

FName ASingleSwordMan_Ally::GetDodgeSectionName(const FDamageInfo * const _damageInfo) const
{
	// figure out the anim section to play
	FName animSection;

	FVector impactPoint = _damageInfo->hitResult.ImpactPoint;

	FVector hitDir = (impactPoint - GetActorLocation());
	hitDir.Z = 0.0f;
	hitDir = hitDir.GetUnsafeNormal();

	FVector pelvisLocation = GetMesh()->GetBoneLocation(TEXT("pelvis"));

	bool RandSel = FMath::RandRange(-5, 5) >= 0;

	if (impactPoint.Z >= pelvisLocation.Z)
	{
		FVector chestLocation = GetMesh()->GetBoneLocation(TEXT("spine_03"));
		// top hit
		if (impactPoint.Z > chestLocation.Z)
			animSection = (RandSel) ? TEXT("Dodge_Top_0") : TEXT("Dodge_Top_1");
		else
			animSection = (RandSel) ? TEXT("Dodge_Up_0") : TEXT("Dodge_Up_1");
	}
	else
		animSection = (RandSel) ? TEXT("Dodge_Down_0") : TEXT("Dodge_Down_1");
	return animSection;
}

int ASingleSwordMan_Ally::GetMeleeComboSel_Rookie(bool _bIsMoving) const
{

	if (_bIsMoving)
		return bAttackFromRight ? FMath::RandRange(SH_Roo_Move_Right_Min, SH_Roo_Move_Right_Max) :
		FMath::RandRange(SH_Roo_Move_Left_Min, SH_Roo_Move_Left_Max);
	else
		return bAttackFromRight ? FMath::RandRange(SH_Roo_InPlace_Right_Min, SH_Roo_InPlace_Right_Max) :
		FMath::RandRange(SH_Roo_InPlace_Left_Min, SH_Roo_InPlace_Left_Max);
}

int ASingleSwordMan_Ally::GetMeleeComboSel_Fast(bool _bIsMoving) const
{
	if (_bIsMoving)
		return FMath::RandRange(SH_Fast_Move_Min, SH_Fast_Move_Max);
	else
		return bAttackFromRight ? FMath::RandRange(SH_Fast_InPlace_Right_Min, SH_Fast_InPlace_Right_Max) :
		FMath::RandRange(SH_Fast_InPlace_Left_Min, SH_Fast_InPlace_Left_Max);
}

int ASingleSwordMan_Ally::GetMeleeComboSel_Power(bool _bIsMoving) const
{
	if (_bIsMoving)
		return bAttackFromRight ? FMath::RandRange(SH_Pow_Move_Right_Min, SH_Pow_Move_Right_Max) :
		FMath::RandRange(SH_Pow_Move_Left_Min, SH_Pow_Move_Left_Max);
	else
		return bAttackFromRight ? FMath::RandRange(SH_Pow_InPlace_Right_Min, SH_Pow_InPlace_Right_Max) :
		FMath::RandRange(SH_Pow_InPlace_Left_Min, SH_Pow_InPlace_Left_Max);
}






