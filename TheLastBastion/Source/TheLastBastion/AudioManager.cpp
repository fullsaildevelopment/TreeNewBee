// Fill out your copyright notice in the Description page of Project Settings.

#include "AudioManager.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "ConstructorHelpers.h"
#include "CustomType.h"
#include "Combat/Gear.h"



USoundCue* UAudioManager::CrossbowFire = nullptr;
USoundCue* UAudioManager::SingleHandSwordSlash = nullptr;
USoundCue* UAudioManager::BoltsFleshImpact = nullptr;
USoundCue* UAudioManager::BoltsMetalImpact = nullptr;
USoundCue* UAudioManager::BoltsSticks_LightShield = nullptr;
USoundCue* UAudioManager::BoltsSticks_HeavyShield = nullptr;
USoundCue* UAudioManager::LightWeaponImpact = nullptr;
USoundCue* UAudioManager::HeavyWeaponImpact = nullptr;
USoundCue* UAudioManager::MeleeCounterAttackImpactpact = nullptr;


UAudioManager::UAudioManager()
{   
	// Find all the sound cues of our game
	CrossbowFire = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/CrossBow/CrossBow_Fire"));

	SingleHandSwordSlash = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Sword/SwordAttack"));

	BoltsFleshImpact = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/BoltsHitImpact/BoltsImpactOnCharacter"));
	BoltsSticks_LightShield = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/BoltsHitImpact/BoltsSticks_LightShield"));
	BoltsSticks_HeavyShield = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/BoltsHitImpact/BoltsSticks_HeavyShield"));

	BoltsMetalImpact = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/BoltsHitImpact/BoltsImpactOnMetal"));

	LightWeaponImpact = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/MeleeWeaponHitImpact/LightWeaponImpact"));
	HeavyWeaponImpact = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/MeleeWeaponHitImpact/HeavyWeaponImpact"));
	MeleeCounterAttackImpactpact = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/MeleeWeaponHitImpact/MeleeCounterAttackImpact"));
}

USoundCue* UAudioManager::FindSoundCue(const TCHAR* _path)
{
	ConstructorHelpers::FObjectFinder<USoundCue> SoundCueFinder(_path);
	if (SoundCueFinder.Succeeded())
		return SoundCueFinder.Object;
	else
		return nullptr;
}

USoundCue * UAudioManager::GetSFX(ESoundEffectType _sfxType)
{
	switch (_sfxType)
	{
	case ESoundEffectType::ECrossBowFire:
		return CrossbowFire;

	case ESoundEffectType::ESingeHandSwordSlash:
		return SingleHandSwordSlash;

	case ESoundEffectType::EBoltsFleshImpact:
		return BoltsFleshImpact;

	case ESoundEffectType::EBoltsMetalImpact:
		return BoltsMetalImpact;

	case ESoundEffectType::EBoltsStickToLightShield:
		return BoltsSticks_LightShield;

	case ESoundEffectType::EBoltsStickToHeavyShield:
		return BoltsSticks_HeavyShield;	

	case ESoundEffectType::ELightWeaponImpact:
		return LightWeaponImpact;

	case ESoundEffectType::EHeavyWeaponImpact:
		return HeavyWeaponImpact;

	case ESoundEffectType::EMeleeCounterAttackImpact:
		return MeleeCounterAttackImpactpact;

	default:
		return nullptr;
	}

}

USoundCue * UAudioManager::GetProjectileImpactByMaterial(EPhysicalSurface _surfaceType)
{

	switch (_surfaceType)
	{
	case SURFACE_FLESH:
		return BoltsFleshImpact;
	case SURFACE_METAL:
		return BoltsMetalImpact;
	case SURFACE_LightShield:
		return BoltsSticks_LightShield;
	case SURFACE_HeavyShield:
		return BoltsSticks_HeavyShield;
	default:
		return nullptr;
	}

}

USoundCue* UAudioManager::GetMeleeWeaponImpactSFXByGearType(EGearType _gearType)
{
	switch (_gearType)
	{
	case EGearType::Armor:
		return nullptr;

	case EGearType::Shield:
		return nullptr;

	case EGearType::LongSword:
		return LightWeaponImpact;

	case EGearType::DoubleHandWeapon:
		return LightWeaponImpact;

	case EGearType::CrossBow:
		return nullptr;

	case EGearType::CrossBowBolt:
		return nullptr;

	case EGearType::TwinBlade:
		return nullptr;

	case EGearType::GreatSword:
		return HeavyWeaponImpact;

	case EGearType::Bow:
		return nullptr;

	case EGearType::Arrow:
		return nullptr;

	case EGearType::WarAxe:
		return LightWeaponImpact;

	case EGearType::Mace:
		return LightWeaponImpact;

	case EGearType::BattleAxe:
		return HeavyWeaponImpact;

	case EGearType::Hammer:
		return HeavyWeaponImpact;

	default:
		return nullptr;
	}
}



