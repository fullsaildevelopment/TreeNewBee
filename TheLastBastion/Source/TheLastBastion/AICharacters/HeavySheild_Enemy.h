// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "HeavySheild_Enemy.generated.h"

/**
 * 
 */




enum class EAIActionState : uint8;

UCLASS()
class THELASTBASTION_API AHeavySheild_Enemy : public ATheLastBastionEnemyCharacter
{
	GENERATED_BODY()
	
	
public:

	bool OnCounterAttack(const struct FDamageInfo* const _damageInfo,
		const class UPawnStatsComponent* const _damageCauserPawnStats) override;

	bool OnParry(const struct FDamageInfo* const _damageInfo,
		const class UPawnStatsComponent* const _damageCauserPawnStats) override;

	int GetMeleeComboSel(bool _bIsMoving) const override;

	FORCEINLINE int GetComboCounter() const override { return CharacterType == ECharacterType::LanTrooper_Shield ? FMath::RandRange(Sns_Combo_Counter_Min, Sns_Combo_Counter_Max) : FMath::RandRange(Sns_Ulti_Combo_Counter_Min, Sns_Ulti_Combo_Counter_Max); }

	/** Get How many melee attack this AI can parry before getting hit, 
	for Ulti guardian, this is once being hit how many melee attack it can takes before parrying attack*/
	FORCEINLINE int GetParryEndurance() const override { return FMath::RandRange(Sns_Parry_End_Min, Sns_Parry_End_Max); }
	FORCEINLINE int GetCounterEndurance() const override {return  FMath::RandRange(Sns_Ulti_Counter_Min, Sns_Ulti_Counter_Max);}

	void ClearEndurance() override;

protected:


	int GetParrySectionNameIndex(bool _damageByHeavyWeapon, const struct FDamageInfo* const _damageInfo) const;

	int GetParrySectionNameIndex_Easy(const struct FDamageInfo* const _damageInfo) const;

	int GetParrySectionNameIndex_Weak(const struct FDamageInfo* const _damageInfo) const;

	FORCEINLINE FName GetParrySectionName(int _index) const { return SnsParryAndBlockSectionName[_index]; }

	FName GetCounterAttackSectionName(const struct FDamageInfo* const _damageInfo) const;

	void UpdateEnduranceOnBeingHit(const AActor* const _damageCauser) override;

private:

	int GetMeleeComboSel_Guardian(bool _bIsMoving) const;
	int GetMeleeComboSel_UltiGuardian(bool _bIsMoving) const;

	/** Called when parry is done successfully*/
	void OnParrySuccess(const class UPawnStatsComponent* const _damageCauserPawnStats);
	/** Check if parry is available for current animation state, according to damage causer's current weapon, before the directional check*/
	bool IsParrySuccess(bool _damageByHeavyWeapon, EAIActionState _currentActionState) const;
	bool IsParrySuccess_Ulti(bool _damageByHeavyWeapon, EAIActionState _currentActionState) const;

};
