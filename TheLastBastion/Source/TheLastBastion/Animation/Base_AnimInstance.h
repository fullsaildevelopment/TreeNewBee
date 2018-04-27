// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Base_AnimInstance.generated.h"

/**
 * 
 */


#define MONTAGE_CB_FireOnce   TEXT("FireOnce")



namespace 
{
	UAnimMontage* AM_SingleHandWeapon_HitReaction = nullptr;
	UAnimMontage* AM_CB_HitReaction = nullptr;
	UAnimMontage* AM_HV_HitReaction = nullptr;
	UAnimMontage* AM_TH_HitReaction = nullptr;
	UAnimMontage* AM_Sns_HitReaction = nullptr;
	UAnimMontage* AM_CounterAttack = nullptr;

	UAnimMontage* AM_CrossBow = nullptr;
	UAnimMontage* AM_Skill = nullptr;
	UAnimMontage* AM_HV_ParryDodge = nullptr;
	UAnimMontage* AM_SH_Parry = nullptr;
	UAnimMontage* AM_SH_Dodge = nullptr;
	UAnimMontage* AM_Sns_Parry = nullptr;
	UAnimMontage* AM_Dazed = nullptr;
	UAnimMontage* AM_Katana_Parry = nullptr; // for AI katana parry
	UAnimMontage* AM_Katana_Dodge = nullptr; // for AI katana dodge
	UAnimMontage* AM_CB_Reload = nullptr;
}

enum class EGearType : uint8;

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

	//UFUNCTION()
	//	virtual bool OnCounterAttack(const FVector& _damageCauserRelative) { return false; }

	UFUNCTION(BlueprintCallable)
	virtual void AnimInstanceResetOnRagDoll();

	void OnGetUp(bool _isFaceDown);


protected:

	float PlayMontage(class UAnimMontage* _animMontage, float _rate = 1.0f, FName _startSectionName = NAME_None);

	UFUNCTION()
		virtual void OnMontageStartHandle(class UAnimMontage* _animMontage);

	UFUNCTION()
		virtual void OnMontageEndHandle(class UAnimMontage* _animMontage, bool _bInterruptted);

	UFUNCTION()
		virtual void OnMontageBlendOutStartHandle(class UAnimMontage* _animMontage, bool _bInterruptted);

	UFUNCTION()
		virtual void ResetOnBeingHit();

	virtual void ResetOnBeingStuned() {}

	UFUNCTION(BlueprintCallable, Category = Effects)
		/** Play sfx and vfx for one foot step*/
		virtual void FxFootStep();

		virtual void FxMeleeSwing(bool _rightHand = true);

	UFUNCTION(BlueprintCallable, Category = Effects)
		virtual void FxOnEquip();

	UFUNCTION(BlueprintCallable, Category = Effects)
		virtual void FxOnDraw();

	UFUNCTION(BlueprintCallable, Category = Effects)
		virtual void FxOnReload();

	UFUNCTION(BlueprintCallable, Category = Effects)
		virtual void FxOnJump();

	UFUNCTION(BlueprintCallable, Category = Effects)
		virtual void FxOnRoll();

	UFUNCTION(BlueprintCallable, Category = Effects)
		virtual void StartMeleeWeaponTrail(bool _rightHand = true);

	UFUNCTION(BlueprintCallable, Category = Effects)
		virtual void EndMeleeWeaponTrail(bool _rightHand = true);

	virtual void OnAutoFire() {}

protected:

	UPROPERTY()
		class ATheLastBastionCharacter* mBaseCharacter;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Combat)
		/** current Animation Montage to play when this character get hurt*/
		class UAnimMontage* Hit_Montage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Combat)
		/** current Animation Montage to play when this character need to parry*/
		class UAnimMontage* Parry_Montage;

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

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = CounterAttack)
		class UAnimMontage* CounterAttack_Montage;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Movement)
		FVector damageMomentum;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		/** angle between the desired moving direction 
		* (i.e. acceleration, control rotation, focus point) and mesh forward*/
		float turn;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float currentSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		/** Speed component on X axis*/
		float MoveForwardAxis;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		/** Speed component on Y axis*/
		float MoveRightAxis;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Movement)
		/** the weight to decide damage momentum direction by damage causer location */
		float MomentumRatioByActor;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Defend)
		bool bOnDefend;

public:

	FORCEINLINE bool IsOnDefend() const { return bOnDefend; }
	FORCEINLINE bool IsDoingCounterAttackAnimation() const { return Montage_IsPlaying(CounterAttack_Montage); }
	FORCEINLINE void SetHitMontage(class UAnimMontage* _hitMontage) { Hit_Montage = _hitMontage; }

	virtual void UpdateAnimationSetOnWeaponChange(EGearType _gearType) {}


private:

	bool FindMontage(class UAnimMontage*& _animMontage, const TCHAR* _path);





};
