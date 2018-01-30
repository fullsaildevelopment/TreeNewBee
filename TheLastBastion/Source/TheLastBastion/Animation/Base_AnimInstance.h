// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Base_AnimInstance.generated.h"

/**
 * 
 */





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
	(FName boneName ,const FVector& _shotFromDirection, const FVector& _hitLocation);


protected:

	float PlayMontage(class UAnimMontage* _animMontage, float _rate, FName _startSectionName = NAME_None);

	UFUNCTION()
		virtual void OnMontageStartHandle(class UAnimMontage* _animMontage);

	UFUNCTION()
		virtual void OnMontageEndHandle(class UAnimMontage* _animMontage, bool _bInterruptted);

	UFUNCTION()
		virtual void OnMontageBlendOutStartHandle(class UAnimMontage* _animMontage, bool _bInterruptted);


protected:

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


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Movement)
		FVector damageMomentum;


};
