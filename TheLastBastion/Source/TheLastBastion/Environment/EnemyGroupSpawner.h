// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyGroupSpawner.generated.h"

USTRUCT(BlueprintType)
struct FMarchPath
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MakeEditWidget), Category = Path)
		/** All Unit Spawn Position*/
		TArray<FTransform> WayPoints;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MakeEditWidget), Category = Behavior)
		/** Range unit hold position*/
		TArray<FVector> ShootingPoints;


	FTimerHandle SpawnTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		float SpawnFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		float FirstSpawnDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** All Unit Spawn Position*/
		bool bEnableSpawning;



private:

	/// Enemy Group Preset
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SpawnPreset, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class AEnemyGroup> LanT0;

	UPROPERTY()
	/** Single Player Character ref*/
		const class ACharacter* Hero;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FTransform GetNextWayPointFrom(int _pathIndex, int _nextWaypoint) const;
	FORCEINLINE bool HasNextWayPointOnPath
	(int _pathIndex, int _wayPointIndex) const { return Paths[_pathIndex].WayPoints.IsValidIndex(_wayPointIndex); }

private:

	void FindAllEnemyGroupPreset();

	void Spawn();

	void SelectedPath(FVector& _location, FQuat& _rotation, int& _pathIndex) const;
};
