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

	//bool bHideEquipOptions[4];
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gear)
	//	bool bHideSHWhenEquip;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gear)
	//	bool bHideHVWhenEquip;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gear)
	//	bool bHideTHWhenEquip;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gear)
	//	bool bHideCBWhenEquip;

public:

	// Called when Melee player enter Focus mode
	void OnFocus();

	bool OnSwapBetweenMeleeAndRange() override;

	bool OnSwitchWeapon(EEquipType _nextEquip);

	/** Called when player update their gears, replace the gear with players'choice and update stats*/
	void OnTradeMenuAccept(class UInventoryUI* _inventoryMenu);

	/** Called when player apply the equip hide settings */
	void UpdateEquipHideOption();

public:

	FORCEINLINE const class ATheLastBastionEnemyCharacter* GetCurrentTarget() const { return mCurrentTarget; }

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
