// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnLocation.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ESpawnWorldLocation : uint8
{
	Lannester_S1 = 0 	    UMETA(DisplayName = "LannesterSpawnPoint 1"),
	Lannester_S2 	UMETA(DisplayName = "LannesterSpawnPoint 2"),
	Lannester_S3	UMETA(DisplayName = "LannesterSpawnPoint 3"),
	WhiteWalker_S1        UMETA(DisplayName = "WhiteWalkerSpawnPoint 1")
};

UCLASS()
class THELASTBASTION_API ASpawnLocation : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
		class UBoxComponent* BoxComp;
	
public:	
	// Sets default values for this actor's properties
	ASpawnLocation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LocationType")
		ESpawnWorldLocation LocationType;

public:
	UFUNCTION(BlueprintPure)
	FVector GetRandomSpawnPoint() const;

	UFUNCTION(BlueprintPure)
		FORCEINLINE ESpawnWorldLocation GetSpawnLocationType() const { return LocationType; }


};
