// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Barracks.generated.h"

UCLASS(BlueprintType)
class THELASTBASTION_API ABarracks : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABarracks();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MakeEditWidget))
		TArray<FVector> SpawnPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UBoxComponent* InteractBox;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE FVector GetSpawnLocationOffsetAt(int _index) const { return SpawnPoints[_index]; }
	
};
