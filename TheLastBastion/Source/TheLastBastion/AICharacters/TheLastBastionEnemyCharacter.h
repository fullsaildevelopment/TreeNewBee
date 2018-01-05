// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheLastBastionCharacter.h"
#include "TheLastBastionEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API ATheLastBastionEnemyCharacter : public ATheLastBastionCharacter
{
	GENERATED_BODY()
	
public:

	ATheLastBastionEnemyCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		class UPawnStatsComponent* EnemyStats;

protected:

	UPROPERTY(EditDefaultsOnly, Category = AiProperty)
		FText AiName;

	UPROPERTY(EditDefaultsOnly, Category = AiProperty)
		int AILevel;

	UPROPERTY(EditDefaultsOnly, Category = AiProperty)
		int AITier;

	UPROPERTY(EditDefaultsOnly, Category = Behavior)
		class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, Category = AiHud)
		class UWidgetComponent* InfoHUD;

	UPROPERTY(VisibleAnywhere, Category = AiHud)
		bool  bAIHUDisEnabledForLockedOn;

private:

	UPROPERTY()
		class UAIBase_AnimInstance* mAnimInstanceRef;

protected:

	void BeginPlay() override;	

 	UFUNCTION()
		void OnHealthChangedHandle(const UPawnStatsComponent * _pawnStatsComp, float _damage, const UDamageType * _damageType, FName _boneNmame, FVector _shotFromDirection) override;

public:

	/** Toggle the AI hud to screen*/
	void ToggleAIHUD(bool _val);

public:

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UPawnStatsComponent* GetEnemyStatsComponent() const { return EnemyStats; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UAIBase_AnimInstance* GetAnimInstanceRef() const { return mAnimInstanceRef; }

};
