// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyGroupSpawner.generated.h"

#define South_TrooperRoute_0 0
#define South_TrooperRoute_1 1
#define South_ShooterRoute_0 2
#define North_WoodOutputTakingRoute 3

USTRUCT(BlueprintType)
struct FMarchPath
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MakeEditWidget), Category = Path)
		/** All Unit Spawn Position*/
		TArray<FTransform> WayPoints;

};


USTRUCT(BlueprintType)
struct FWaveUnit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		TSubclassOf<class AEnemyGroup> GroupClass_Bp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		int Path;

	FORCEINLINE void SetWaveUnit(TSubclassOf<class AEnemyGroup> _class, int _path) { GroupClass_Bp = _class; Path = _path; }
};

USTRUCT(BlueprintType)
struct FWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		TArray<FWaveUnit> WaveUnits;

};


UCLASS()
class THELASTBASTION_API AEnemyGroupSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyGroupSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MakeEditWidget), Category = Spawning)
		/** All Availble Path*/
		TArray<FMarchPath> Paths;

	FTimerHandle SpawnTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
		TArray<FWave> AllWaves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnPreset, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class AEnemyGroup> TestGroup;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnPreset, meta = (AllowPrivateAccess = "true"))
		int TestGroupAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnPreset, meta = (AllowPrivateAccess = "true"))
		int TestGroupSize;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		float SpawnFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		float FirstSpawnDelay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
		/** which surviving wave we are currently fight against*/
		int CurrentWaveIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
		/** How many wave units we have spawned so far in current wave*/
		int CurrentWaveUnitIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
		/** How many wave units we are going to spawn in current wave*/
		int MaxWaveUnitAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** How many wave units we are going to spawn in current wave*/
		int TestingRoute;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** Spawn a group without any pathing settings*/
		bool bTestingMode;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** All Unit Spawn Position*/
		bool bEnableSpawning;





private:

	/// Enemy Group Preset
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SpawnPreset, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class AEnemyGroup> LanT0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SpawnPreset, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class AEnemyGroup> LanT0_CB;



	UPROPERTY()
	/** Single Player Character ref*/
		const class ACharacter* Hero;

	/** Hard Coded spawn waves for game cycle*/
	void EditWaves();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FTransform GetNextWayPointFrom(int _pathIndex, int _nextWaypoint) const;
	FORCEINLINE bool HasNextWayPointOnPath
	(int _pathIndex, int _wayPointIndex) const { return Paths[_pathIndex].WayPoints.IsValidIndex(_wayPointIndex); }

private:

	/** Setup indexes that manipulate the spawn process */
	void InitWaveSpawner();

	void InitCurrentWave();

	///** Called on wave begin */
	//void OnWaveBegin(int _waveIndex);

	///** Called on wave end */
	//void OnWaveEnd(int _waveIndex);

	/** Called EveryTime after successfully spawn a Group */
	void OnSpawnFinished();

	void FindAllEnemyGroupPreset();

	void Spawn();

	/** Get Spawn Location and direction by path*/
	void GetSpawnTransform(FVector& _location, FQuat& _rotation, int _pathIndex) const;
};
