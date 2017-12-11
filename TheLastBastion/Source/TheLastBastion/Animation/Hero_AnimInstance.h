// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Hero_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UHero_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()


private:

	bool bSpeedOverrideByAnim;
	bool bRotationRateOverrideByAnim;


protected:
	

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		bool bEnableJump;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		bool bTryToJump;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		bool bTryToMove;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float currentSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float turn;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		int activatedStateMachine;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		FVector acceleration_bodySpace;

protected:

	UPROPERTY(BlueprintReadOnly)
		class ATheLastBastionCharacter* mCharacter;

protected:

		virtual void OnBeginPlay();

		virtual void OnInit();

		virtual void OnUpdate(float _deltaTime);

		virtual void OnPostEvaluate();	

#pragma region Anim Notification

		UFUNCTION(BlueprintCallable)
			void StartOverrideSpeed();

		UFUNCTION(BlueprintCallable)
			void StopOverrideSpeed();

		UFUNCTION(BlueprintCallable)
			void EnableJump();

		UFUNCTION(BlueprintCallable)
			void DisableJump();

#pragma endregion



public:


	virtual void OnAttack();

	FORCEINLINE bool IsSpeedOverrideByAnim() const { return bSpeedOverrideByAnim; }
	FORCEINLINE bool IsRotationRateOverrideByAnim() const { return bRotationRateOverrideByAnim; }
	FORCEINLINE bool GetIsJumpEnable() const { return bEnableJump; }
	FORCEINLINE void SetIsJump(bool _val) { bTryToJump = _val; }

};
