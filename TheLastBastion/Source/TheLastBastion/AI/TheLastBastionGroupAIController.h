// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TheLastBastionGroupAIController.generated.h"

/**
 * 
 */
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
		targetLocation_KeyID;

public:

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UBehaviorTreeComponent* GetBTComp() const { return mBTComp; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_TargetActor() const { return targetActor_KeyID; }

};
