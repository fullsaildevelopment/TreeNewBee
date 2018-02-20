// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AudioManager.generated.h"


enum class EGearType :uint8;

UENUM()
enum class ESoundEffectType : uint8
{
	ECrossBowFire = 0,
	ESingeHandSwordSlash,
	EBoltsFleshImpact,
	EBoltsMetalImpact,
	EBoltsStickToLightShield,
	EBoltsStickToHeavyShield,
	ELightWeaponImpact,
	EHeavyWeaponImpact,
	EMeleeCounterAttackImpact,

};
/**
 * 
 */
UCLASS()
class THELASTBASTION_API UAudioManager : public UObject
{
	GENERATED_BODY()
			
public:
	UAudioManager();

private:
	static class USoundCue* CrossbowFire;

	static class USoundCue* SingleHandSwordSlash;

	static class USoundCue* BoltsFleshImpact;
	static class USoundCue* BoltsMetalImpact;
	static class USoundCue* BoltsSticks_LightShield;
	static class USoundCue* BoltsSticks_HeavyShield;

	// Melee Weapon SFX
	static class USoundCue* LightWeaponImpact;
	static class USoundCue* HeavyWeaponImpact;
	static class USoundCue* MeleeCounterAttackImpactpact;



public:

	static class USoundCue* GetSFX(ESoundEffectType _sfxType);

	static class USoundCue* GetProjectileImpactByMaterial(EPhysicalSurface _surfaceType);

	static class USoundCue* GetMeleeWeaponImpactSFXByGearType(EGearType _gearType);

private:

	USoundCue* FindSoundCue(const TCHAR* _path);

};
