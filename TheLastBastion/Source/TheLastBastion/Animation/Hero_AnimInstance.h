// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Hero_AnimInstance.generated.h"

/**
 * 
 */
UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EEquipType : uint8
{
	Travel = 0 	    UMETA(DisplayName = "Travel"),
	ShieldSword 	UMETA(DisplayName = "ShieldSword"),
	TwoHandSword	UMETA(DisplayName = "TwoHandSword"),
	Bow             UMETA(DisplayName = "Bow")
};




UCLASS()
class THELASTBASTION_API UHero_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:

	UHero_AnimInstance(const FObjectInitializer& _objectInitalizer);

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


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = HeadTrack)
		float headTrackRate;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = HeadTrack)
		float headTrackYaw;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = HeadTrack)
		float headTrackPitch;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		FVector acceleration_bodySpace;


	/** Current activated equipment type */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Equipment)
		EEquipType activatedEquipment;

	/** The type of weapon is going to use when player draw weapon*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Equipment)
		EEquipType currentEquipment;

	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Action)
		class UAnimMontage* Equip_Montage;

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

		float PlayMontage(class UAnimMontage* _animMontage, float _rate, FName _startSectionName = NAME_None);



public:


	virtual void OnAttack();
	virtual void OnEquip();

	FORCEINLINE bool IsSpeedOverrideByAnim() const { return bSpeedOverrideByAnim; }
	FORCEINLINE bool IsRotationRateOverrideByAnim() const { return bRotationRateOverrideByAnim; }
	FORCEINLINE bool GetIsJumpEnable() const { return bEnableJump; }
	FORCEINLINE void SetIsJump(bool _val) { bTryToJump = _val; }

	FORCEINLINE EEquipType GetCurrentEquipmentType() const { return currentEquipment; }
	FORCEINLINE EEquipType GetActivatedEquipmentType() const { return activatedEquipment;  }

private:

	void HeadTrack();
};
