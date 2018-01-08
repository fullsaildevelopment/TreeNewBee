// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnVolumn.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ASpawnVolumn::ASpawnVolumn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	mSpawnVolumn = CreateDefaultSubobject<UBoxComponent>(TEXT("SizeOfSpawnVolumn"));
	SetRootComponent(mSpawnVolumn);

	MinimumSpawnDelay = 10.0f;
	MaximumSpawnDelay = 20.0f;


}

// Called when the game starts or when spawned
void ASpawnVolumn::BeginPlay()
{
	Super::BeginPlay();

	spawnDelay = FMath::RandRange(MinimumSpawnDelay, MaximumSpawnDelay);

	GetWorldTimerManager().SetTimer(mSpawnTimer, this, &ASpawnVolumn::Spawn, spawnDelay, true, 1.0f);

}

// Called every frame
void ASpawnVolumn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnVolumn::Spawn()
{
	if (TrooperTier0 != nullptr)
	{
		UWorld* const world = GetWorld();
		if (world)
		{
			FVector spawnLocation = GetRandomPointVolumn();
			spawnLocation.Z = 0;
			world->SpawnActor<ATheLastBastionCharacter>(TrooperTier0, spawnLocation, FRotator::ZeroRotator);
		}

	}
}

FVector ASpawnVolumn::GetRandomPointVolumn()
{
	return UKismetMathLibrary::RandomPointInBoundingBox(mSpawnVolumn->Bounds.Origin,
		mSpawnVolumn->Bounds.BoxExtent);
}

