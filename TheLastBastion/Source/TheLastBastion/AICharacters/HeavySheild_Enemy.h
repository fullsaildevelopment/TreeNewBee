// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "HeavySheild_Enemy.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API AHeavySheild_Enemy : public ATheLastBastionEnemyCharacter
{
	GENERATED_BODY()
	
	
public:

	//bool OnCounterAttack(const struct FDamageInfo* const _damageInfo,
	//	const class UPawnStatsComponent* const _damageCauserPawnStats) override;

	bool OnParry(const struct FDamageInfo* const _damageInfo,
		const class UPawnStatsComponent* const _damageCauserPawnStats) override;

	int GetMeleeComboSel(bool _bIsMoving) const override;

	FORCEINLINE int GetComboCounter() const override { return CharacterType == ECharacterType::LanTrooper_Shield ? FMath::RandRange(Sns_Combo_Counter_Min, Sns_Combo_Counter_Max) : FMath::RandRange(Sns_Ulti_Combo_Counter_Min, Sns_Ulti_Combo_Counter_Max); }

	/** Get How many melee attack this AI can parry before getting hit*/
	FORCEINLINE int GetParryEndurance() const override { return FMath::RandRange(Sns_Parry_End_Min, Sns_Parry_End_Max); }

protected:

	FName GetParrySectionName(const struct FDamageInfo* const _damageInfo) const;

	FName GetCounterAttackSectionName(const struct FDamageInfo* const _damageInfo) const;

private:

	int GetMeleeComboSel_Guardian(bool _bIsMoving) const;
	int GetMeleeComboSel_UltiGuardian(bool _bIsMoving) const;


	bool IsParrySuccess(const class UPawnStatsComponent* const _damageCauserPawnStats) const;
	bool IsParrySuccess_Ulti(const class UPawnStatsComponent* const _damageCauserPawnStats) const;

};
