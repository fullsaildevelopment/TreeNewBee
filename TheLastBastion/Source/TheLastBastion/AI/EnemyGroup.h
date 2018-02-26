// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGroupBase.h"
#include "EnemyGroup.generated.h"

/**
 * 
 */
#define EM_ToCastle     1
#define EM_MeleeAgainstPlayer 2



UCLASS()
class THELASTBASTION_API AEnemyGroup : public AAIGroupBase
{
	GENERATED_BODY()
	
public:
	AEnemyGroup();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Behavior)
		/** The Strategy purpose of this enemy group unit*/
		int MainTask;

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	void SpawnChildGroup() override;

	/** Called when the number of group member get changed */
	void OnReform() override;

	// Call when group go to the opposite direction
	void SwapChildenOrder() override;

public:

	UFUNCTION()
		void SetMarchLocation(const FVector& _location, int _commandIndex) override;

	/** Set the Strategy purpose of this enemy group unit*/
	FORCEINLINE void SetMainTask(int _task) { MainTask = _task; }
	/** Get the Strategy purpose of this enemy group unit*/
	int GetMainTask() const { return MainTask; }

	void OnChildDeath(int _childIndex) override;

	int GetMaxColoumnCount() const override;

	int GetMaxRowCount() const override;

	FORCEINLINE void SetSpawnInfoAtSection_TotalNum(int _totalNumber, int _sectionIndex) { AIToSpawn[_sectionIndex].TotalNumber = _totalNumber;}
	FORCEINLINE void SetSpawnInfoAtSection_MaxCol(int _maxCol, int _sectionIndex) { AIToSpawn[_sectionIndex].MaxNumOfCol = _maxCol; }


	/** AI Decision: Move to close to player and attack player by melee*/
	void MeleeAgainstPlayer_OnEnemyGroupMission();

protected:

	/** Called upon when player enter the combat trigger, while group is not in battle*/
	void MeleeGroupAgainstPlayer();
};
