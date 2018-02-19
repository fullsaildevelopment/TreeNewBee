// Fill out your copyright notice in the Description page of Project Settings.

#include "AudioManager.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "ConstructorHelpers.h"


USoundCue* UAudioManager::CrossbowFire = nullptr;
USoundCue* UAudioManager::SingleHandSwordSlash = nullptr;
USoundCue* UAudioManager::BoltsFleshCharacter = nullptr;
USoundCue* UAudioManager::BoltsStickToShield = nullptr;
USoundCue* UAudioManager::SwordFleshImpact = nullptr;

UAudioManager::UAudioManager()
{   
	// Find all the sound cues of our game
	CrossbowFire = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/CrossBow/CrossBow_Fire"));

	SingleHandSwordSlash = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Sword/SwordAttack"));

	BoltsFleshCharacter = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/BoltsHitImpact/BoltsImpactOnCharacter"));

	BoltsStickToShield = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/BoltsHitImpact/BoltsSticksToShield"));

	SwordFleshImpact = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/MeleeWeaponHitImpact/SwordFleshImpact"));
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

	case ESoundEffectType::EBoltsStickToShield:
		return BoltsStickToShield;

	case ESoundEffectType::ESwordFleshImpact:
		return SwordFleshImpact;

	default:
		return nullptr;
	}

}


