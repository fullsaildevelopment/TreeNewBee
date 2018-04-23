// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacters/TheLastBastionAllyCharacter.h"
#include "SingleSwordMan_Ally.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API ASingleSwordMan_Ally : public ATheLastBastionAllyCharacter
{
	GENERATED_BODY()
	
public:

	bool OnParry(const struct FDamageInfo* const _damageInfo,
		const class UPawnStatsComponent* const _damageCauserPawnStats) override;

	bool OnAutoDodge(const struct FDamageInfo* const _damageInfo,
		const class UPawnStatsComponent* const _damageCauserPawnStats) override;

	int GetMeleeComboSel(bool _bIsMoving) const override;

	FORCEINLINE int GetAutoDodgeEndurance() const override { return FMath::RandRange(SH_Fast_Dodge_End_Min, SH_Fast_Dodge_End_Max); }
	FORCEINLINE int GetParryEndurance() const override { return FMath::RandRange(SH_Pow_Parry_End_Min, SH_Pow_Parry_End_Max); }
	FORCEINLINE int GetComboCounter() const override { return CharacterType == ECharacterType::HillTribe_Rookie_LS ? FMath::RandRange(SH_Roo_Combo_Counter_Min, SH_Roo_Combo_Counter_Max) : FMath::RandRange(SH_PowFast_Combo_Counter_Min, SH_PowFast_Combo_Counter_Max); }

protected:

	FName GetParrySectionName(const struct FDamageInfo* const _damageInfo) const;
	FName GetDodgeSectionName(const struct FDamageInfo* const _damageInfo) const;

private:

	int GetMeleeComboSel_Rookie(bool _bIsMoving) const;
	int GetMeleeComboSel_Fast(bool _bIsMoving) const;
	int GetMeleeComboSel_Power(bool _bIsMoving) const;

	
};
