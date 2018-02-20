// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheLastBastionCharacter.h"
#include "TheLastBastionAIBase.generated.h"

/**
 * 
 */


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
		int mGroupIndex;

	UPROPERTY(EditDefaultsOnly, Category = AiProperty)
		int AILevel;

	UPROPERTY(EditDefaultsOnly, Category = AiProperty)
		int AITier;


protected:

	void BeginPlay() override;

public:

	UFUNCTION(BlueprintNativeEvent)
		UTexture2D* GetThumbNailImage() const;

	UTexture2D* GetThumbNailImage_Implementation() const { return nullptr; }


	UFUNCTION()
		virtual void OnTargetDeathHandle();

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UPawnStatsComponent* GetEnemyStatsComponent() const { return AIStats; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UAIBase_AnimInstance* GetAnimInstanceRef() const { return mAnimInstanceRef; }


	UFUNCTION(BlueprintPure)
		FORCEINLINE class AAIGroupBase* GetGroup() const { return mGroup; }

	FORCEINLINE void SetGroupIndex(int _groupIndex) { mGroupIndex = _groupIndex; }

	UFUNCTION(BlueprintCallable)
			bool OnGroupTaskStart();

	/** Toggle the AI hud to screen*/
	virtual void ToggleAIHUD(bool _val);

	bool OnFriendFireCheck(const ATheLastBastionCharacter* _target);

	void SetTarget(AActor* _target);

	void SetParent(class AAIGroupBase* _Group, int _groupIndex);

	AActor* GetTarget() const;

	void RequestAnotherTarget();



protected:


	UFUNCTION()
		void OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType
			, class AController* InstigatedBy, AActor* DamageCauser) override;
	UFUNCTION()
		void OnTakePointDamageHandle(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
			class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
			const class UDamageType* DamageType, AActor* DamageCauser) override;

	virtual void GenerateFloatingText(const FVector& HitLocation, const class ATheLastBastionHeroCharacter* heroAttacker,
		float totalDamage, bool isCritical, bool isStun);

	virtual void EvaluateAttackerThreat(AActor* DamageCauser, float hp);

	// Define how am I going to do with the damage causer
	virtual void HitResponse(AActor* DamageCauser);

	// Called on Hp = 0;
	void OnDead(const FVector& dir, const AActor* _damageCauser, const FName& _boneName) override;

	// Called on actor destroyed
	void Kill() override;

};
