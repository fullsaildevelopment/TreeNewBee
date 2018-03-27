// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheLastBastionCharacter.h"
#include "TheLastBastionAIBase.generated.h"

/**
 * 
 */

#define RangeUnitMinimumFirePlayBackTime 3.0f

UCLASS()
class THELASTBASTION_API ATheLastBastionAIBase : public ATheLastBastionCharacter
{
	GENERATED_BODY()
	
public:

	ATheLastBastionAIBase();

protected:

	UPROPERTY(EditDefaultsOnly, Category = AiProperty)
		FText AiName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		class UPawnStatsComponent* AIStats;

	UPROPERTY()
		class UAIBase_AnimInstance* mAnimInstanceRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Behavior)
		class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AiHud)
		class UWidgetComponent* InfoHUD;

	UPROPERTY()
		class AAIGroupBase* mGroup;

	UPROPERTY()
		class UInGameAIHUD* AI_HUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GroupBehavior)
		FVector ChildtargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GroupBehavior)
		int mGroupIndex;

	//UPROPERTY(EditDefaultsOnly, Category = AiProperty)
	//	int AILevel;

	UPROPERTY(EditDefaultsOnly, Category = AiProperty)
		int AITier;

	UPROPERTY(EditAnywhere, Category = AiProperty)
		/** The waiting time between each attack */
		float AttackWait;

	UPROPERTY(EditDefaultsOnly, Category = AiProperty)
		bool bIsRangeUnit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		/* if true, this character will be walking instead of jogging*/
		bool bIsWalkingUnit;



protected:

	void BeginPlay() override;

public:

	UFUNCTION(BlueprintNativeEvent)
		UTexture2D* GetThumbNailImage() const;
	UTexture2D* GetThumbNailImage_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintNativeEvent)
		float GetSiegePoint() const;
	float GetSiegePoint_Implementation() const { return 1.0f; }

	UFUNCTION(BlueprintNativeEvent)
		/** The amount of exp that hero gain, when this character is defeated*/
		float GetExperience() const;
	float GetExperience_Implementation() const { return 120.0f; }

	UFUNCTION()
		virtual void OnTargetDeathHandle();

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsRangeUnit() const { return bIsRangeUnit; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsNearTargetLocation(float _distSq) const { return FVector::DistSquared2D(GetActorLocation(), ChildtargetLocation) < _distSq; }


	UFUNCTION(BlueprintPure)
		FORCEINLINE class UPawnStatsComponent* GetEnemyStatsComponent() const { return AIStats; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UAIBase_AnimInstance* GetAnimInstanceRef() const { return mAnimInstanceRef; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class AAIGroupBase* GetGroup() const { return mGroup; }

	/** Get Waiting Time between two attack */
	FORCEINLINE float GetAttackWait() const { return (bIsRangeUnit)? AttackWait : (AttackWait + RangeUnitMinimumFirePlayBackTime); }
	/** Check if this ai is walking or jogging*/
	FORCEINLINE bool IsWalking() const { return bIsWalkingUnit; }


	FORCEINLINE void SetGroupIndex(int _groupIndex) { mGroupIndex = _groupIndex; }

	UFUNCTION(BlueprintCallable)
			bool OnGroupTaskStart();

	/** Toggle the AI hud to screen*/
	virtual void ToggleAIHUD(bool _val);

	bool HasFullHealth() const;

	bool OnFriendFireCheck(const ATheLastBastionCharacter* _target);

	/** Set target to this AI, default to binding the target request delegate*/
	void SetTarget(AActor* _target, bool _asGroupMember = false);

	void SetParent(class AAIGroupBase* _Group, int _groupIndex);

	AActor* GetTarget() const;

	void RequestAnotherTarget();

	// Called on actor destroyed
	void Kill() override;

protected:


	UFUNCTION()
		void OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType
			, class AController* InstigatedBy, AActor* DamageCauser) override;
	UFUNCTION()
		void OnTakePointDamageHandle(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
			class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
			const class UDamageType* DamageType, AActor* DamageCauser) override;

	/** Generate floating text to represent damage*/
	virtual void OnTakeDamageFromHero(const FVector& HitLocation, const class ATheLastBastionHeroCharacter* heroAttacker,
		float totalDamage, bool isCritical, bool isStun) {}

	virtual void EvaluateAttackerThreat(AActor* DamageCauser, float hp);

	// Define how does this AI response to the damage causer
	virtual void HitResponse(AActor* DamageCauser, float _currentHp) {  }

	// Called on Hp = 0;
	void OnDead(const FVector& dir, const AActor* _damageCauser, FName _boneName) override;


	void AddExp(class ATheLastBastionHeroCharacter* _heroAttacker);

};
