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

	UPROPERTY()
		FTimerHandle mKillTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		class UPawnStatsComponent* AIStats;

	UPROPERTY()
		class UAIBase_AnimInstance* mAnimInstanceRef;

	UPROPERTY(EditDefaultsOnly, Category = Behavior)
		class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, Category = AiHud)
		class UWidgetComponent* InfoHUD;

	UPROPERTY()
		class AAIGroupBase* mGroup;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GroupBehavior)
		int mGroupIndex;

	UPROPERTY(EditDefaultsOnly, Category = AiProperty)
		int AILevel;

	UPROPERTY(EditDefaultsOnly, Category = AiProperty)
		int AITier;

protected:

	void BeginPlay() override;


public:

	/** Toggle the AI hud to screen*/
	virtual void ToggleAIHUD(bool _val);

public:

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UPawnStatsComponent* GetEnemyStatsComponent() const { return AIStats; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UAIBase_AnimInstance* GetAnimInstanceRef() const { return mAnimInstanceRef; }

	void SetParent(class AAIGroupBase* _Group, int _groupIndex); 

	FORCEINLINE void SetGroupIndex(int _groupIndex) { mGroupIndex = _groupIndex; }

		UFUNCTION(BlueprintCallable)
			void CalculateMarchTargetPosition();

protected:

	// Called on Hp = 0;
	void OnDead();

	// Called on actor destroyed
	void Kill();

};
