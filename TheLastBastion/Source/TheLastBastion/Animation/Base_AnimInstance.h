// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Base_AnimInstance.generated.h"

/**
 * 
 */


#define MONTAGE_CB_FireOnce   TEXT("FireOnce")


UCLASS()
class THELASTBASTION_API UBase_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:

	UBase_AnimInstance(const FObjectInitializer& _objectInitalizer);


protected:

	virtual void OnBeginPlay();

	virtual void OnInit();

	virtual void OnUpdate(float _deltaTime);

	virtual void OnPostEvaluate();

public:

	virtual void OnBeingHit
	(FName boneName ,const FVector& _damageCauseRelative, const FVector& _hitLocation);

	UFUNCTION()
		virtual bool OnCounterAttack(const FVector& _damageCauserRelative) { return false; }

	UFUNCTION(BlueprintCallable)
	virtual void AnimInstanceResetOnRagDoll();

	void OnGetUp(bool _isFaceDown);


protected:

	float PlayMontage(class UAnimMontage* _animMontage, float _rate, FName _startSectionName = NAME_None);

	UFUNCTION()
		virtual void OnMontageStartHandle(class UAnimMontage* _animMontage);

	UFUNCTION()
		virtual void OnMontageEndHandle(class UAnimMontage* _animMontage, bool _bInterruptted);

	UFUNCTION()
		virtual void OnMontageBlendOutStartHandle(class UAnimMontage* _animMontage, bool _bInterruptted);

	UFUNCTION()
		virtual void ResetOnBeingHit();

	UFUNCTION(BlueprintCallable, Category = Effects)
		/** Play sfx and vfx for one foot step*/
		virtual void FxFootStep();

	//UFUNCTION(BlueprintCallable, Category = Effects)
		virtual void FxMeleeSwing();

	UFUNCTION(BlueprintCallable, Category = Effects)
		virtual void FxOnEquip();

	UFUNCTION(BlueprintCallable, Category = Effects)
		virtual void FxOnDraw();

	UFUNCTION(BlueprintCallable, Category = Effects)
		virtual void FxOnReload();

protected:

	UPROPERTY()
		class ATheLastBastionCharacter* mBaseCharacter;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Combat)
		/** Animation Montage to play when this character get hurt*/
		class UAnimMontage* Hit_Montage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Combat)
		class UAnimMontage* Equip_Montage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Combat)
		class UAnimMontage* Attack_Montage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Combat)
		class UAnimMontage* Dodge_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
		class UAnimMontage* Fire_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
		class UAnimMontage* GetUp_Montage;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Movement)
		FVector damageMomentum;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		/** angle between the acceleration and mesh forward*/
		float turn;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float currentSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float MoveForwardAxis;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float MoveRightAxis;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Movement)
		/** the weight to decide damage momentum direction by damage causer location */
		float MomentumRatioByActor;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Defend)
		bool bOnDefend;


public:

	FORCEINLINE bool IsOnDefend() const { return bOnDefend; }


};
