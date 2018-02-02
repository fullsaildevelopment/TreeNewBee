// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TheLastBastionGroupAIController.generated.h"

/**
 * 
 */

#define GC_GOTOLOCATION 1
#define GC_HOLDLOCATION 2
#define GC_FOLLOW 3
#define GC_REFORM 4
#define GC_DISTRIBUTE 5
#define GC_FORWARD 6
#define GC_BACKWARD 7

UCLASS()
class THELASTBASTION_API ATheLastBastionGroupAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ATheLastBastionGroupAIController(const FObjectInitializer& objectInitializer);
	void Possess(APawn* _possPawn) override;
	void BeginPlay() override;

private:

	UPROPERTY(transient)
		class UBehaviorTreeComponent* mBTComp;

	UPROPERTY(transient)
		class UBlackboardComponent* mBBComp;

	UPROPERTY()
		class AAIGroupBase* mGroupPawn;

	uint8
		targetActor_KeyID,
		targetLocation_KeyID,
		targetForward_KeyID,
		targetRight_KeyID,
		newCommandIndex_KeyID,
		oldCommandIndex_KeyID;

public:

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UBehaviorTreeComponent* GetBTComp() const { return mBTComp; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_TargetActor() const { return targetActor_KeyID; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_TargetLocation() const { return targetLocation_KeyID; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_NewCommandIndex() const { return newCommandIndex_KeyID; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_OldCommandIndex() const { return oldCommandIndex_KeyID; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_TargetForward() const { return targetForward_KeyID; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_TargetRight() const { return targetRight_KeyID; }


};
