// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGroupBase.h"
#include "AllyGroup.generated.h"

/**
 * 
 */

#define GroupFormation_ScatterPadding_Square 500.0f
#define GroupFormation_CompactPadding_Square 250.0f

#define GroupFormation_ScatterPadding_Row  400.0f
#define GroupFormation_CompactPadding_Row  150.0f
#define MaxGroupSize 20

UCLASS()
class THELASTBASTION_API AAllyGroup : public AAIGroupBase
{
	GENERATED_BODY()

public:
	AAllyGroup();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		bool bUseSquareFormation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		bool bUseScatterFormation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		// size of array means the number of row, each entry is the number of col in current row
	TArray<int> FormationInfo;

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	void SpawnChildGroup() override;
	

	UFUNCTION()
		void OnGroupVolumnOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	UFUNCTION()
		void OnGroupVolumnOverrlapEnd(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:


	void RedistributeBy(int _cenMeters);

	void SwitchToScatter();

	void SwitchToCompact();

	void SwitchToSquare();

	void SwitchToRow();

	TSubclassOf<class ATheLastBastionAIBase> GetClassDuringAllySpawn(int _currentIndex) const;

public:

	UFUNCTION()
		void SetMarchLocation(const FVector& _location, int _commandIndex) override;

};
