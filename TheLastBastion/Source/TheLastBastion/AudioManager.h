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
	EBoltsFleshCharacter,
	EBoltsStickToLightShield,
	EBoltsStickToHeavyShield,
	ELightWpnFleshImpact,
	EMeleeCounterAttackImpact,
	EBoltsMetalImpact
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

	static class USoundCue* BoltsFleshCharacter;
	static class USoundCue* BoltsMetalImpact;
	static class USoundCue* BoltsSticks_LightShield;
	static class USoundCue* BoltsSticks_HeavyShield;

	static class USoundCue* LightWpnFleshImpact;

	static class USoundCue* MeleeCounterAttackImpactpact;



public:

	static class USoundCue* GetSFX(ESoundEffectType _sfxType);

	static class USoundCue* GetProjectileImpactByMaterial(EPhysicalSurface _surfaceType);

	static class USoundCue* GetMeleeImpactByMaterialAndGearType(EGearType _gearType, EPhysicalSurface _surfaceType);

	static class USoundCue* GetSingleHandWeaponImpactByMaterial(EPhysicalSurface _surfaceType);


private:

	USoundCue* FindSoundCue(const TCHAR* _path);

};
