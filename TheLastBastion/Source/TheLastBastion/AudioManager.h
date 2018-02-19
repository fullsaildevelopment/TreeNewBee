// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AudioManager.generated.h"




UENUM()
enum class ESoundEffectType : uint8
{
	ECrossBowFire = 0,
	ESingeHandSwordSlash = 1,
	EBoltsFleshCharacter = 2,
	EBoltsStickToShield = 3,
	ESwordFleshImpact = 4
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

	static class USoundCue* BoltsStickToShield;

	static class USoundCue* SwordFleshImpact;

public:
	static class USoundCue* GetSFX(ESoundEffectType _sfxType);

private:
	USoundCue* FindSoundCue(const TCHAR* _path);

};
