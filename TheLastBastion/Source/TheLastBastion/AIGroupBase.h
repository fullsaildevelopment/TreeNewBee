// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIGroupBase.generated.h"

USTRUCT(BlueprintType)
struct FAISpawnInfo 
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
	TSubclassOf<class ATheLastBastionCharacter> AIClassBP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** the number of this ai class in group*/
	int32 TotalNumber;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** the number of row this ai class occupied in group*/
	int32 NumOfRow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** the distance between each ai in same row */
	float ColumnPadding = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** the distance between each ai in same column */
	float RowPadding = 150.0f;
};


USTRUCT(BlueprintType)
struct FAICharacterInfo
{
	GENERATED_BODY()

	UPROPERTY()
		class ATheLastBastionCharacter* AICharacter;

	UPROPERTY()
		FVector GroupRelativeLocation;

};

UCLASS(BlueprintType)
class THELASTBASTION_API AAIGroupBase : public ACharacter
{
	GENERATED_BODY()


	UPROPERTY()
	TArray<FAICharacterInfo> AICharactersInfo;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** The class and the number we about to spawn*/
		TArray<FAISpawnInfo> AIToSpawn;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//	class USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		/** Trigger volumn to present the group size and trigger group combat*/
		class UBoxComponent* GroupVolumn;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Behavior)
		class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
	bool bActivated;


public:
	// Sets default values for this pawn's properties
	AAIGroupBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//
	void SpawnAGroup();
	
	//
	void SetGroupSpawingLocations();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	/** Update the children location during move to*/
	UFUNCTION(BlueprintCallable, Category = GroupBehavior)
	void SetChildPathLocation();
	
};
