// Fill out your copyright notice in the Description page of Project Settings.

#include "Base_AnimInstance.h"
#include "TheLastBastionCharacter.h"
#include "TheLastBastionHeroCharacter.h"
#include "AudioManager.h"
#include "Components/AudioComponent.h"
#include "Combat/Armor.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Combat/Weapon.h"
#include "VfxManager.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "ConstructorHelpers.h"

#define SN_GetUpFromBack TEXT("GetUpFromBack")
#define SN_GetUpFromFace TEXT("GetUpFromFace")


UBase_AnimInstance::UBase_AnimInstance(const FObjectInitializer& _objectInitalizer)
{
	FindMontage(AM_SingleHandWeapon_HitReaction, 
		TEXT("/Game/Blueprints/AnimationBP/AnimAssets/Montage_SSword_HitReaction"));
	FindMontage(AM_CB_HitReaction,
		TEXT("/Game/Blueprints/AnimationBP/AnimAssets/Montage_CB_HitReaction"));
	FindMontage(AM_HV_HitReaction,
		TEXT("/Game/Blueprints/AnimationBP/AnimAssets/Montage_HV_HitReaction"));
	FindMontage(AM_Sns_HitReaction,
		TEXT("/Game/Blueprints/AnimationBP/AnimAssets/Montage_SwordShieldHitReaction"));
	FindMontage(AM_TH_HitReaction,
		TEXT("/Game/Blueprints/AnimationBP/AnimAssets/Montage_TH_HitReaction"));

	FindMontage(AM_CrossBow, 
		TEXT("/Game/Blueprints/AnimationBP/AnimAssets/Montage_CBFIre"));
	FindMontage(AM_Skill,
		TEXT("/Game/Blueprints/AnimationBP/AnimAssets/Montage_HeroSkills"));
	FindMontage(AM_HV_ParryDodge,
		TEXT("/Game/Blueprints/AnimationBP/AnimAssets/Montage_HV_Parry"));
	FindMontage(AM_SH_Parry,
		TEXT("/Game/Blueprints/AnimationBP/AnimAssets/Montage_SH_Parry"));
	FindMontage(AM_SH_Dodge,
		TEXT("/Game/Blueprints/AnimationBP/AnimAssets/Montage_SH_Dodge"));
	FindMontage(AM_Sns_Parry,
		TEXT("/Game/Blueprints/AnimationBP/AnimAssets/Montage_Sns_Parry"));

}

void UBase_AnimInstance::OnBeginPlay()
{
	OnMontageStarted.AddDynamic(this, &UBase_AnimInstance::OnMontageStartHandle);
	OnMontageBlendingOut.AddDynamic(this, &UBase_AnimInstance::OnMontageBlendOutStartHandle);
}

void UBase_AnimInstance::OnInit()
{
}

void UBase_AnimInstance::OnUpdate(float _deltaTime)
{
	if (mBaseCharacter && mBaseCharacter->IsRagDoll())
	{
		if (mBaseCharacter->IsRagDollRecovereing())
		{
			mBaseCharacter->DuringRagDollRecovering(_deltaTime);
		}
		else
		{
			mBaseCharacter->DuringRagDoll();
		}
	}
}

void UBase_AnimInstance::OnPostEvaluate()
{
}

void UBase_AnimInstance::OnBeingHit(FName boneName, const FVector & _damageCauseRelative, const FVector & _hitLocation) {}

void UBase_AnimInstance::AnimInstanceResetOnRagDoll() {}

void UBase_AnimInstance::OnGetUp(bool _isFaceDown)
{
	if (GetUp_Montage == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GetUp_Montage == nullptr  - UBase_AnimInstance::OnGetUp() "));
		return;
	}
	if (_isFaceDown)
		PlayMontage(GetUp_Montage, 1.0f, SN_GetUpFromFace);
	else
		PlayMontage(GetUp_Montage, 1.0f, SN_GetUpFromBack);
}

float UBase_AnimInstance::PlayMontage(class UAnimMontage* _animMontage, float _rate, FName _startSectionName)
{
	if (_animMontage)
	{
		float const duration = this->Montage_Play(_animMontage, _rate);
		if (duration > 0.f)
		{
			if (_startSectionName != NAME_None)
			{
				this->Montage_JumpToSection(_startSectionName, _animMontage);
			}
		}
		return duration;
	}
	return 0.f;

}

void UBase_AnimInstance::OnMontageStartHandle(UAnimMontage * _animMontage)
{

}

void UBase_AnimInstance::OnMontageEndHandle(UAnimMontage * _animMontage, bool _bInterruptted)
{
}

void UBase_AnimInstance::OnMontageBlendOutStartHandle(UAnimMontage * _animMontage, bool _bInterruptted)
{

	if (mBaseCharacter)
	{
		if (_animMontage == GetUp_Montage  && mBaseCharacter->IsOldKnockOut())
		{
			mBaseCharacter->RagDollRecoverOnFinish();
		}

	}
}

void UBase_AnimInstance::ResetOnBeingHit() {}

void UBase_AnimInstance::FxFootStep()
{
	if (mBaseCharacter)
	{
		USoundCue* sfx = nullptr;
		AArmor* armor = mBaseCharacter->GetCurrentArmor();
		UParticleSystem* vfx = UVfxManager::GetVfx(EVfxType::FootSteps);
		FVector FootLocation = mBaseCharacter->GetMesh()->GetBoneLocation(TEXT("ik_foot_root"), EBoneSpaces::WorldSpace);
		if (armor && armor->IsHeavyArmor())
		{

		}
		else
		{
			sfx = UAudioManager::GetSFX(ESoundEffectType::ELightArmorFootStepOnDirt);
		}

		if (sfx && vfx)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), sfx, FootLocation);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), vfx, FootLocation);
		}
	}
}

void UBase_AnimInstance::FxMeleeSwing(bool _rightHand)
{
	if (mBaseCharacter)
	{
		AGear* CurrentWeapon = (_rightHand)? mBaseCharacter->GetCurrentWeapon() : mBaseCharacter->GetCurrentSecondaryWeapon();
		if (CurrentWeapon)
		{   
			UAudioComponent* AudioComp = mBaseCharacter->GetAudioComp();
			USoundCue* sfx = UAudioManager::GetSFX(ESoundEffectType::EMeleeWeaponSwing);
			EGearType GearType = CurrentWeapon->GetGearType();
			int weaponType = 0;
			switch (GearType)
			{
			case EGearType::LongSword:
			case EGearType::WarAxe:
				weaponType = 0;
				break;
			case EGearType::DoubleHandWeapon:
				weaponType = 1;
				break;
			case EGearType::Mace:
			case EGearType::BattleAxe:
			case EGearType::GreatSword:
			case EGearType::Hammer:
			case EGearType::Shield:
				weaponType = 2;
				break;
			default:
				return;
			}
			AudioComp->SetSound(sfx);
			AudioComp->SetIntParameter(TEXT("WeaponType"), weaponType);
			AudioComp->Play();
		}
	}
}

void UBase_AnimInstance::FxOnEquip()
{
	if (mBaseCharacter)
	{
		AGear* CurrentWeapon = mBaseCharacter->GetCurrentWeapon();
		if (CurrentWeapon)
		{
			UAudioComponent* AudioComp = mBaseCharacter->GetAudioComp();
			USoundCue* sfx = UAudioManager::GetSFX(ESoundEffectType::EWeaponEquip);
			EGearType GearType = CurrentWeapon->GetGearType();
			int weaponType = 0;
			switch (GearType)
			{
			case EGearType::LongSword:
				weaponType = 0;
				break;

			case EGearType::DoubleHandWeapon:
				weaponType = 1;
				break;

			case EGearType::CrossBow:
				weaponType = 2;
				break;

			case EGearType::WarAxe:
				weaponType = 3;
				break;

			case EGearType::Mace:
				weaponType = 4;
				break;

			case EGearType::BattleAxe:
			case EGearType::GreatSword:
				weaponType = 5;
				break;

			case EGearType::Hammer:
				weaponType = 6;
				break;

			default:
				return;
			}
			AudioComp->SetSound(sfx);
			AudioComp->SetIntParameter(TEXT("WeaponType"), weaponType);
			AudioComp->Play(0.4f);
		}
	}
}

void UBase_AnimInstance::FxOnDraw()
{
	if (mBaseCharacter)
	{
		AGear* CurrentWeapon = mBaseCharacter->GetCurrentWeapon();
		if (CurrentWeapon)
		{
			UAudioComponent* AudioComp = mBaseCharacter->GetAudioComp();
			USoundCue* sfx = UAudioManager::GetSFX(ESoundEffectType::EWeaponDraw);
			EGearType GearType = CurrentWeapon->GetGearType();
			int weaponType = 0;
			switch (GearType)
			{
			case EGearType::LongSword:
			case EGearType::DoubleHandWeapon:
				weaponType = 0;
				break;

			case EGearType::CrossBow:
				weaponType = 1;
				break;

			case EGearType::WarAxe:
				weaponType = 2;
				break;

			case EGearType::Mace:
				weaponType = 3;
				break;

			case EGearType::BattleAxe:
			case EGearType::GreatSword:
				weaponType = 4;
				break;

			case EGearType::Hammer:
				weaponType = 5;
				break;

			default:
				return;
			}
			AudioComp->SetSound(sfx);
			AudioComp->SetIntParameter(TEXT("WeaponType"), weaponType);
			AudioComp->Play(0.2f);
		}
	}
}

void UBase_AnimInstance::FxOnReload()
{
	if (mBaseCharacter)
	{
		AGear* CurrentWeapon = mBaseCharacter->GetCurrentWeapon();
		if (CurrentWeapon)
		{
			UAudioComponent* AudioComp = mBaseCharacter->GetAudioComp();
			USoundCue* sfx = UAudioManager::GetSFX(ESoundEffectType::ECrossbowReload);
			AudioComp->SetSound(sfx);
			AudioComp->Play();
		}
	}
}

void UBase_AnimInstance::FxOnJump()
{
	if (mBaseCharacter)
	{
		USoundCue* sfx = UAudioManager::GetSFX(ESoundEffectType::EJump);
		if (sfx)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), sfx, mBaseCharacter->GetActorLocation());
	}
}

void UBase_AnimInstance::FxOnRoll()
{
	if (mBaseCharacter)
	{
		USoundCue* sfx = UAudioManager::GetSFX(ESoundEffectType::ERoll);
		if (sfx)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), sfx, mBaseCharacter->GetActorLocation());
	}
}

void UBase_AnimInstance::StartMeleeWeaponTrail(bool _rightHand)
{
	
	AGear* CurrentWeapon = _rightHand ? mBaseCharacter->GetCurrentWeapon() : mBaseCharacter->GetCurrentSecondaryWeapon();
	ATheLastBastionHeroCharacter* HeroCharacter = Cast<ATheLastBastionHeroCharacter>(mBaseCharacter);
	if (CurrentWeapon&&HeroCharacter)
	{
		UParticleSystemComponent* ParticleSystemComp = CurrentWeapon->GetParticleSystemComp();
		if (ParticleSystemComp)
		{   
			UParticleSystem* WeaponTrail = nullptr;
			// replace particle template if player has encharted weapon
			if (HeroCharacter->HasEnchartedWeapon() == true && _rightHand == true)
				WeaponTrail = UVfxManager::GetVfx(EVfxType::WeaponFireTrail);

			else
				WeaponTrail = UVfxManager::GetVfx(EVfxType::WeaponNormalTrail);

			ParticleSystemComp->Template = WeaponTrail;
			ParticleSystemComp->BeginTrails(TEXT("TrailStart"), TEXT("TrailEnd"), ETrailWidthMode::ETrailWidthMode_FromCentre, 1.0f);
		}
	}
}

void UBase_AnimInstance::EndMeleeWeaponTrail(bool _rightHand)
{
	AGear* CurrentWeapon = _rightHand ? mBaseCharacter->GetCurrentWeapon() : mBaseCharacter->GetCurrentSecondaryWeapon();
	if (CurrentWeapon)
	{
		UParticleSystemComponent* ParticleSystemComp = CurrentWeapon->GetParticleSystemComp();
		if (ParticleSystemComp)
			ParticleSystemComp->EndTrails();
	}
}



bool UBase_AnimInstance::FindMontage(class UAnimMontage*& _animMontage, const TCHAR * _path)
{
	if (_animMontage == nullptr)
	{
		ConstructorHelpers::FObjectFinder<UAnimMontage> ps_finder(_path);
		_animMontage = ps_finder.Object;
	}

	return (_animMontage != nullptr);
}
