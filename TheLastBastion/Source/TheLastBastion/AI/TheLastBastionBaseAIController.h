// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TheLastBastionBaseAIController.generated.h"

/**
 * 
 */
enum class ECharacterType : uint8;

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
	     class ATheLastBastionAIBase* mCharacter;

	UPROPERTY()
		class UAIBase_AnimInstance* mAnimInstanceRef;

	UPROPERTY(transient)
		class UBehaviorTreeComponent* mBTComp;

	UPROPERTY(transient)
		class UBlackboardComponent* mBBComp;

	uint8
		targetActor_KeyID,
		targetLocation_KeyID,
		CurrentActionState_KeyID,
		NewCommandIndex_KeyID,
		ToTargetLocationDistanceSqr_KeyId,
		ToTargetActorDistanceSqr_KeyId;

public:

	UFUNCTION(BlueprintPure)
		FORCEINLINE ATheLastBastionAIBase* GetBaseAICharacter() const { return mCharacter; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UAIBase_AnimInstance* GetAnimInstance() const { return mAnimInstanceRef; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UBehaviorTreeComponent* GetBTComp() const { return mBTComp; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_TargetActor() const { return targetActor_KeyID; }
	
	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_TargetLocation() const { return targetLocation_KeyID; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_NewCommandIndex() const { return NewCommandIndex_KeyID; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_ToTargetActorDistanceSqr() const { return ToTargetActorDistanceSqr_KeyId; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_ToTargetLocationDistanceSqr() const { return ToTargetLocationDistanceSqr_KeyId; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_CurrentActionState() const { return CurrentActionState_KeyID; }

	void OnBeingHit(ECharacterType _characterType);
};
