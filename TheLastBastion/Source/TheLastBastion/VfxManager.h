// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VfxManager.generated.h"

/**
 * 
 */




UENUM()
enum class EVfxType : uint8
{
	bloodImpact_sputtering = 0,
	metalImpact_sputtering,
	WeaponNormalTrail,
	WeaponFireTrail,
	WeaponFireEnchantment,
	PlayerHealing,
	PlayerLevelUp,
	FootSteps
};

UCLASS()
class THELASTBASTION_API UVfxManager : public UObject
{
	GENERATED_BODY()
	
public:

	UVfxManager();

private:

	static class UParticleSystem* bloodImpact_sputtering_PS;

	static class UParticleSystem* metalImpact_sputtering_PS;

	static class UParticleSystem* WeaponNormalTrail;
	static class UParticleSystem* WeaponFireTrail;

	static class UParticleSystem* WeaponFireEnchantment;

	static class UParticleSystem* PlayerHealing;
	static class UParticleSystem* PlayerLevelUp;

	static class UParticleSystem* FootSteps;

	UParticleSystem* FindParticalSystem(const TCHAR* _path);

public:

	static class UParticleSystem* GetVfx(EVfxType _vfxType);

	static class UParticleSystem* GetVfxBySurfaceType(EPhysicalSurface _surfaceType);
};
