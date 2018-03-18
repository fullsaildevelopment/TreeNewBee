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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Behavior)
		/** The selected path to castle */
		int PathIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Behavior)
		/** The current Waypoint of this group */
		int CurrentWayPoint;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Behavior)
		class UBehaviorTree* BehaviorTree;


protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	void SpawnChildGroup() override;

	/** Called when the number of group member get changed */
	void OnReform() override;

	// Call when group go to the opposite direction
	void SwapChildenOrder() override;

public:

	/** Get the Strategy purpose of this enemy group unit*/
	int GetMainTask() const { return MainTask; }
	int GetMaxColoumnCount() const override;
	int GetMaxRowCount() const override;
	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }


	UFUNCTION()
		void SetMarchLocation(const FVector& _location, int _commandIndex) override;

	FORCEINLINE int GetCurrentWayPoint() const { return CurrentWayPoint; }
	FORCEINLINE int GetPathIndex() const { return PathIndex; }

	UFUNCTION(BlueprintCallable)
		void GoToNextWayPoint();

	UFUNCTION(BlueprintCallable)
		void ReachWayPoint();


	UFUNCTION(BlueprintPure)
		/** Check if the distance in x-y space between 
		the front line of this group location and target location is less than radius*/
		bool IsNearTargetLocation(float radius_Sqr = 5000.0f) const;


	/** Set the Strategy purpose of this enemy group unit*/
	FORCEINLINE void SetMainTask(int _task) { MainTask = _task; }

	/** Set the marching path, it should only be called during spawn*/
	FORCEINLINE void SetPath(int _pathIndex) {PathIndex = _pathIndex; CurrentWayPoint = 0;}
	FORCEINLINE void SetSpawnInfoAtSection_TotalNum(int _totalNumber, int _sectionIndex) { AIToSpawn[_sectionIndex].TotalNumber = _totalNumber; }
	FORCEINLINE void SetSpawnInfoAtSection_MaxCol(int _maxCol, int _sectionIndex) { AIToSpawn[_sectionIndex].MaxNumOfCol = _maxCol; }

	/** AI Decision: Move to close to player and attack player by melee*/
	void MeleeAgainstPlayer_OnEnemyGroupMission();
	void OnChildDeath(int _childIndex) override;

	void SetInBattle(bool _val) override;

protected:

	/** Called upon when player enter the combat trigger, while group is not in battle*/
	void MeleeGroupAgainstPlayer();
};
