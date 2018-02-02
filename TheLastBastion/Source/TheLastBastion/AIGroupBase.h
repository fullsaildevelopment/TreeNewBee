// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
//#include "GameFramework/Character.h"
#include "AIGroupBase.generated.h"


#define SIDEPADDING 200.0f
#define GroupFormation_ScatterPadding_Square 500.0f
#define GroupFormation_CompactPadding_Square 250.0f

#define GroupFormation_ScatterPadding_Row  400.0f
#define GroupFormation_CompactPadding_Row  150.0f


USTRUCT(BlueprintType)
struct FAISpawnInfo 
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
	TSubclassOf<class ATheLastBastionAIBase> AIClassBP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** the number of this ai class in group*/
	int32 TotalNumber;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** the number of row this ai class occupied in group*/
	int32 NumOfRow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** the distance between each ai in same row */
	float ColumnPadding = GroupFormation_CompactPadding_Square;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** the distance between each ai in same column */
	float RowPadding = GroupFormation_CompactPadding_Square;
};

USTRUCT(BlueprintType)
struct FAICharacterInfo
{
	GENERATED_BODY()

	UPROPERTY()
		class ATheLastBastionAIBase* AICharacter;

	UPROPERTY()
		FVector GroupRelativeOffset;

	UPROPERTY()
		/** the row - column index of the this character of same class*/
		FVector2D GroupIndexOffset;
};


UCLASS(BlueprintType)
class THELASTBASTION_API AAIGroupBase : public APawn
{
	GENERATED_BODY()


	UPROPERTY()
	TArray<FAICharacterInfo> AICharactersInfo;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** The class and the number we about to spawn*/
		TArray<FAISpawnInfo> AIToSpawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		/** Trigger volumn to present the group size and trigger group combat*/
		class UBoxComponent* GroupVolumn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		/** Trigger volumn to present the group size and trigger group combat*/
		class UFloatingPawnMovement* MoveComp;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Behavior)
		class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		bool bActivated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		bool bUseSquareFormation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		bool bUseScatterFormation;

public:
	// Sets default values for this pawn's properties
	AAIGroupBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//
	void SpawnAGroup();
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	FORCEINLINE FVector GetGroupRelativeOffsetAt(int _index) const { return AICharactersInfo[_index].GroupRelativeOffset; }

	/** Update the children location during move to*/
	UFUNCTION(BlueprintCallable, Category = GroupBehavior)
	void SetChildPathLocation();

	UFUNCTION()
		void SetMarchLocation(const FVector& _location, int _commandIndex);


private:
	
	// Call when group go to the opposite direction
	void SwapChildenOrder();

	void SwitchToScatter();

	void SwitchToCompact();
	
};
