// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "DaiKatana_Enemy.generated.h"

/**
 * 
 */

#define NK_InPlace_Left_Min 0 
#define NK_InPlace_Left_Max 3 
#define NK_InPlace_Right_Min 4 
#define NK_InPlace_Right_Max 8 
#define NK_Move_Left_Min 9 
#define NK_Move_Left_Max 12
#define NK_Move_Right_Min 13 
#define NK_Move_Right_Max 17

#define NK_Combo_Counter 5
#define NK_Counter_End 3



UCLASS()
class THELASTBASTION_API ADaiKatana_Enemy : public ATheLastBastionEnemyCharacter
{
	GENERATED_BODY()
	
	
public:

	bool OnAutoDodge(const struct FDamageInfo* const _damageInfo,
		const class UPawnStatsComponent* const _damageCauserPawnStats) override;

	bool OnCounterAttack(const struct FDamageInfo* const _damageInfo,
		const class UPawnStatsComponent* const _damageCauserPawnStats) override;

	bool OnParry(const struct FDamageInfo* const _damageInfo,
		const class UPawnStatsComponent* const _damageCauserPawnStats) override;

	int GetMeleeComboSel(bool _bIsMoving) const override;

	FORCEINLINE int GetComboCounter() const override { return NK_Combo_Counter; } 

	FORCEINLINE int GetCounterEndurance() const override { return  NK_Counter_End; }

	void ClearEndurance() override;

protected:



	//int GetParrySectionNameIndex_Easy(const struct FDamageInfo* const _damageInfo) const;

	//int GetParrySectionNameIndex_Weak(const struct FDamageInfo* const _damageInfo) const;

	//FORCEINLINE FName GetParrySectionName(int _index) const { return SnsParryAndBlockSectionName[_index]; }

	//FName GetCounterAttackSectionName(const struct FDamageInfo* const _damageInfo) const;

private:

	FName GetParrySectionName(bool _damageByHeavyWeapon, const struct FDamageInfo* const _damageInfo) const;

	FName GetDodgeSectionName(const struct FDamageInfo* const _damageInfo) const;


	//int GetMeleeComboSel_Guardian(bool _bIsMoving) const;
	//int GetMeleeComboSel_UltiGuardian(bool _bIsMoving) const;

	///** Called when parry is done successfully*/
	//void OnParrySuccess(const class UPawnStatsComponent* const _damageCauserPawnStats);
	///** Check if parry is available for current animation state, according to damage causer's current weapon, before the directional check*/
	//bool IsParrySuccess(bool _damageByHeavyWeapon, EAIActionState _currentActionState) const;
	//bool IsParrySuccess_Ulti(bool _damageByHeavyWeapon, EAIActionState _currentActionState) const;

	
};
