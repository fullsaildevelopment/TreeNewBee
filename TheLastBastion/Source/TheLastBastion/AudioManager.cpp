// Fill out your copyright notice in the Description page of Project Settings.

#include "AudioManager.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "ConstructorHelpers.h"
#include "CustomType.h"
#include "Combat/Gear.h"



USoundCue* UAudioManager::CrossbowFire = nullptr;
USoundCue* UAudioManager::SingleHandSwordSlash = nullptr;
USoundCue* UAudioManager::BoltsFleshCharacter = nullptr;
USoundCue* UAudioManager::BoltsMetalImpact = nullptr;
USoundCue* UAudioManager::BoltsSticks_LightShield = nullptr;
USoundCue* UAudioManager::BoltsSticks_HeavyShield = nullptr;
USoundCue* UAudioManager::LightWpnFleshImpact = nullptr;
USoundCue* UAudioManager::MeleeCounterAttackImpactpact = nullptr;


UAudioManager::UAudioManager()
{   
	// Find all the sound cues of our game
	CrossbowFire = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/CrossBow/CrossBow_Fire"));

	SingleHandSwordSlash = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Sword/SwordAttack"));

	BoltsFleshCharacter = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/BoltsHitImpact/BoltsImpactOnCharacter"));
	BoltsSticks_LightShield = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/BoltsHitImpact/BoltsSticks_LightShield"));
	BoltsSticks_HeavyShield = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/BoltsHitImpact/BoltsSticks_HeavyShield"));

	BoltsMetalImpact = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/BoltsHitImpact/BoltsImpactOnMetal"));

	LightWpnFleshImpact = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/MeleeWeaponHitImpact/Light_Flesh_Impact"));

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
	case ESoundEffectType::EBoltsFleshCharacter:
		return BoltsFleshCharacter;
	case ESoundEffectType::EBoltsMetalImpact:
		return BoltsMetalImpact;
	case ESoundEffectType::EBoltsStickToLightShield:
		return BoltsSticks_LightShield;
	case ESoundEffectType::EBoltsStickToHeavyShield:
		return BoltsSticks_HeavyShield;		
	case ESoundEffectType::ELightWpnFleshImpact:
		return LightWpnFleshImpact;
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
		return BoltsFleshCharacter;
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

USoundCue * UAudioManager::GetMeleeImpactByMaterialAndGearType(EGearType _gearType, EPhysicalSurface _surfaceType)
{

	switch (_gearType)
	{
	case SURFACE_FLESH:
		return BoltsFleshCharacter;
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

USoundCue * UAudioManager::GetSingleHandWeaponImpactByMaterial(EPhysicalSurface _surfaceType)
{

	return nullptr;
}


