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
	
}

// Called every frame
void AEnemyGroupSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

		FRotator spawnRotation;
		FVector  spawnLocation;

		GetSpawnLocationAndRotation(spawnLocation, spawnRotation);

		FTransform spawnTransform;
		spawnTransform.SetLocation(spawnLocation);
		spawnTransform.SetRotation(spawnRotation.Quaternion());
		spawnTransform.SetScale3D(FVector(1, 1, 1));
		
		// Spawn new group
	/*	AEnemyGroup* newEnemyGroup = GetWorld()
			->SpawnActor<AEnemyGroup>(LanT0, spawnLocation, spawnRotation, spawnParam);
		newEnemyGroup->SpawnDefaultController();*/

		// spawn new group
		AEnemyGroup* newEnemyGroup
			= GetWorld()->SpawnActorDeferred<AEnemyGroup>
			(LanT0, spawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		FAISpawnInfo spawnInfo;
		newEnemyGroup->SetSpawnInfoAtSection_TotalNum (FMath::RandRange(8, 12), 0);
		newEnemyGroup->SetSpawnInfoAtSection_MaxCol(FMath::RandRange(4, 6), 0);

		UGameplayStatics::FinishSpawningActor(newEnemyGroup, spawnTransform);
		newEnemyGroup->SpawnDefaultController();

		gm->RegisterEnemyGroup(newEnemyGroup);
	}
}

void AEnemyGroupSpawner::GetSpawnLocationAndRotation(FVector& _location, FRotator& _rotation) const
{
	int spawnSelection = FMath::RandRange(0, SpawnPoints.Num() - 1);
	_location =  SpawnPoints[spawnSelection];
	FVector toHero = Hero->GetActorLocation() - _location;
	_rotation = toHero.Rotation();
	_rotation.Pitch = 0;
}

