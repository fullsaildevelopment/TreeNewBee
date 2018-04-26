// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/PawnStatsComponent.h"
#include "HeroStatsComponent.generated.h"

/**
 * Extension of Pawn Stats to help player for combat targetting
 * and manage mini inventory, skill tree
 */
enum class EEquipType : uint8;

#define HeroHpRecoverRate_Init 0.1f           // percentage
#define HeroHpRecoverDelay_Init 10.0f         // seconds
#define HeroSpRecoverRate_Init 0.115f         // percentage
#define HeroDpGainOn_CounterAttack_Init 7.5f  // percentage
#define HeroDpGainOn_ShieldBash_Init 10.0f	  // percentage
#define HeroDpGainOn_HVPowerHit_Init 5.0f	  // percentage

#define HeroMeleeAttackSpCost_Init_Hammer_BattleAxe -30.0f
#define HeroMeleeAttackSpCost_Init_GreatSword -25.0f
#define HeroMeleeAttackSpCost_Init_Katana -10.0f
#define HeroMeleeAttackSpCost_Init_SnAxeMace -15.0f
#define HeroMeleeAttackSpCost_Init_SnSword -13.0f

#define HeroCounterAttackSpCost_Init_Hammer -90.0f
#define HeroCounterAttackSpCost_Init_BattleAxe_GreatSword  -55.0f
#define HeroCounterAttackSpCost_Init_Katana_Sns -20.0f

#define HereDodgeSpCost_Init_HammerBattleAxe -50.0f 
#define HereDodgeSpCost_Init_GreatSword -35.0f
#define HereDodgeSpCost_Init_Katana     -20.0f
#define HereDodgeSpCost_Init_Sns        -25.0f

#define SpConsumeRateDeductionOnEachLevel 0.05f
#define HpRegenDelayDeductionOnEachLevel 0.1f
#define DpGainIncrementOnEachLevel 0.1f
#define DpGain_Increment_OnEachLevel 1.0f

#define HereDefenceSpCost_Init_HammerBattleAxe -0.8f 
#define HereDefenceSpCost_Init_GreatSword      -0.6f 
#define HereDefenceSpCost_Init_Katana          -0.5f
#define HereDefenceSpCost_Init_Sns             -0.2f 


#define SurvivalTrainingLevel 0 
#define StaminaTraingingLevel 1
#define FarmerLevel 2
#define BuilderLevel 3
#define MinerLevel 4
#define SawyerLevel 5
#define HitThemHardLevel 6
#define MakeThemSufferLevel 7
#define FaithLevel 8
#define LeaderLevel 9
#define SkillNum 10


#define SpCost_PowerHit      -60
#define SpCost_Heal          -80

#define SpCost_Combo_HV      -40
#define SpCost_Combo_Katana  -20
#define SpCost_Combo_Sns     -25


#define DamageMultiplier_Combo 1.5f
#define DamageMultiplier_PowerHit_Sns 1
#define DamageMultiplier_PowerHit_Katana 3
#define DamageMultiplier_PowerHit_HV 2

#define DamageMultiplier_increment_onEachLevel_Combo 0.1f

#define  Skill_Heal_Radius_Init 500
#define  Skill_Heal_Radius_Increment_OnLevelUp 50
#define  Skill_Heal_Amount_Init 20 // percentage
#define  Skill_Heal_Amount_Increment_OnLevelUp 4

#define StaminaRecoverRateDuringFaith 0.5f
#define StaminaRecoverRateDuringFaith_Increment_OnLevelUp 0.05f;


UCLASS()
class THELASTBASTION_API UHeroStatsComponent : public UPawnStatsComponent
{
	GENERATED_BODY()
		
public:
	// Sets default values for this component's properties
	UHeroStatsComponent();

protected:

	virtual void BeginPlay() override;


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gear)
		int LastMeleeWeapon_Index;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		int SkillPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** Contains the skill level for each skill in skill menu*/
		TArray<int> SkillsSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** Scaler to the time delay of health recover after being hit in seconds*/
		float HeroHpRecoverDelay_Scaler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** Scaler to the percentage of hero hp recoverd at each frame */
		float HeroHpRecoverRate_Scaler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** Scaler the percentage of hero sp recoverd at each frame*/
		float HeroSpRecoverRate_Scaler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** Scaler the percentage of hero sp recoverd at each frame with enchanted weapon*/
		float HeroSpRecoverRateDuringFaith_Scaler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** Scaler the percentage of hero sp consumed by action(melee, skill, dodge, defence) at each frame*/
		float HeroSpConsumeRate_Scaler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** Scaler the percentage of hero dp gain by counter attack*/
		float Hero_DpGain_Scaler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** Scaler the percentage of hero dp gain by ShieldBash*/
		float Hero_DpGain_OnShieldBash;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** Scaler the percentage of hero dp gain by HV power hit*/
		float Hero_DpGain_OnHVPowerHit;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		float Hero_Combo_DamageMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		float Hero_PowerHit_DamageMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		float Hero_PowerHit_DamageMultiplier_Sns;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		float Hero_PowerHit_DamageMultiplier_Katana;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		float Hero_PowerHit_DamageMultiplier_HV;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** current hero sp consumed by melee attack*/
		float Hero_MeleeAttack_SpCost;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** current hero sp consumed by Counter attack*/
		float Hero_CounterAttack_SpCost;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** current hero sp consumed by dodge*/
		float Hero_Dodge_SpCost;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** current hero sp consumed by defence mode*/
		float Hero_Defence_SpCost;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		float HealAmount_Skill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		float HealRadius_Skill;

private:

	UPROPERTY()
		class ATheLastBastionHeroCharacter* mHeroCharacter;
	UPROPERTY()
	    class ATheLastBastionEnemyCharacter* mCurrentTarget;
	UPROPERTY()
	    class ATheLastBastionEnemyCharacter* mNextThreat;
	UPROPERTY()
		class USphereComponent*    TargetDetector;
	UPROPERTY()
	TArray<class ATheLastBastionEnemyCharacter*> mPotentialTargets;

public:

	// Called when Melee player enter Focus mode
	void OnFocus();

	bool OnSwapBetweenMeleeAndRange() override;

	bool OnSwitchWeapon(EEquipType _nextEquip);

	/** Called when player update their gears, replace the gear with players'choice and update stats*/
	void OnTradeMenuAccept(class UInventoryUI* _inventoryMenu);

	/** Called when player apply the equip hide settings */
	void UpdateEquipHideOption();

	bool ApplyDamage(const FDamageInfo& _hit) override;

	void LevelUp() override;

public:

	FORCEINLINE const class ATheLastBastionEnemyCharacter* GetCurrentTarget() const { return mCurrentTarget; }

	/** Get Hero Sp Recover Rate as a unit value*/
	FORCEINLINE float GetHeroHpRecoverDelay() const { return HeroHpRecoverDelay_Init * HeroHpRecoverDelay_Scaler; }
	/** return Hero Hp Recover percentage as a unit value*/
	FORCEINLINE float GetHeroHpRecoverRate() const { return HeroHpRecoverRate_Init * HeroHpRecoverRate_Scaler; }
	/** Get Hero Sp Recover by percentage as a unit value*/
	FORCEINLINE float GetHeroSpRecoverRate() const { return HeroSpRecoverRate_Init * HeroSpRecoverRate_Scaler; }

	/** Get Melee Attack sp cost */
	FORCEINLINE float GetSpCost_MeleeAttack() const { return Hero_MeleeAttack_SpCost * HeroSpConsumeRate_Scaler; }
	/** Get Counter Attack sp cost*/
	FORCEINLINE float GetSpCost_CounterAttack() const { return Hero_CounterAttack_SpCost * HeroSpConsumeRate_Scaler; }
	/** Get Dodge sp cost*/
	FORCEINLINE float GetSpCost_Dodge() const { return Hero_Dodge_SpCost * HeroSpConsumeRate_Scaler; }
	/** Get Defence sp cost*/
	FORCEINLINE float GetSpCost_Defence() const { return Hero_Defence_SpCost * HeroSpConsumeRate_Scaler * 100.0f; }

	/** Get Dp Gain By percentage on successful counter attack*/
	FORCEINLINE float GetDpGainPercentage_CounterAttack() const { return HeroDpGainOn_CounterAttack_Init * Hero_DpGain_Scaler * 0.01f; }
	/** Get Dp Gain By percentage on successful Shield bash*/
	FORCEINLINE float GetDpGainPercentage_ShieldBash() const { return Hero_DpGain_OnShieldBash * Hero_DpGain_Scaler * 0.01f; }
	/** Get Dp Gain By percentage on successful HV powerHit*/
	FORCEINLINE float GetDpGainPercentage_HVPowerHit() const { return Hero_DpGain_OnHVPowerHit * Hero_DpGain_Scaler * 0.01f; }




	/** A Unit value to scale the sp consume rate*/
	FORCEINLINE float GetHeroSpConsumeRate_Scaler() const { return HeroSpConsumeRate_Scaler; }
	FORCEINLINE float GetHeroHpRecoverDelay_Scaler() const { return HeroHpRecoverDelay_Scaler; }
	FORCEINLINE float GetDpGain_Scaler() const { return Hero_DpGain_Scaler; }
	FORCEINLINE float GetDpGain_OnShieldBash() const { return Hero_DpGain_OnShieldBash; }
	FORCEINLINE float GetDpGain_OnHVPowerHit() const { return Hero_DpGain_OnHVPowerHit; }
	FORCEINLINE float GetPowerHit_DamageMultiplier() const { return Hero_PowerHit_DamageMultiplier; }
	FORCEINLINE float GetCombo_DamageMultiplier() const { return Hero_Combo_DamageMultiplier; }
	FORCEINLINE float GetHealRadius() const { return HealRadius_Skill; }
	FORCEINLINE float GetHealAmount() const { return HealAmount_Skill * 0.01f; }
	

	FORCEINLINE void SetHeroSpConsumeRateByLevel_Scaler(int _level) { HeroSpConsumeRate_Scaler = 1 - SpConsumeRateDeductionOnEachLevel * _level; }
	FORCEINLINE void SetHeroHpRecoverDelayByLevel_Scaler(int _level) { HeroHpRecoverDelay_Scaler = 1 - HpRegenDelayDeductionOnEachLevel * _level; }
	FORCEINLINE void SetDpGain_Scaler(int _level) {  Hero_DpGain_Scaler = 1 + DpGainIncrementOnEachLevel * _level ; }
	FORCEINLINE void SetDpGainByLevel_OnShieldBash(int _level) { Hero_DpGain_OnShieldBash = HeroDpGainOn_ShieldBash_Init + DpGain_Increment_OnEachLevel * _level; }
	FORCEINLINE void SetDpGainByLevel_OnHVPowerHit(int _level) { Hero_DpGain_OnHVPowerHit = HeroDpGainOn_HVPowerHit_Init + DpGain_Increment_OnEachLevel * _level; }
	FORCEINLINE void SetDamageMultiplierByLevel_Combo(int _level) { Hero_Combo_DamageMultiplier = DamageMultiplier_Combo + DamageMultiplier_increment_onEachLevel_Combo * _level; }
	FORCEINLINE void SetDamageMultiplierByLevel_PowerHit_Sns(int _level) { Hero_Combo_DamageMultiplier = DamageMultiplier_PowerHit_Sns + DamageMultiplier_increment_onEachLevel_Combo * _level; }
	FORCEINLINE void SetDamageMultiplierByLevel_PowerHit_Katana(int _level) { Hero_Combo_DamageMultiplier = DamageMultiplier_PowerHit_Katana + DamageMultiplier_increment_onEachLevel_Combo * _level; }
	FORCEINLINE void SetDamageMultiplierByLevel_PowerHit_HV(int _level) { Hero_Combo_DamageMultiplier = DamageMultiplier_PowerHit_HV + DamageMultiplier_increment_onEachLevel_Combo * _level; }
	FORCEINLINE void SetHealRadiusByLevel(int _level) { HealRadius_Skill = Skill_Heal_Radius_Init + _level * Skill_Heal_Radius_Increment_OnLevelUp; }
	FORCEINLINE void SetHealAmountByLevel(int _level) { HealAmount_Skill = Skill_Heal_Amount_Init + _level * Skill_Heal_Amount_Increment_OnLevelUp; }
	FORCEINLINE void SetSpRecoverRateDuringFaith(int _level) { HeroSpRecoverRateDuringFaith_Scaler = StaminaRecoverRateDuringFaith + _level * StaminaRecoverRateDuringFaith_Increment_OnLevelUp; }

	FORCEINLINE int GetSkillPoints() const { return SkillPoints; }
	FORCEINLINE void SetSkillPoints(int _val) { SkillPoints = _val; }
	FORCEINLINE int GetSkillLevelAt(int _index) { return SkillsSet[_index]; }
	FORCEINLINE void SetSkillLevelAt(int _index, int _val) {SkillsSet[_index] = _val; }

	FORCEINLINE void EnableFaith(bool _val) { HeroSpRecoverRate_Scaler = (_val) ? HeroSpRecoverRateDuringFaith_Scaler : 1; }
	/** Called on Switch Weapon, 
	* update Sp Cost
	* update Skill animation section name*/
	void UpdateOnWeaponChange(EGearType _gearType);


private:
	
	/** Called when an enemy enter the detection sphere*/
	UFUNCTION()
	void OnEnemyEnter(UPrimitiveComponent * _overlappedComponent, 
		AActor * _otherActor, UPrimitiveComponent * _otherComp, int32 _otherBodyIndex,
		bool _bFromSweep, const FHitResult & _SweepResult);
	
	UFUNCTION()
	void OnEnemyLeave(UPrimitiveComponent * _overlappedComponent, 
		AActor * _otherActor, UPrimitiveComponent * _otherComp, int32 _otherBodyIndex);

	void OnEnemyLeaveMelee(AActor * _otherActor);

	void MeleeFocus();
};
