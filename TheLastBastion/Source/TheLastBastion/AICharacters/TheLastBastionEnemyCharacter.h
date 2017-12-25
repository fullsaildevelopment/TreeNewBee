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

	UPROPERTY(EditDefaultsOnly, Category = Behavior)
		class UBehaviorTree* BehaviorTree;

private:

	class UAIBase_AnimInstance* mAnimInstanceRef;

protected:

	virtual void BeginPlay();	


public:

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UPawnStatsComponent* GetEnemyStatsComponent() const { return EnemyStats; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }


	UFUNCTION(BlueprintPure)
		FORCEINLINE class UAIBase_AnimInstance* GetAnimInstanceRef() const { return mAnimInstanceRef; }

};
