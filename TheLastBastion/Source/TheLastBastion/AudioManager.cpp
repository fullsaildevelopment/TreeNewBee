// Fill out your copyright notice in the Description page of Project Settings.

#include "AudioManager.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"


USoundCue* UAudioManager::CrossBowFire = nullptr;

UAudioManager::UAudioManager()
{   

	ConstructorHelpers::FObjectFinder<USoundCue> cb_fire (TEXT("/Game/Assets/Audio/AudioCues/CrossBow/CrossBow_Fire"));
	if (cb_fire.Succeeded())
		CrossBowFire = cb_fire.Object;
}

USoundCue * UAudioManager::GetSFX(ESoundEffectType _sfxType)
{

	switch (_sfxType)
	{
	case ESoundEffectType::ECrossBowFire:
		return CrossBowFire;
	default:
		return nullptr;
	}

}


