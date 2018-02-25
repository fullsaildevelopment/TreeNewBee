// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyGroupSpawner.generated.h"

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
		TArray<FVector> SpawnPoints;

	FTimerHandle SpawnTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Spawning)
	float SpawnFrequency;


private:

	/// Enemy Group Preset
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SpanwPreset, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class AEnemyGroup> LanT0;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:

	void FindAllEnemyGroupPreset();

	void Spawn();

	
	
};
