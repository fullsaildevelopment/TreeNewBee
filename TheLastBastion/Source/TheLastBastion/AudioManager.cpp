// Fill out your copyright notice in the Description page of Project Settings.

#include "AudioManager.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "ConstructorHelpers.h"
#include "CustomType.h"
#include "Combat/Gear.h"



USoundCue* UAudioManager::CrossbowFire = nullptr;
USoundCue* UAudioManager::CrossbowReload = nullptr;
USoundCue* UAudioManager::BoltsHitImpactOnCharacter = nullptr;
USoundCue* UAudioManager::BoltsHitImpactOnLightShield = nullptr;
USoundCue* UAudioManager::BoltsHitImpactOnHeavyShield = nullptr;
USoundCue* UAudioManager::BoltsHitImpactOnTerrain = nullptr;
USoundCue* UAudioManager::LightWeaponImpact = nullptr;
USoundCue* UAudioManager::HeavyWeaponImpact = nullptr;
USoundCue* UAudioManager::ShieldBashImpact = nullptr;
USoundCue* UAudioManager::MeleeCounterAttackImpact = nullptr;
USoundCue* UAudioManager::MeleeWeaponSwing = nullptr;
USoundCue* UAudioManager::WeaponEquip = nullptr;
USoundCue* UAudioManager::WeaponDraw = nullptr;
USoundCue* UAudioManager::LightArmorFootStepOnDirt = nullptr;
USoundCue* UAudioManager::Jump = nullptr;
USoundCue* UAudioManager::Roll = nullptr;
USoundCue* UAudioManager::PlayerVoiceOnGroupSelection = nullptr;
USoundCue* UAudioManager::PlayerVocalCommandsOnGroup = nullptr;
USoundCue* UAudioManager::FoodWarning = nullptr;
USoundCue* UAudioManager::MineWarning = nullptr;
USoundCue* UAudioManager::StoneWarning = nullptr;
USoundCue* UAudioManager::WoodWarning = nullptr;
USoundCue* UAudioManager::SoldierDialog = nullptr;
USoundCue* UAudioManager::DefaultTheme = nullptr;
USoundCue* UAudioManager::LannisterTheme1 = nullptr;
USoundCue* UAudioManager::LannisterTheme2 = nullptr;
USoundCue* UAudioManager::WhiteWalkerTheme = nullptr;
USoundCue* UAudioManager::PlayerLevelUp = nullptr;
USoundCue* UAudioManager::PlayerCastHealing = nullptr;

UAudioManager::UAudioManager()
{   
	// Find all the sound cues of our game
	//Range Weapon
	CrossbowFire = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/CrossBow/CrossBow_Fire"));
	CrossbowReload = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/CrossBow/Crossbow_Reload"));
	BoltsHitImpactOnCharacter = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/BoltsHitImpact/BoltsHitImpactOnCharacter"));
	BoltsHitImpactOnLightShield = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/BoltsHitImpact/BoltsHitImpactOnLightShield"));
	BoltsHitImpactOnHeavyShield = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/BoltsHitImpact/BoltsHitImpactOnHeavyShield"));
	BoltsHitImpactOnTerrain = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/BoltsHitImpact/BoltsHitImpactOnTerrain"));

	// Melee Weapon Impact
	LightWeaponImpact = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/MeleeWeaponHitImpact/LightWeaponImpact"));
	HeavyWeaponImpact = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/MeleeWeaponHitImpact/HeavyWeaponImpact"));
	ShieldBashImpact = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/MeleeWeaponHitImpact/ShieldBashImpact"));
	MeleeCounterAttackImpact = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/MeleeWeaponHitImpact/MeleeCounterAttackImpact"));

	// Melee Weapon Swing
	MeleeWeaponSwing = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/MeleeWeaponSwing/MeleeWeaponSwing"));

	// Weapon Equip and Draw
	WeaponEquip = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/GearEquipAndDraw/GearEquip"));
	WeaponDraw = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/GearEquipAndDraw/GearDraw"));

	// FootStep
	LightArmorFootStepOnDirt = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/FootStep/LightArmorFootStepOnDirt"));

	// Jump and Roll
	Jump = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/RollAndJump/Jump"));
	Roll = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/RollAndJump/Roll"));

	// Player Vocal
	PlayerVoiceOnGroupSelection = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Vocal/PlayerGroupSelection"));
	PlayerVocalCommandsOnGroup = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Vocal/PlayerGroupCommands"));

	// Vocal Notification
	FoodWarning = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Vocal/FoodWarning"));
	MineWarning = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Vocal/MineWarning"));
	StoneWarning = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Vocal/StoneWarning"));
	WoodWarning = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Vocal/WoodWarning"));

	// BGM
	SoldierDialog= FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Vocal/SoldiersDialog"));
	DefaultTheme = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Music/DefaultTheme"));
	LannisterTheme1 = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Music/LannisterTheme1"));
	LannisterTheme2 = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Music/LannisterTheme2"));
	WhiteWalkerTheme = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Music/WhiteWalkerTheme"));

	//Player ability
	PlayerLevelUp = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Player/PlayerLevelUp"));
	PlayerCastHealing = FindSoundCue(TEXT("/Game/Assets/Audio/AudioCues/Player/PlayerCastHealing"));
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

	case ESoundEffectType::ECrossbowReload:
		return CrossbowReload;

	case ESoundEffectType::EBoltsHitImpactOnCharacter:
		return BoltsHitImpactOnCharacter;

	case ESoundEffectType::EBoltsHitImpactOnLightShield:
		return BoltsHitImpactOnLightShield;

	case ESoundEffectType::EBoltsHitImpactOnHeavyShield:
		return BoltsHitImpactOnHeavyShield;

	case ESoundEffectType::EBoltsHitImpactOnTerrain:
		return BoltsHitImpactOnTerrain;

	case ESoundEffectType::ELightWeaponImpact:
		return LightWeaponImpact;

	case ESoundEffectType::EHeavyWeaponImpact:
		return HeavyWeaponImpact;

	case ESoundEffectType::EShieldBashImpact:
		return ShieldBashImpact;

	case ESoundEffectType::EMeleeCounterAttackImpact:
		return MeleeCounterAttackImpact;

	case ESoundEffectType::EMeleeWeaponSwing:
		return MeleeWeaponSwing;

	case ESoundEffectType::EWeaponEquip:
		return WeaponEquip;

	case ESoundEffectType::EWeaponDraw:
		return WeaponDraw;

	case ESoundEffectType::ELightArmorFootStepOnDirt:
		return LightArmorFootStepOnDirt;

	case ESoundEffectType::EJump:
		return Jump;

	case ESoundEffectType::ERoll:
		return Roll;

	case ESoundEffectType::EPlayerVoiceOnGroupSelection:
		return PlayerVoiceOnGroupSelection;

	case ESoundEffectType::EPlayerVocalCommandsOnGroup:
		return PlayerVocalCommandsOnGroup;

	case ESoundEffectType::EFoodWarning:
		return FoodWarning;

	case ESoundEffectType::EMineWarning:
		return MineWarning;

	case ESoundEffectType::EStoneWarning:
		return StoneWarning;

	case ESoundEffectType::EWoodWarning:
		return WoodWarning;

	case ESoundEffectType::ESoldierDialog:
		return SoldierDialog;

	case ESoundEffectType::EDefaultTheme:
		return DefaultTheme;

	case ESoundEffectType::ELannisterTheme1:
		return LannisterTheme1;

	case ESoundEffectType::ELannisterTheme2:
		return LannisterTheme2;

	case ESoundEffectType::EWhiteWalkerTheme:
		return WhiteWalkerTheme;

	case ESoundEffectType::EPlayerLevelUp:
		return PlayerLevelUp;

	case ESoundEffectType::EPlayerCastHealing:
		return PlayerCastHealing;

	default:
		return nullptr;
	}

}

USoundCue * UAudioManager::GetProjectileImpactByMaterial(EPhysicalSurface _surfaceType)
{

	switch (_surfaceType)
	{
	case SURFACE_FLESH:
	case SURFACE_METAL:
	case SURFACE_ICE:
		return BoltsHitImpactOnCharacter;
	case SURFACE_LightShield:
		return BoltsHitImpactOnLightShield;

	case SURFACE_HeavyShield:
		return BoltsHitImpactOnHeavyShield;

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
		return ShieldBashImpact;

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
	case EGearType::DaiKatana:
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



