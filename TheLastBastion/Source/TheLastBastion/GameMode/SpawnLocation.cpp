// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnLocation.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASpawnLocation::ASpawnLocation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComp;

	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxComp->bGenerateOverlapEvents = false;
	bIsWorkingLocation = true;

}

// Called when the game starts or when spawned
void ASpawnLocation::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector ASpawnLocation::GetRandomSpawnPoint() const
{
	return UKismetMathLibrary::RandomPointInBoundingBox(BoxComp->Bounds.Origin,
		BoxComp->Bounds.BoxExtent);
}


