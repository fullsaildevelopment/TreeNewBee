// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGroupBase.h"
#include "AllyGroup.generated.h"

/**
 * 
 */

#define GroupFormation_ScatterPadding_Square 500.0f
#define GroupFormation_CompactPadding_Square 250.0f

#define GroupFormation_ScatterPadding_Row  400.0f
#define GroupFormation_CompactPadding_Row  150.0f


UCLASS()
class THELASTBASTION_API AAllyGroup : public AAIGroupBase
{
	GENERATED_BODY()

public:
	AAllyGroup();
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		bool bUseSquareFormation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		bool bUseScatterFormation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		bool bIsFollowing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Formation)
		float FollowingFrq;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		float CurrentPadding;

private:

	FTimerHandle mFollowingTimer;
	FVector mFollowingLocation;


protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	void SpawnChildGroup() override;

	void OnReform() override;
	
	void SwapChildenOrder() override;


	//UFUNCTION()
	//	void OnGroupVolumnOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	//		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	//UFUNCTION()
	//	void OnGroupVolumnOverrlapEnd(UPrimitiveComponent* OverlappedComponent,
	//		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:

	void RedistributeBy(int _cenMeters);

	void SwitchToScatter();

	void SwitchToCompact();

	void SwitchToSquare();

	void SwitchToRow();

	TSubclassOf<class ATheLastBastionAIBase> GetClassDuringAllySpawn(int _currentIndex) const;

public:

	UFUNCTION()
		void SetMarchLocation(const FVector& _location, int _commandIndex) override;


	UFUNCTION()
		virtual TArray<class ATheLastBastionAIBase*> GetColumnAt(int _index) const;
	UFUNCTION()
		virtual TArray<class ATheLastBastionAIBase*> GetRowAt(int _index) const;


	UFUNCTION()
		void SetFollowingLocation();

		void OnStartFollowing();

		void OnStopFollowing();


	void OnChildDeath(int _childIndex) override;

	/**Called when this unit get selected, show up UI for each child*/
	void OnSelected();

	void OnDeSelected();

	int GetMaxColoumnCount() const override;

	int GetMaxRowCount() const override;

	FORCEINLINE bool IsFollowing() const { return bIsFollowing; }

	bool CanBeReformed() const;

	void SetHUDIndex(int _index);

};
