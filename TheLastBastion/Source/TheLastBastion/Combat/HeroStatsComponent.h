// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/PawnStatsComponent.h"
#include "HeroStatsComponent.generated.h"

/**
 * Extension of Pawn Stats to help player for combat targetting
 * and manage mini inventory, skill tree
 */
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

	UPROPERTY(Replicated)
		class ATheLastBastionHeroCharacter* mHeroCharacter;


private:

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

public:

	FORCEINLINE const class ATheLastBastionEnemyCharacter* GetCurrentTarget() const { return mCurrentTarget; }
	FORCEINLINE int GetMaxNumOfWeaponSlot() const override { return 4; }


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
