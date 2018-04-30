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

#define MinSquareFormationNum 3 // the group size must great than 3, to use square formation

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float CrewIndex;


private:

	FTimerHandle mFollowingTimer;
	FVector mFollowingLocation;

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	void SpawnChildGroup() override;

	void OnReform() override;
	
	void SwapChildenOrder() override;

private:

	void RedistributeBy(int _cenMeters);

	void SwitchToScatter();

	void SwitchToCompact();

	void SwitchToSquare();

	void SwitchToRow();

	// Reconstruct the formation info by a given total number, called in OnGroupSizeChangeByNum
	void UpdateFormationInfoByTotalNum(int _totalNum);

	// when group size is increased, called in OnGroupSizeChangeByNum
	void OnGroupSizeChanged_GroupSizeIncresed(int _expectedNum);

	// when group size is decreased, called in OnGroupSizeChangeByNum
	void OnGroupSizeChanged_GroupSizeDecresed(int _expectedNum);

	//void SetAllyGroupVisionVolumn();

	TSubclassOf<class ATheLastBastionAIBase> GetClassDuringAllySpawn(int _currentIndex) const;

public:

	TSubclassOf<class ATheLastBastionAIBase> GetAllyGroupClass() const { return AIToSpawn[0].AIClassBP; }
	//UTexture2D* GetThumbNailImage() const;

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

	void OnHeroLevelUp(int _heroLevel);

	void OnChildDeath(int _childIndex) override;

	/**Called when this unit get selected, show up UI for each child*/
	void OnSelected();

	void OnDeSelected();
 
	/** Called when the exsited ally group member amount is changed from recruit menu*/
	void OnGroupSizeChangeByNum(int _delta);

	int GetMaxColoumnCount() const override;

	int GetMaxRowCount() const override;

	FORCEINLINE bool IsFollowing() const { return bIsFollowing; }

	FORCEINLINE bool IsSquareFormation() const { return bUseSquareFormation; }
	FORCEINLINE bool IsScatterFormation() const { return bUseScatterFormation; }

	FORCEINLINE float GetCurrentRowPadding() const override { return CurrentPadding; }
	FORCEINLINE float GetCurrentColumnPadding() const override { return CurrentPadding; }

	//FORCEINLINE virtual float GetRowPadding() const { return IsSquareFormation() ? (IsScatterFormation()? GroupFormation_ScatterPadding_Square : GroupFormation_CompactPadding_Square) : (IsScatterFormation() ? GroupFormation_CompactPadding_Square : GroupFormation_CompactPadding_Row); }


	void SetSpawnInfo(const FAISpawnInfo& _aiToSpawn); 

	bool CanBeReformed() const;
	
	FORCEINLINE int GetCrewIndex() const { return CrewIndex; }

	/**Set Ally Index on HUD, _index is the index for TArray*/
	void SetHUDIndex(int _index);
};
