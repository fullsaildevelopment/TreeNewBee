// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TheLastBastionBaseAIController.generated.h"

/**
 * 
 */
enum class ECharacterType : uint8;
enum class EAIActionState : uint8;

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
		bIsPaused_KeyID,
		targetActor_KeyID,
		targetLocation_KeyID,
		CurrentActionState_KeyID,
		NewCommandIndex_KeyID,
		OldCommandIndex_KeyID,
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
		FORCEINLINE uint8 GetKeyID_OldCommandIndex() const { return OldCommandIndex_KeyID; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_ToTargetActorDistanceSqr() const { return ToTargetActorDistanceSqr_KeyId; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_ToTargetLocationDistanceSqr() const { return ToTargetLocationDistanceSqr_KeyId; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_CurrentActionState() const { return CurrentActionState_KeyID; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE uint8 GetKeyID_IsPaused() const { return bIsPaused_KeyID; }


	//void OnBeingHit(ECharacterType _characterType);

	void SetTargetActor_BBC(AActor* _target);

	void SetTargetLocation_BBC(const FVector& _targetLocation);

	void SetAICurrentActionState_BBC(EAIActionState _aiState);

	void SetNewCommandIndex_BBC(int _newCommand);

	void SetOldCommandIndex_BBC(int _oldCommand);

	void SetToTargetLocationDistanceSqr(float _disTanceSqr);

	void SetToTargetActorDistanceSqr(float _disTanceSqr);

	/** Set BT to wait and stay in wait progress, unless the bIsPaused is unset*/
	void SetIsPaused_BBC(bool _val);



	AActor* GetTargetActor_BBC() const;

	FVector GetTargetLocation_BBC() const;

	EAIActionState GetAICurrentActionState_BBC() const;

	int GetNewCommandIndex_BBC() const;

	int GetOldCommandIndex_BBC() const;

	float GetToTargetLocationDistanceSqr() const;

	float GetToTargetActorDistanceSqr()const;

	bool GetIsPaused_BBC() const;
};
