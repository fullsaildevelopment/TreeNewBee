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

}

// Called when the game starts or when spawned
void AEnemyGroupSpawner::BeginPlay()
{
	Super::BeginPlay();
	
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

// Called every frame
void AEnemyGroupSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FTransform AEnemyGroupSpawner::GetNextWayPointFrom(int _pathIndex, int _nextWaypoint) const
{
	return Paths[_pathIndex].WayPoints[_nextWaypoint];
}

void AEnemyGroupSpawner::FindAllEnemyGroupPreset()
{
	if (LanT0 == nullptr)
		UCustomType::FindClass<AEnemyGroup>(LanT0,
			TEXT("/Game/Blueprints/AI/GroupPreset/EnemyGroupPreset/LanT0_Bp"));

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

	if (gm->HasRoomNewEnemyGroup())
	{
		/// just gonna spawn Lan T0 for now
		FActorSpawnParameters spawnParam;
		spawnParam.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		FQuat spawnRotation;
		FVector spawnLocation;
		int pathIndex = 0;
		SelectedPath(spawnLocation, spawnRotation, pathIndex);

		FTransform spawnTransform;
		spawnTransform.SetLocation(spawnLocation);
		spawnTransform.SetRotation(spawnRotation);
		spawnTransform.SetScale3D(FVector(1, 1, 1));
		
		// spawn new group
		AEnemyGroup* newEnemyGroup
			= GetWorld()->SpawnActorDeferred<AEnemyGroup>
			(LanT0, spawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (newEnemyGroup)
		{
			FAISpawnInfo spawnInfo;
			newEnemyGroup->SetSpawnInfoAtSection_TotalNum(FMath::RandRange(8, 12), 0);
			newEnemyGroup->SetSpawnInfoAtSection_MaxCol(FMath::RandRange(4, 6), 0);
			newEnemyGroup->SetPath(pathIndex);

			UGameplayStatics::FinishSpawningActor(newEnemyGroup, spawnTransform);
			newEnemyGroup->SpawnDefaultController();

			gm->RegisterEnemyGroup(newEnemyGroup);

			//FVector forward = newEnemyGroup->GetActorForwardVector();
			//UE_LOG(LogTemp, Log, TEXT(" forward: %f, %f, %f -- AEnemyGroupSpawner::Spawn"), 
			//	forward.X, forward.Y, forward.Z );
		}
	}
}

void AEnemyGroupSpawner::SelectedPath(FVector& _location, FQuat& _rotation, int& _pathIndex) const
{
	_pathIndex = FMath::RandRange(0, Paths.Num() - 1);
	_location =  this->GetActorLocation() + Paths[_pathIndex].WayPoints[0].GetLocation();

	_rotation = Paths[_pathIndex].WayPoints[0].GetRotation();
	
	//FVector forward = Paths[spawnSelection].WayPoints[0].GetUnitAxis(EAxis::Type::X);
	//UE_LOG(LogTemp, Log, TEXT("forward: %f, %f, %f -- AEnemyGroupSpawner::SelectedPath"), 
	//	forward.X, forward.Y, forward.Z );
}

