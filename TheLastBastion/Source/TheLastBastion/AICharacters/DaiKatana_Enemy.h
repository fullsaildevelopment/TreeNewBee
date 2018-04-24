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

	FORCEINLINE int GetAutoDodgeEndurance() const override { return  NK_Counter_End; }
	FORCEINLINE int GetCounterEndurance() const override { return  NK_Counter_End; }
	FORCEINLINE int GetParryEndurance() const override { return  NK_Counter_End; }

	void ClearEndurance() override;

protected:

	bool ShouldPlayHitAnimation() const override;

private:

	FName GetParrySectionName(bool _damageByHeavyWeapon, const struct FDamageInfo* const _damageInfo) const;

	FName GetDodgeSectionName(const struct FDamageInfo* const _damageInfo) const;

	
};
