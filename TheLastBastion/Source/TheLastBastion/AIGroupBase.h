// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
//#include "GameFramework/Character.h"
#include "AIGroupBase.generated.h"


#define SIDEPADDING 200.0f
#define GroupVolumnZ 10000.0f
#define TARGETREQUEST_UpLimit 4

#define ThreatGain_ByHit 1
#define ThreatGain_ByKill 3
#define ThreatGain_HeroInit 0
#define ThreatGain_AIInit 0

#define COS22_5 0.9f
#define COS67_5 0.4f
#define COS112_5 -0.4f
#define COS157_5 -0.9f

#define COS45 0.7f

#define TargetAtFront_Chasing 1 
#define TargetAtFront_Face2Face 2
#define TargetAtFront_FlankItsLeft 3
#define TargetAtFront_FlankItsRight 4
#define TargetFromBack_BeingChased 5 // being Surprised from back
#define TargetFromBack_Back2Back 6
#define TargetFromBack_ToMyRight 7
#define TargetFromBack_ToMyLeft 8

#define TargetFromLeft_BackToUs 9
#define TargetFromLeft_FaceToUs 10 // being Flanked from Left
#define TargetFromRight_BackToUs 11
#define TargetFromRight_FaceToUs 12 // being Flanked from Right

#define MaxGroupSize 20
#define RangeUnitShootingRange 8000
#define VisionHalfHeight 200.0f
#define GroupFrontExtraVision 100.0f


USTRUCT(BlueprintType)
struct FAISpawnInfo 
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
	TSubclassOf<class ATheLastBastionAIBase> AIClassBP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** the number of this ai class in group*/
	int32 TotalNumber;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** the max number of Col this ai class has in group*/
	int32 MaxNumOfCol = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
	int32 NumOfRow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** the distance between each ai in same row */
	float ColumnPadding = 250;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** the distance between each ai in same column */
	float RowPadding = 250;
};

USTRUCT(BlueprintType)
struct FAICharacterInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class ATheLastBastionAIBase* AICharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector GroupRelativeOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		/** the row - column index of the this character of same class*/
		FVector2D GroupIndexOffset;
};

USTRUCT(BlueprintType)
struct FThreat
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class AActor* Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float GroupThreat;

	UPROPERTY()
		float Manhaton;
};



UCLASS(BlueprintType)
class THELASTBASTION_API AAIGroupBase : public APawn
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		TArray<FAICharacterInfo> AICharactersInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		// size of array means the number of row, each entry is the number of col in current row
		TArray<int> FormationInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		// the dead group member should be remove from the character list, before execute the next command
		TArray<int> RemovePendingList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Behavior)
		// Look up map for check the individual character to this threat
		TMap<class ATheLastBastionCharacter* , float> ThreatMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** The class and the number we about to spawn*/
		TArray<FAISpawnInfo> AIToSpawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		/** Trigger volumn to present the group size and trigger group combat*/
		class UBoxComponent* MeleeVision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		/** Trigger volumn to present the group size and trigger group combat*/
		class UBoxComponent* RangeVision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		/** Trigger volumn to present the group size and trigger group combat*/
		class UFloatingPawnMovement* MoveComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		/** Trigger volumn to present the group size and trigger group combat*/
		class UArrowComponent* ArrowComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Behavior)
		class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UWidgetComponent* GroupHUD;

	UPROPERTY()
		class ATheLastBastionHeroCharacter* PlayerHero;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Behavior)
		FVector GroupTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Behavior)
		FVector GroupTargetForward;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Behavior)
		FVector GroupTargetRight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		bool bDisabled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Behavior)
		/** The Group Already In Battle, group members already has target */
		bool bInBattle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Behavior)
		bool bIsAgreesive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Formation)
		bool bReformPending;


	FTimerHandle mGroupUpdateTimer;


public:
	// Sets default values for this pawn's properties
	AAIGroupBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Update();

	void UpdateGroupVolumnDuringBattle();

	virtual void SpawnChildGroup();

	/** Called when the number of group member get changed */
	virtual void OnReform();

	// Call when group go to the opposite direction
	virtual void SwapChildenOrder();

	// Send group command index to each group member BT 
	void SendGroupCommand(int _commandIndex);

	// Return a index that present target relative location and heading
	int CheckTargetRelativeWhereAbout(const AActor* const _target ) const;

	/// Melee Group Target Selection
	void MeleeTargetSelect_TAtF_Chasing(AAIGroupBase * const _targetGroup);
	void MeleeTargetSelect_TAtF_F2F(AAIGroupBase * const _targetGroup);
	void MeleeTargetSelect_TAtF_FL(AAIGroupBase * const _targetGroup);
	void MeleeTargetSelect_TAtF_FR(AAIGroupBase * const _targetGroup);
	void MeleeTargetSelect_TFL_Face2Us(AAIGroupBase * const _targetGroup);
	void MeleeTargetSelect_TFR_Face2Us(AAIGroupBase * const _targetGroup);



	// all unit target on the first threat, when there comes the first threat
	void RangeTargetSelect_OnFirstOverlap(AActor* TargetActor);



	UFUNCTION()
		virtual void OnMeleeVisionOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);


	UFUNCTION()
		virtual void OnMeleeVisionOverrlapEnd(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UFUNCTION()
		virtual void OnRangeVisionOverrlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		virtual void OnRangeVisionOverrlapEnd(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	/** Melee Unit version of OnTarget Request*/
	AActor* OnTargetRequest_Melee(const ATheLastBastionCharacter* _requestSender);
	/** Range Unit version of OnTarget Request*/
	AActor* OnTargetRequest_Range(const ATheLastBastionCharacter* _requestSender);


	virtual void SetGroupVisionVolumn(float _maxGroupWidth, float _maxGroupLength);

	float GetDivider(int _index) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PairColumn(AAIGroupBase* const _enemyGroup, int _myColumn, int _theirColumn);
	void AssignColumnToColumn(AAIGroupBase* const _targetGroup, int _myColumn, int _theirColumn, bool _isReversed = false);
	void AssignRowToColumn(AAIGroupBase* const _targetGroup, int _myRow, int _theirColumn, bool _isReversed = false);
	void AssignColumnToRow(AAIGroupBase* const _targetGroup, int _myColumn, int _theirRow, bool _isReversed = false);

	void MeleeTargetSelectionOnOverlap(AAIGroupBase* _targetGroup);


	UFUNCTION()
		virtual void SetMarchLocation(const FVector& _location, int _commandIndex);
	UFUNCTION()
		virtual void OnChildDeath(int _childIndex);


	void AddThreat(class ATheLastBastionCharacter* _character, float _threat);
	void AddThreatByGroup(class AAIGroupBase* _targetGroup);
	void RemoveThreat(class ATheLastBastionCharacter* _character);
	void RemoveThreatByGroup(class AAIGroupBase* _targetGroup);



	virtual AActor* OnTargetRequest(const ATheLastBastionCharacter* _requestSender);

	void QuickSortThreatListByManDistance(TArray<FThreat>& _threatList, int _left, int _right) const;

	void ToggleHUDVisibility(bool _val);

	/** Update the children location during move to*/
	/** DEPRECATED */
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void CheckGroupCommand();

	UFUNCTION()
		/** Get Current most front line*/
		TArray<class ATheLastBastionAIBase*> GetFrontLine() const;
	UFUNCTION()
		/** Get Current most Back line*/
		TArray<class ATheLastBastionAIBase*> GetBackLine() const;

	UFUNCTION()
		/** Get Current most Right line*/
		TArray<class ATheLastBastionAIBase*> GetRightLine() const;
	UFUNCTION()
		/** Get Current most Left line*/
		TArray<class ATheLastBastionAIBase*> GetLeftLine() const;


	UFUNCTION()
		virtual TArray<class ATheLastBastionAIBase*> GetColumnAt(int _index) const;
	UFUNCTION()
		virtual TArray<class ATheLastBastionAIBase*> GetRowAt(int _index) const;

	FORCEINLINE bool IsInBattle() const { return bInBattle; }
	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
	FORCEINLINE int GetGroupSize() const { return AICharactersInfo.Num(); }
	FORCEINLINE class ATheLastBastionAIBase* GetGroupMemberAt(int _index) const { return AICharactersInfo[_index].AICharacter; }

	virtual int GetMaxColoumnCount() const;
	virtual int GetMaxRowCount() const;

	FORCEINLINE FVector GetGroupRelativeOffsetAt(int _index) const { return AICharactersInfo[_index].GroupRelativeOffset; }
	FORCEINLINE FVector GetGroupTargetLocation() const { return GroupTargetLocation; }
	FORCEINLINE FVector GetGroupTargetForward() const { return GroupTargetForward; }
	FORCEINLINE FVector GetGroupTargetRight() const { return GroupTargetRight; }
};
