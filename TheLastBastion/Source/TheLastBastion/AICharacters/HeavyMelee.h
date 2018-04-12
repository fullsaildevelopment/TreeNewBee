// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "HeavyMelee.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API AHeavyMelee : public ATheLastBastionEnemyCharacter
{
	GENERATED_BODY()
	
public:

	bool OnParry(const struct FDamageInfo* const _damageInfo, const class UPawnStatsComponent* const _damageCauserPawnStats) override;

protected:

	UFUNCTION(BlueprintNativeEvent)
		bool IsQueensGuard() const;
	bool IsQueensGuard_Implementation() const { return false; }

	FName GetParrySectionName(const struct FDamageInfo* const _damageInfo) const;

	bool ShouldPlayHitAnimation() const override;
	
};
