// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGroupBase.h"
#include "EnemyGroup.generated.h"

/**
 * 
 */
#define EM_ToCastle     1
#define EM_AttackPlayer 2



UCLASS()
class THELASTBASTION_API AEnemyGroup : public AAIGroupBase
{
	GENERATED_BODY()
	
public:
	AEnemyGroup();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Behavior)
		int MainTask;


protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	void SpawnChildGroup() override;

	/** Called when the number of group member get changed */
	void OnReform() override;

	// Call when group go to the opposite direction
	void SwapChildenOrder() override;

	//UFUNCTION()
	//	virtual void OnGroupVolumnOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	//		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//UFUNCTION()
	//	virtual void OnGroupVolumnOverrlapEnd(UPrimitiveComponent* OverlappedComponent,
	//		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:

	UFUNCTION()
		void SetMarchLocation(const FVector& _location, int _commandIndex) override;

	void OnChildDeath(int _childIndex) override;

	int GetMaxColoumnCount() const override;

	int GetMaxRowCount() const override;

protected:

	/** Called upon when player enter the combat trigger, while group is not in battle*/
	void MeleeGroupAgainstPlayer();
};
