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

	virtual void BeginPlay();	
};
