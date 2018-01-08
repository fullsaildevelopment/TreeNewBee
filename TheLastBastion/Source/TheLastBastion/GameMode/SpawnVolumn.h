// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TheLastBastionCharacter.h"

#include "SpawnVolumn.generated.h"

UCLASS()
class THELASTBASTION_API ASpawnVolumn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolumn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



protected:

	FTimerHandle mSpawnTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning, meta = (AllowPrivateAccess = "true"))
		float MinimumSpawnDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning, meta = (AllowPrivateAccess = "true"))
		float MaximumSpawnDelay;


private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* mSpawnVolumn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Lannester, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class ATheLastBastionCharacter> TrooperTier0;

	float spawnDelay;

public:

	FORCEINLINE class UBoxComponent* GetSpawnVolumn() const { return mSpawnVolumn; }

	void Spawn();

private:

	UFUNCTION(BlueprintPure)
		FVector GetRandomPointVolumn();
	
};
