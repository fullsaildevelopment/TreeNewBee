// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AudioManager.generated.h"




UENUM()
enum class ESoundEffectType : uint8
{
	ECrossBowFire = 0
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
	static class USoundCue* CrossBowFire;

public:
	static class USoundCue* GetSFX(ESoundEffectType _sfxType);

};
