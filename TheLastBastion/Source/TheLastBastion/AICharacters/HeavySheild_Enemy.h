// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "HeavySheild_Enemy.generated.h"

/**
 * 
 */

#define Parry_Up_Right 0 
#define Parry_Up_Left 1 
#define Parry_Top_Left 2 
#define Block_Top_Down_Right 3 
#define Block_Top_Up_Mid 4 
#define Block_Down 5 

const FName SnsParryAndBlockSectionName[6] = 
{ FName("Parry_Up_Right"),FName("Parry_Up_Left"), FName("Parry_Top_Left"), 
FName("Block_Top_Down_Right"),FName("Block_Top_Up_Mid") ,FName("Block_Down") };


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

	int GetParrySectionNameIndex(const struct FDamageInfo* const _damageInfo) const;

	FORCEINLINE FName GetParrySectionName(int _index) const { return SnsParryAndBlockSectionName[_index]; }

	FName GetCounterAttackSectionName(const struct FDamageInfo* const _damageInfo) const;

	void UpdateEnduranceOnBeingHit(const AActor* const _damageCauser) override;

private:

	int GetMeleeComboSel_Guardian(bool _bIsMoving) const;
	int GetMeleeComboSel_UltiGuardian(bool _bIsMoving) const;


	bool IsParrySuccess(const class UPawnStatsComponent* const _damageCauserPawnStats, EAIActionState _currentActionState) const;
	bool IsParrySuccess_Ulti(const class UPawnStatsComponent* const _damageCauserPawnStats, EAIActionState _currentActionState) const;

};
