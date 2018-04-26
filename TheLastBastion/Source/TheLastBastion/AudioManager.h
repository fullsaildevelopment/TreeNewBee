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
	ECrossbowReload,
	EBoltsHitImpactOnCharacter,
	EBoltsHitImpactOnLightShield,
	EBoltsHitImpactOnHeavyShield,
	EBoltsHitImpactOnTerrain,
	ELightWeaponImpact,
	EHeavyWeaponImpact,
	EShieldBashImpact,
	EMeleeCounterAttackImpact,
	EMeleeWeaponSwing,
	EWeaponEquip,
	EWeaponDraw,
	ELightArmorFootStepOnDirt,
	EJump,
	ERoll,
	EPlayerVoiceOnGroupSelection,
	EPlayerVocalCommandsOnGroup,
	EFoodWarning,
	EMineWarning,
	EStoneWarning,
	EWoodWarning,
	ESoldierDialog,
	EDefaultTheme,
	ELannisterTheme1,
	ELannisterTheme2,
	EWhiteWalkerTheme,
	EPlayerLevelUp,
	EPlayerCastHealing
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
	static class USoundCue* CrossbowReload;
	static class USoundCue* BoltsHitImpactOnCharacter;
	static class USoundCue* BoltsHitImpactOnLightShield;
	static class USoundCue* BoltsHitImpactOnHeavyShield;
	static class USoundCue* BoltsHitImpactOnTerrain;

	// Melee Weapon Impact SFX
	static class USoundCue* LightWeaponImpact;
	static class USoundCue* HeavyWeaponImpact;
	static class USoundCue* ShieldBashImpact;
	static class USoundCue* MeleeCounterAttackImpact;

	// Melee Weapon Swing SFX
	static class USoundCue* MeleeWeaponSwing;

	// Weapon Equip
	static class USoundCue* WeaponEquip;
	static class USoundCue* WeaponDraw;

	// Foot Step
	static class USoundCue* LightArmorFootStepOnDirt;

	// Jump and Roll
	static class USoundCue* Jump;
	static class USoundCue* Roll;

	// Player Vocal
	static class USoundCue* PlayerVoiceOnGroupSelection;
	static class USoundCue* PlayerVocalCommandsOnGroup;

	// Vocal Notification
	static class USoundCue* FoodWarning;
	static class USoundCue* MineWarning;
	static class USoundCue* StoneWarning;
	static class USoundCue* WoodWarning;

	// Background music
	static class USoundCue* SoldierDialog;
	static class USoundCue* DefaultTheme;
	static class USoundCue* LannisterTheme1;
	static class USoundCue* LannisterTheme2;
	static class USoundCue* WhiteWalkerTheme;

	// Player Ability
	static class USoundCue* PlayerLevelUp;
	static class USoundCue* PlayerCastHealing;

public:

	static class USoundCue* GetSFX(ESoundEffectType _sfxType);

	static class USoundCue* GetProjectileImpactByMaterial(EPhysicalSurface _surfaceType);

	static class USoundCue* GetMeleeWeaponImpactSFXByGearType(EGearType _gearType);

private:

	USoundCue* FindSoundCue(const TCHAR* _path);

};
