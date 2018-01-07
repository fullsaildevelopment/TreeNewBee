// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TheLastBastionBaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API ATheLastBastionBaseAIController : public AAIController
{
	GENERATED_BODY()
	

public:
	 ATheLastBastionBaseAIController(const FObjectInitializer& objectInitializer);
	 void Possess(APawn* _possPawn) override;
	 void BeginPlay() override;

private:

	/** Reference to our enemy blueprint */
	UPROPERTY()
	     class ATheLastBastionEnemyCharacter* mCharacter;

	UPROPERTY()
		class UAIBase_AnimInstance* mAnimInstanceRef;

	UPROPERTY(transient)
		class UBehaviorTreeComponent* mBTComp;

	UPROPERTY(transient)
		class UBlackboardComponent* mBBComp;

	uint8
		targetActor_KeyID,
		ToTargetActorDistanceSqr_KeyId;


public:

	UFUNCTION(BlueprintPure)
		FORCEINLINE ATheLastBastionEnemyCharacter* GetBaseEnemyCharacter() const { return mCharacter; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UAIBase_AnimInstance* GetAnimInstance() const { return mAnimInstanceRef; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UBehaviorTreeComponent* GetBTComp() const { return mBTComp; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_TargetActor() const { return targetActor_KeyID; }
	
	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_ToTargetActorDistanceSqr() const { return ToTargetActorDistanceSqr_KeyId; }

};
