// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyGroupSpawner.h"
#include "CustomType.h"
#include "AI/EnemyGroup.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/SinglePlayerGM.h"

// Sets default values
AEnemyGroupSpawner::AEnemyGroupSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	FindAllEnemyGroupPreset();


	SpawnFrequency = 10.0f;
	FirstSpawnDelay = 5.0f;
	TestGroupAmount = 1;
	TestGroupSize = 2;
	TestGroup = LanT0_CB;

}

// Called when the game starts or when spawned
void AEnemyGroupSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	EditWaves();
	InitWaveSpawner();

	Hero = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	GetWorldTimerManager().SetTimer(SpawnTimer, this, 
		&AEnemyGroupSpawner::Spawn, SpawnFrequency, true, FirstSpawnDelay);

	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("gm is null - ABarracks::BeginPlay"));
		return;
	}

	gm->RegisterEnemySpawner(this);




}

void AEnemyGroupSpawner::EditWaves()
{
	AllWaves.SetNum(1);
	// # Wave 1:
	AllWaves[0].WaveUnits.SetNum(10);
	AllWaves[0].WaveUnits[0].SetWaveUnit(LanT0_CB, South_ShooterRoute_0);
	AllWaves[0].WaveUnits[1].SetWaveUnit(LanT0, South_TrooperToute_0);
	AllWaves[0].WaveUnits[2].SetWaveUnit(LanT0, South_TrooperToute_0);
	AllWaves[0].WaveUnits[3].SetWaveUnit(LanT0_CB, South_ShooterRoute_0);
	AllWaves[0].WaveUnits[4].SetWaveUnit(LanT0, South_TrooperToute_0);
	AllWaves[0].WaveUnits[5].SetWaveUnit(LanT0, South_TrooperToute_0);
	AllWaves[0].WaveUnits[6].SetWaveUnit(LanT0_CB, South_ShooterRoute_0);
	AllWaves[0].WaveUnits[7].SetWaveUnit(LanT0, South_TrooperToute_0);
	AllWaves[0].WaveUnits[8].SetWaveUnit(LanT0, South_TrooperToute_0);
	AllWaves[0].WaveUnits[9].SetWaveUnit(LanT0_CB, South_ShooterRoute_0);
}

// Called every frame
void AEnemyGroupSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FTransform AEnemyGroupSpawner::GetNextWayPointFrom(int _pathIndex, int _nextWaypoint) const
{
	return Paths[_pathIndex].WayPoints[_nextWaypoint];
}

void AEnemyGroupSpawner::InitWaveSpawner()
{
	CurrentWaveIndex = 0;

	InitCurrentWave();
}

void AEnemyGroupSpawner::InitCurrentWave()
{
	// if we are given a unvalid wave index, just restart from first wave
	if (AllWaves.IsValidIndex(CurrentWaveIndex) == false)
		CurrentWaveIndex = 0;

	MaxWaveUnitAmount = AllWaves[CurrentWaveIndex].WaveUnits.Num();
	CurrentWaveUnitIndex = 0;

}

//void AEnemyGroupSpawner::OnWaveBegin(int _waveIndex)
//{
//	// if we are given a unvalid wave index, just restart from first wave
//}
//
//void AEnemyGroupSpawner::OnWaveEnd(int _waveIndex)
//{
//}

void AEnemyGroupSpawner::OnSpawnFinished()
{
	CurrentWaveUnitIndex++;

	// if the current wave is finished spawning
	if (CurrentWaveUnitIndex >= MaxWaveUnitAmount)
	{
		CurrentWaveIndex++;
		// spawn next wave
		InitCurrentWave();
	}

}

void AEnemyGroupSpawner::FindAllEnemyGroupPreset()
{
	UCustomType::FindClass<AEnemyGroup>(LanT0,
		TEXT("/Game/Blueprints/AI/GroupPreset/EnemyGroupPreset/LanT0_Bp"));

	UCustomType::FindClass<AEnemyGroup>(LanT0_CB,
		TEXT("/Game/Blueprints/AI/GroupPreset/EnemyGroupPreset/LanT0_CB_Bp"));


}

void AEnemyGroupSpawner::Spawn()
{
	if (!bEnableSpawning)
		return;

	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>
		(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm == nullptr)
	{
		UE_LOG(LogTemp, Error,
			TEXT("gm = nullptr - AEnemyGroupSpawner::Spawn"));
		return;
	}

	bool HasRoomToSpawn = (bTestingMode) ? 
		gm->GetEnemyGroupAmount() < TestGroupAmount : gm->HasRoomNewEnemyGroup();

	if (HasRoomToSpawn)
	{
		FActorSpawnParameters spawnParam;
		spawnParam.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		/** Get What to spawn, and path */		

		TSubclassOf<AEnemyGroup> classToSpawn; 
		int pathIndex;

		if (bTestingMode)
		{
			classToSpawn = TestGroup;
			pathIndex = 0;
		}
		else
		{
			classToSpawn = AllWaves[CurrentWaveIndex].WaveUnits[CurrentWaveUnitIndex].GroupClass_Bp;
			pathIndex = AllWaves[CurrentWaveIndex].WaveUnits[CurrentWaveUnitIndex].Path;

		}


		/** Spawn Group*/
		FQuat spawnRotation;
		FVector spawnLocation;
		GetSpawnTransform(spawnLocation, spawnRotation, pathIndex);

		FTransform spawnTransform;
		spawnTransform.SetLocation(spawnLocation);
		spawnTransform.SetRotation(spawnRotation);
		spawnTransform.SetScale3D(FVector(1, 1, 1));
		
		// spawn new group
		AEnemyGroup* newEnemyGroup
			= GetWorld()->SpawnActorDeferred<AEnemyGroup>
			(classToSpawn, spawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (newEnemyGroup)
		{
			FAISpawnInfo spawnInfo;
			if (bTestingMode)
			{
				newEnemyGroup->SetSpawnInfoAtSection_TotalNum(TestGroupSize, 0);
				newEnemyGroup->SetSpawnInfoAtSection_MaxCol(TestGroupSize, 0);
				newEnemyGroup->SetPath(pathIndex);
			}
			else
			{
				newEnemyGroup->SetSpawnInfoAtSection_TotalNum(FMath::RandRange(8, 12), 0);
				newEnemyGroup->SetSpawnInfoAtSection_MaxCol(FMath::RandRange(4, 6), 0);
				newEnemyGroup->SetPath(pathIndex);
			}

			UGameplayStatics::FinishSpawningActor(newEnemyGroup, spawnTransform);
			newEnemyGroup->SpawnDefaultController();

			gm->RegisterEnemyGroup(newEnemyGroup);

			if (bTestingMode == false)
				OnSpawnFinished();
		}

	}
}

void AEnemyGroupSpawner::GetSpawnTransform(FVector& _location, FQuat& _rotation, int _pathIndex) const
{
	//_pathIndex = 0;// FMath::RandRange(0, Paths.Num() - 1);
	_location =  this->GetActorLocation() + Paths[_pathIndex].WayPoints[0].GetLocation();

	_rotation = Paths[_pathIndex].WayPoints[0].GetRotation();
	
	//FVector forward = Paths[spawnSelection].WayPoints[0].GetUnitAxis(EAxis::Type::X);
	//UE_LOG(LogTemp, Log, TEXT("forward: %f, %f, %f -- AEnemyGroupSpawner::SelectedPath"), 
	//	forward.X, forward.Y, forward.Z );
}

