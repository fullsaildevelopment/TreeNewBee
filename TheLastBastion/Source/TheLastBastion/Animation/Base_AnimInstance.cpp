// Fill out your copyright notice in the Description page of Project Settings.

#include "Base_AnimInstance.h"
#include "TheLastBastionCharacter.h"
#include "AudioManager.h"
#include "Components/AudioComponent.h"
#include "Combat/Armor.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#define SN_GetUpFromBack TEXT("GetUpFromBack")
#define SN_GetUpFromFace TEXT("GetUpFromFace")


UBase_AnimInstance::UBase_AnimInstance(const FObjectInitializer& _objectInitalizer)
{
	
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
		if (armor && armor->IsHeavyArmor())
		{

		}
		else
		{
			sfx = UAudioManager::GetSFX(ESoundEffectType::ELightArmorFootStep);
		}
		if (sfx)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), sfx, mBaseCharacter->GetActorLocation());
	}
}

void UBase_AnimInstance::FxMeleeSwing()
{
	if (mBaseCharacter)
	{
		AGear* CurrentWeapon = mBaseCharacter->GetCurrentWeapon();
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
			case EGearType::WarAxe:
			case EGearType::CrossBow:
				weaponType = 0;
				break;
			case EGearType::DoubleHandWeapon:
				weaponType = 1;
				break;
			case EGearType::Mace:
			case EGearType::BattleAxe:
			case EGearType::GreatSword:
			case EGearType::Hammer:
				weaponType = 2;
				break;
			default:
				return;
			}
			AudioComp->SetSound(sfx);
			AudioComp->SetIntParameter(TEXT("GearType"), weaponType);
			AudioComp->Play();
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
			case EGearType::WarAxe:
			case EGearType::CrossBow:
				weaponType = 0;
				break;
			case EGearType::DoubleHandWeapon:
				weaponType = 1;
				break;
			case EGearType::Mace:
			case EGearType::BattleAxe:
			case EGearType::GreatSword:
			case EGearType::Hammer:
				weaponType = 2;
				break;
			default:
				return;
			}
			AudioComp->SetSound(sfx);
			AudioComp->SetIntParameter(TEXT("GearType"), weaponType);
			AudioComp->Play();
		}
	}
}

void UBase_AnimInstance::FxOnReload()
{
}
