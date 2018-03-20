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
#define GC_REFORM 4     // Square or Row
#define GC_DISTRIBUTE 5 // Split up or centered
#define GC_FORWARD 6
#define GC_BACKWARD 7

#define GC_FIGHT 8



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
		class AEnemyGroup* mGroupPawn;

	uint8
		targetActor_KeyID,
		targetLocation_KeyID,
		bIsCharging_KeyID,
		bIsMoving_KeyID,
		bNewWayPoint_KeyID,
		bAtDestination_KeyID;
public:

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UBehaviorTreeComponent* GetBTComp() const { return mBTComp; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_TargetActor() const { return targetActor_KeyID; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_TargetLocation() const { return targetLocation_KeyID; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_IsMoving() const { return bIsMoving_KeyID; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_AtDestination() const { return bAtDestination_KeyID; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_IsCharging() const { return bIsCharging_KeyID; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_NewWayPoint() const { return bNewWayPoint_KeyID; }


	void SetTargetLocation_BBC(const FVector& _targetLocation);

	void SetIsMoving_BBC(bool _val);

	void SetAtDestination_BBC(bool _val);

	void SetIsCharging_BBC(bool _val);

	void SetNewWayPoint_BBC(bool _val);

	//void SetOldCommandIndex_BBC(int _oldCommand);

	FVector GetTargetLocation_BBC() const;

	bool GetIsMoving_BBC() const;

	bool GetAtDestination_BBC() const;

	bool GetIsCharging_BBC() const;

	bool GetNewWayPoint_BBC() const;

	//int GetOldCommandIndex_BBC() const;

};
