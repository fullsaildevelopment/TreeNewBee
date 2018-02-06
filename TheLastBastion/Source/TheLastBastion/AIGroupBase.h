// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
//#include "GameFramework/Character.h"
#include "AIGroupBase.generated.h"


#define SIDEPADDING 200.0f
#define GroupVolumnZ 10000.0f


#define COS22_5 0.9f
#define COS67_5 0.4f
#define COS112_5 -0.4f
#define COS157_5 -0.9f

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
		/** the max number of Col this ai class has in group*/
	int32 MaxNumOfCol = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
	int32 NumOfRow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** the distance between each ai in same row */
	float ColumnPadding = 250;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** the distance between each ai in same column */
	float RowPadding = 250;
};

USTRUCT(BlueprintType)
struct FAICharacterInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class ATheLastBastionAIBase* AICharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector GroupRelativeOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		/** the row - column index of the this character of same class*/
		FVector2D GroupIndexOffset;
};

USTRUCT(BlueprintType)
struct FThreat
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class ATheLastBastionCharacter* AICharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Threat;
};


UCLASS(BlueprintType)
class THELASTBASTION_API AAIGroupBase : public APawn
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		TArray<FAICharacterInfo> AICharactersInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		// size of array means the number of row, each entry is the number of col in current row
		TArray<int> FormationInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		// the dead group member should be remove from the character list, before execute the next command
		TArray<int> RemovePendingList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Behavior)
		TArray<FThreat> ThreatList;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		/** Trigger volumn to present the group size and trigger group combat*/
		class UArrowComponent* ArrowComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Behavior)
		class UBehaviorTree* BehaviorTree;

	UPROPERTY()
		class ATheLastBastionHeroCharacter* Hero;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		bool bDisabled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Behavior)
		bool bIsAgreesive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		bool bReformPending;


public:
	// Sets default values for this pawn's properties
	AAIGroupBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SpawnChildGroup();

	/** Called when the number of group member get changed */
	virtual void OnReform();

	// Call when group go to the opposite direction
	virtual void SwapChildenOrder();


	

	UFUNCTION()
		virtual void OnGroupVolumnOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		virtual void OnGroupVolumnOverrlapEnd(UPrimitiveComponent* OverlappedComponent, 
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PairColumn(AAIGroupBase* const _enemyGroup, int _myColumn, int _theirColumn);
	void AssignColumn(AAIGroupBase* const _targetGroup, int _myColumn, int _theirColumn);


	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	FORCEINLINE FVector GetGroupRelativeOffsetAt(int _index) const { return AICharactersInfo[_index].GroupRelativeOffset; }

	/** Update the children location during move to*/
	UFUNCTION(BlueprintCallable, Category = GroupBehavior)
	void CheckGroupCommand();

	UFUNCTION()
	virtual void SetMarchLocation(const FVector& _location, int _commandIndex);

	UFUNCTION()
		virtual void OnChildDeath(int _childIndex);

	virtual int GetMaxColoumnCount() const;


	UFUNCTION()
		TArray<class ATheLastBastionAIBase*> GetFrontLine() const;


	UFUNCTION()
		TArray<class ATheLastBastionAIBase*> GetColumnAt(int _index) const;



	//UFUNCTION()
	//	TArray<class ATheLastBastionCharacter*> GetFrontLine() const;

	//UFUNCTION()
	//	TArray<class ATheLastBastionCharacter*> GetFrontLine() const;

	//UFUNCTION()
	//	TArray<class ATheLastBastionCharacter*> GetFrontLine() const;

	//UFUNCTION()
	//	TArray<class ATheLastBastionCharacter*> GetFrontLine() const;

	//UFUNCTION()
	//	TArray<class ATheLastBastionCharacter*> GetFrontLine() const;

};
