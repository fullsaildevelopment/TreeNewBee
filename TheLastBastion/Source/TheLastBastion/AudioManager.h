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
	EBoltsHitImpactOnCharacter,
	EBoltsHitImpactOnLightShield,
	EBoltsHitImpactOnHeavyShield,
	EBoltsHitImpactOnTerrain,
	ELightWeaponImpact,
	EHeavyWeaponImpact,
	EMeleeCounterAttackImpact,
	EMeleeWeaponSwing,
	EWeaponEquip,
	EWeaponDraw,
	ELightArmorFootStepOnDirt,
	EPlayerVoiceOnGroupSelection,
	EPlayerVocalCommandsOnGroup
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
	static class USoundCue* BoltsHitImpactOnCharacter;
	static class USoundCue* BoltsHitImpactOnLightShield;
	static class USoundCue* BoltsHitImpactOnHeavyShield;
	static class USoundCue* BoltsHitImpactOnTerrain;

	// Melee Weapon Impact SFX
	static class USoundCue* LightWeaponImpact;
	static class USoundCue* HeavyWeaponImpact;
	static class USoundCue* MeleeCounterAttackImpact;

	// Melee Weapon Swing SFX
	static class USoundCue* MeleeWeaponSwing;

	// Weapon Equip
	static class USoundCue* WeaponEquip;
	static class USoundCue* WeaponDraw;

	// Foot Step
	static class USoundCue* LightArmorFootStepOnDirt;

	// Player Vocal
	static class USoundCue* PlayerVoiceOnGroupSelection;
	static class USoundCue* PlayerVocalCommandsOnGroup;

public:

	static class USoundCue* GetSFX(ESoundEffectType _sfxType);

	static class USoundCue* GetProjectileImpactByMaterial(EPhysicalSurface _surfaceType);

	static class USoundCue* GetMeleeWeaponImpactSFXByGearType(EGearType _gearType);

private:

	USoundCue* FindSoundCue(const TCHAR* _path);

};
