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

}

// Called when the game starts or when spawned
void AEnemyGroupSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	//GetWorldTimerManager().SetTimer(SpawnTimer, this, &AEnemyGroupSpawner::Spawn, SpawnFrequency, true, 5.0f);
	
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

		//AEnemyGroup* newEnemyGroup = 
		//	GetWorld()->SpawnActor()
	}
}

