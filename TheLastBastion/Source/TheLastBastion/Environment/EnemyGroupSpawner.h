// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyGroupSpawner.generated.h"

//#define South_TrooperRoute_0 0
//#define South_TrooperRoute_1 1
//#define South_ShooterRoute_0 2
//#define North_WoodOutputTakingRoute 3


UENUM(BlueprintType)
enum class EPath : uint8 
{
	South_TrooperRoute_0 = 0   UMETA(DisplayName = "South to Castle - Trooper_0"), 
	South_TrooperRoute_1 = 1   UMETA(DisplayName = "South to Castle - Trooper_1"),
	South_ShooterRoute_0 = 2   UMETA(DisplayName = "South to Castle - Shooter"),
	East_TrooperRoute  = 3     UMETA(DisplayName = "East  to Castle - Trooper"),
	East_SouthMine  = 4        UMETA(DisplayName = "East to South Mine"),
	East_NorthMine  = 5        UMETA(DisplayName = "East to North Mine"),
	North_NorthMine = 6        UMETA(DisplayName = "North to North Mine"),
	North_TrooperRoute = 7     UMETA(DisplayName = "North to Castle - Trooper"),
	North_Wood = 8             UMETA(DisplayName = "North to Wood"),
	North_Food = 9             UMETA(DisplayName = "North to Food"),
	East_Shooter = 10          UMETA(DisplayName = "South to Castle - Shooter "),
	North_Shooter = 11         UMETA(DisplayName = "North to Castle - Shooter"),
	East_Stone = 12            UMETA(DisplayName = "East to Stone")
};

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
		EPath Path;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		float SpawnDelay = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		class USoundCue* WarningSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		int SpawnTotalNumber = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		int SpawnMaxColumn = 3;

	//FORCEINLINE void SetWaveUnit(TSubclassOf<class AEnemyGroup> _class, int _path) { GroupClass_Bp = _class; Path = _path; }
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		TArray<FWave> AllWaves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnTest, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class AEnemyGroup> TestGroup;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnTest, meta = (AllowPrivateAccess = "true"))
		int TestGroupAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnTest, meta = (AllowPrivateAccess = "true"))
		int TestGroupSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
		float SpawnDelay;

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
		EPath TestingRoute;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** Spawn a group without any pathing settings*/
		bool bTestingMode;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** All Unit Spawn Position*/
		bool bEnableSpawning;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
		bool bIsCurrentWaveFinished;

private:

	UPROPERTY()
	/** Single Player Character ref*/
		const class ACharacter* Hero;

	/** Hard Coded spawn waves for game cycle*/
	void EditWaves();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FTransform GetNextWayPointFrom(int _pathIndex, int _nextWaypoint) const;

	FORCEINLINE bool IsCurrentWaveFinished() const { return bIsCurrentWaveFinished; }

	FORCEINLINE bool HasNextWayPointOnPath
	(int _pathIndex, int _wayPointIndex) const { return Paths[_pathIndex].WayPoints.IsValidIndex(_wayPointIndex); }

	void EnableSpawning();

private:

	/** Setup indexes that manipulate the spawn process */
	void InitWaveSpawner();

	/** Reset CurrentWaveUnitIndex, If CurrentWaveIndex is out of scope, assigne the currentWaveIndex with max index*/
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
