// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillMenu_PopUp.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Combat/HeroStatsComponent.h"
#include "TheLastBastionHeroCharacter.h"
#include "CustomType.h"
#include "UI/Gameplay/InGameMenu.h"
#include "Environment/Castle.h"

namespace SkillMenu_PopUp_Strings
{
	const FString Names[10] = {
		TEXT("Survival Training"),
		TEXT("Stamina Trainging"),
		TEXT("Farmer"),
		TEXT("Builder"),
		TEXT("Miner"),
		TEXT("Sawyer"),
		TEXT("Hit Them Hard"),
		TEXT("Make Them Suffer"),
		TEXT("Faith"),
		TEXT("Leader")
	};
	const FString Descriptions[10] =
	{
		TEXT("Reduce health regeneration delay, gain more defence point on successful counter attack and relative skills"),
		TEXT("Reduce stamina consumption on melee, dodge, couter attack and all skills"),
		TEXT("Gain more food at each increasement"),
		TEXT("Gain more stone at each increasement"),
		TEXT("Gain more metal at each increasement"),
		TEXT("Gain more wood at each increasement"),
		TEXT("Reduce cool down time and increase damage for Combo melee skills and Combo Shots"),
		TEXT("Reduce cool down time and increase damage for PowerHit Skills, gain large amount defence point point if using single hand weapon, multiple damage if using katana, gain fair amount of defence point and multiple damage if using heavy weapon, add extra bullets amount and reduce bullet spread if using crossbow"),
		TEXT("Reduce stamina recovery punishment when Flaming sword is activated"),
		TEXT("Heal yourself and allies, increase radius and heal amount, increase castle defence")
	};
};


bool USkillMenu_PopUp::Initialize()
{
	if (Super::Initialize() == false)
	{
		return false;
	}

	bool AllWidgetGood = BonusList != nullptr &&
		SkillName != nullptr && Description != nullptr;


	return AllWidgetGood;
}

void USkillMenu_PopUp::SetUpSkillPopUp(int _index, const UInGameMenu * const _inGameMenu)
{
	SkillName->SetText(FText::FromString(SkillMenu_PopUp_Strings::Names[_index]));
	Description->SetText(FText::FromString(SkillMenu_PopUp_Strings::Descriptions[_index]));

	switch (_index)
	{

	case SurvivalTrainingLevel:
		AddEntryFor_SurvivalTraining(_inGameMenu);
		break;
	case StaminaTraingingLevel:
		AddEntryFor_StaminaTrainging(_inGameMenu);
		break;
	case FarmerLevel:
		AddEntryFor_FarmerLevel(_inGameMenu);
		break;

	case BuilderLevel:
		AddEntryFor_Builder(_inGameMenu);
		break;

	case MinerLevel:
		AddEntryFor_Miner(_inGameMenu);
		break;

	case SawyerLevel:
		AddEntryFor_Sawyer(_inGameMenu);
		break;

	case HitThemHardLevel:
		AddEntryFor_HitThemHard(_inGameMenu);
		break;

	case MakeThemSufferLevel:
		AddEntryFor_MakeThemSuffer(_inGameMenu);
		break;

	case FaithLevel:
		AddEntryFor_Faith(_inGameMenu);
		break;

	case LeaderLevel:
		AddEntryFor_Leader(_inGameMenu);
		break;

	default:
		break;
	}

}

void USkillMenu_PopUp::AddEntryFor_SurvivalTraining(const UInGameMenu * const _inGameMenu)
{

	int skillLevel = _inGameMenu->GetSkillLevelAt(SurvivalTrainingLevel);
	float val = (1 - HpRegenDelayDeductionOnEachLevel * skillLevel) * 100;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("Hp Regen Delay(%): "), val, BonusList);

	val = DpGainIncrementOnEachLevel * skillLevel * 100;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("Dp extra gain(%): "), val, BonusList);
}

void USkillMenu_PopUp::AddEntryFor_StaminaTrainging(const UInGameMenu * const _inGameMenu)
{
	int skillLevel = _inGameMenu->GetSkillLevelAt(StaminaTraingingLevel);
	float val = (1 - SpConsumeRateDeductionOnEachLevel * skillLevel) * 100;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("Sp Consumption(%): "), val, BonusList);

}

void USkillMenu_PopUp::AddEntryFor_FarmerLevel(const UInGameMenu * const _inGameMenu)
{
	int skillLevel = _inGameMenu->GetSkillLevelAt(FarmerLevel);

	float val = ResourceSkillAddAmountOnEachLevel* skillLevel * 100;
	AddPopUpEntryWithColor(FoodUI_Color, TEXT("Extra Food(%): "), val, BonusList);


}

void USkillMenu_PopUp::AddEntryFor_Builder(const UInGameMenu * const _inGameMenu)
{
	int skillLevel = _inGameMenu->GetSkillLevelAt(BuilderLevel);

	float val = ResourceSkillAddAmountOnEachLevel * skillLevel * 100;
	AddPopUpEntryWithColor(StoneUI_Color, TEXT("Extra Stone(%): "), val, BonusList);

}

void USkillMenu_PopUp::AddEntryFor_Miner(const UInGameMenu * const _inGameMenu)
{
	int skillLevel = _inGameMenu->GetSkillLevelAt(MinerLevel);

	float val = ResourceSkillAddAmountOnEachLevel * skillLevel * 100;
	AddPopUpEntryWithColor(MetalUI_Color, TEXT("Extra Metal(%): "), val, BonusList);

}

void USkillMenu_PopUp::AddEntryFor_Sawyer(const UInGameMenu * const _inGameMenu)
{
	int skillLevel = _inGameMenu->GetSkillLevelAt(SawyerLevel);

	float val = ResourceSkillAddAmountOnEachLevel * skillLevel * 100;
	AddPopUpEntryWithColor(WoodUI_Color, TEXT("Extra Wood(%): "), val, BonusList);

}

void USkillMenu_PopUp::AddEntryFor_HitThemHard(const UInGameMenu * const _inGameMenu)
{
	int skillLevel = _inGameMenu->GetSkillLevelAt(HitThemHardLevel);

	float val = Skill_Combo_CD - Skill_LevelUp_CD_Deduction * skillLevel;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("Cool Down(sec): "), val, BonusList);

	val = (DamageMultiplier_Combo + DamageMultiplier_increment_onEachLevel_Combo * skillLevel) * 100;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("Melee Damage(%): "), val, BonusList);

	val = (DamageMultiplier_ComboShoot + DamageMultiplier_ComboShoot_Increment_onEachLevel * skillLevel) * 100;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("CrossBow Damage(%): "), val, BonusList);

}

void USkillMenu_PopUp::AddEntryFor_MakeThemSuffer(const UInGameMenu * const _inGameMenu)
{
	int skillLevel = _inGameMenu->GetSkillLevelAt(MakeThemSufferLevel);

	float val = Skill_PowerHit_CD - Skill_LevelUp_CD_Deduction * skillLevel;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("Cool Down(sec): "), val, BonusList);

	val = (DamageMultiplier_PowerHit_Sns + DamageMultiplier_increment_onEachLevel_Combo * skillLevel) * 100;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("SingleHand Damage(%): "), val, BonusList);

	val = (DamageMultiplier_PowerHit_Katana + DamageMultiplier_increment_onEachLevel_Katana * skillLevel) * 100;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("Katana Damage(%): "), val, BonusList);

	val = (DamageMultiplier_PowerHit_HV + DamageMultiplier_increment_onEachLevel_Combo * skillLevel) * 100;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("Heavy Damage(%): "), val, BonusList);

	val = (PowerShot_BulletsSpreadImprove_onEachLevel* skillLevel) * 0.1f;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("Spread Reduce(%): "), val, BonusList);

	val = 0.5f * (PowerShot_ExtraProjectile_Amount_Init + skillLevel * PowerShot_AddProjectile_Amount);
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("Additional Bullets: "), val, BonusList);

}

void USkillMenu_PopUp::AddEntryFor_Faith(const UInGameMenu * const _inGameMenu)
{
	int skillLevel = _inGameMenu->GetSkillLevelAt(FaithLevel);

	float val = (StaminaRecoverRateDuringFaith + StaminaRecoverRateDuringFaith_Increment_OnLevelUp * skillLevel) * 100;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("Sp Reg. Reduce(%): "), val, BonusList);

}

void USkillMenu_PopUp::AddEntryFor_Leader(const UInGameMenu * const _inGameMenu)
{
	int skillLevel = _inGameMenu->GetSkillLevelAt(LeaderLevel);

	float val = Skill_Heal_CD - Skill_LevelUp_CD_Deduction * skillLevel;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("Cool Down(sec): "), val, BonusList);

	val = (Skill_Heal_Radius_Init + skillLevel * Skill_Heal_Radius_Increment_OnLevelUp) * 0.01f;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("Heal Radius(m): "), val, BonusList);

	val = Skill_Heal_Amount_Init + skillLevel * Skill_Heal_Amount_Increment_OnLevelUp;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("Heal Amount(%): "), val, BonusList);

	val = Castle_Hp_Init + skillLevel * Castle_Hp_Increasement;
	AddPopUpEntryWithColor(FLinearColor::Green, TEXT("Castle Defence: "), val, BonusList);

}
