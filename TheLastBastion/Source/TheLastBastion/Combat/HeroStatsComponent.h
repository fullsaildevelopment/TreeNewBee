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

#define HeroHpRecoverRate_Init 0.1f   // percentage
#define HeroHpRecoverDelay_Init 5.0f  // seconds
#define HeroSpRecoverRate_Init 0.1f   // percentage
#define HeroDpGainOnCA_Init 10.0f	  // percentage

#define HeroMeleeAttackSpCost_Init_Hammer_BattleAxe -30.0f;
#define HeroMeleeAttackSpCost_Init_GreatSword -25.0f;
#define HeroMeleeAttackSpCost_Init_Katana -10.0f;
#define HeroMeleeAttackSpCost_Init_SnAxeMace -20.0f;
#define HeroMeleeAttackSpCost_Init_SnSword -15.0f;

#define HeroCounterAttackSpCost_Init_Hammer -90.0f;
#define HeroCounterAttackSpCost_Init_BattleAxe_GreatSword  -55.0f;
#define HeroCounterAttackSpCost_Init_Katana_Sns -20.0f;

#define HereDodgeSpCost_Init_HammerBattleAxe -50.0f 
#define HereDodgeSpCost_Init_GreatSword -35.0f
#define HereDodgeSpCost_Init_Katana     -20.0f
#define HereDodgeSpCost_Init_Sns        -25.0f

#define HereDefenceSpCost_Init_HammerBattleAxe -0.8f 
#define HereDefenceSpCost_Init_GreatSword      -0.6f 
#define HereDefenceSpCost_Init_Katana          -0.5f
#define HereDefenceSpCost_Init_Sns             -0.2f 


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
		/** Scaler to the time delay of health recover after being hit in seconds*/
		float HeroHpRecoverDelay_Scaler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** Scaler to the percentage of hero hp recoverd at each frame */
		float HeroHpRecoverRate_Scaler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** Scaler the percentage of hero sp recoverd at each frame*/
		float HeroSpRecoverRate_Scaler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** Scaler the percentage of hero sp consumed by action(melee, skill, dodge, defence) at each frame*/
		float HeroSpConsumeRate_Scaler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterStats)
		/** Scaler the percentage of hero dp gain by counter attack*/
		float Hero_DpGainOnCA_Scaler;

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

public:

	FORCEINLINE const class ATheLastBastionEnemyCharacter* GetCurrentTarget() const { return mCurrentTarget; }

	/** Get Hero Sp Recover Rate as a unit value*/
	FORCEINLINE float GetHeroHpRecoverDelay() const { return HeroHpRecoverDelay_Init * HeroHpRecoverDelay_Scaler; }
	/** return Hero Hp Recover percentage as a unit value*/
	FORCEINLINE float GetHeroHpRecoverRate() const { return HeroHpRecoverRate_Init * HeroHpRecoverRate_Scaler* 0.01f; }
	/** Get Hero Sp Recover by percentage as a unit value*/
	FORCEINLINE float GetHeroSpRecoverRate() const { return HeroSpRecoverRate_Init * HeroSpRecoverRate_Scaler * 0.01f; }

	/** Get Melee Attack sp cost */
	FORCEINLINE float GetSpCost_MeleeAttack() const { return Hero_MeleeAttack_SpCost * HeroSpConsumeRate_Scaler; }
	/** Get Counter Attack sp cost*/
	FORCEINLINE float GetSpCost_CounterAttack() const { return Hero_CounterAttack_SpCost * HeroSpConsumeRate_Scaler; }
	/** Get Dodge sp cost*/
	FORCEINLINE float GetSpCost_Dodge() const { return Hero_Dodge_SpCost * HeroSpConsumeRate_Scaler; }
	/** Get Defence sp cost*/
	FORCEINLINE float GetSpCost_Defence() const { return Hero_Defence_SpCost * HeroSpConsumeRate_Scaler; }
	/** Get Dp Gain By percentage on successful counter attack*/
	FORCEINLINE float GetDpGainPercentage_CounterAttack() const { return HeroDpGainOnCA_Init * Hero_DpGainOnCA_Scaler * 0.01f; }


	/** Called on Switch Weapon*/
	void UpdateSpCostOnWeaponChange(EGearType _gearType);


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
