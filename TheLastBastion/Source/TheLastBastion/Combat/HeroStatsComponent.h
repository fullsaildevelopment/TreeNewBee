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

	class ATheLastBastionHeroCharacter* mHeroCharacter;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Targeting)
	    class USphereComponent*    MeleeTargetDetector;

private:

	const class ATheLastBastionEnemyCharacter* mCurrentTarget;
	const class ATheLastBastionEnemyCharacter* mNextThreat;

	TArray<const class ATheLastBastionEnemyCharacter*> mPotentialTargets;

public:
	// Called when player press Focus button
	void OnFocus();

public:
	FORCEINLINE const class ATheLastBastionEnemyCharacter* GetCurrentTarget() const { return mCurrentTarget; }


private:

	UFUNCTION()
		virtual void OnEnemyEnter(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor
			, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex,
			bool _bFromSweep, const FHitResult& _SweepResult);

	UFUNCTION()
		virtual void OnEnemyLeave(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor
			, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex);

	void MeleeFocus();
	
};
