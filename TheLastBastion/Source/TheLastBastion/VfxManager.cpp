// Fill out your copyright notice in the Description page of Project Settings.

#include "VfxManager.h"
#include "ConstructorHelpers.h"
#include "CustomType.h"


UParticleSystem* UVfxManager::bloodImpact_sputtering_PS = nullptr;
UParticleSystem* UVfxManager::metalImpact_sputtering_PS = nullptr;

UVfxManager::UVfxManager()
{
	bloodImpact_sputtering_PS = FindParticalSystem(TEXT("/Game/Assets/Partical/P_blood_sputtering"));
	metalImpact_sputtering_PS = FindParticalSystem(TEXT("/Game/Assets/Partical/P_metalImpact_sputtering"));
}

UParticleSystem * UVfxManager::FindParticalSystem(const TCHAR * _path)
{
	ConstructorHelpers::FObjectFinder<UParticleSystem> ps_finder(_path);
	if (ps_finder.Succeeded())
		return ps_finder.Object;
	else 
		return nullptr;
}

UParticleSystem * UVfxManager::GetVfx(EVfxType _vfxType)
{

	switch (_vfxType)
	{
	case EVfxType::bloodImpact_sputtering:
		return bloodImpact_sputtering_PS;

	case EVfxType::metalImpact_sputtering:
		return metalImpact_sputtering_PS;
	default:
		return nullptr;
	}

}

UParticleSystem * UVfxManager::GetVfxBySurfaceType(EPhysicalSurface _surfaceType)
{

	switch (_surfaceType)
	{
	case SURFACE_FLESH:
		return bloodImpact_sputtering_PS;
	case SURFACE_LightShield:
	case SURFACE_METAL:
	case SURFACE_HeavyShield:
		return metalImpact_sputtering_PS;
	default:
		return nullptr;
	}

}
