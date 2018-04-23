// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "HeavyMelee_Enemy.generated.h"

/**
 * 
 */



UCLASS()
class THELASTBASTION_API AHeavyMelee_Enemy : public ATheLastBastionEnemyCharacter
{
	GENERATED_BODY()
	
public:

	bool OnParry(const struct FDamageInfo* const _damageInfo, const class UPawnStatsComponent* const _damageCauserPawnStats) override;

	int GetMeleeComboSel(bool _bIsMoving) const override;

	FORCEINLINE int GetComboCounter() const override { return CharacterType == ECharacterType::Lan_QueenGuard ? FMath::RandRange(HV_Combo_Counter_Sr_Min, HV_Combo_Counter_Sr_Max) : FMath::RandRange(HV_Combo_Counter_Jr_Min, HV_Combo_Counter_Jr_Max); }

protected:

	FName GetParrySectionName(const struct FDamageInfo* const _damageInfo) const;

	bool ShouldPlayHitAnimation() const override;
	
};
