// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyGroupSpawner.generated.h"



//#define LannisterFirstWave 0
//#define LannisterMiddleWave 5
//#define LannisterLastWave 9

#define WhiteWalkerWave 14
#define FinalBattleDialogDuration 21.0f
#define FinalBattleDialog_FadeInTime 2.0f


#define DefaultTheme_StartPoint_0 0.0f
#define DefaultTheme_StartPoint_1 42.0f

#define LannisterTheme1_StartPoint_0 0.0f
#define LannisterTheme1_StartPoint_1 31.0f
#define LannisterTheme1_StartPoint_2 71.0f

#define LannisterTheme2_StartPoint_0 0.0f
#define LannisterTheme2_StartPoint_1 53.0f

#define WhiteWalkerTheme_StartPoint_0 0.0f
#define WhiteWalkerTheme_StartPoint_1 23.0f

#define WaveAmount 15
#define BGM_FadeOutTime  5.0f
#define BGM_FadeInTime 10.0f


//enum class ESoundEffectType : uint8;

UENUM(BlueprintType)
enum class EPath : uint8 
{
	South_CastleSouth_0 = 0      UMETA(DisplayName = "South to Castle South 0"), 
	South_CastleSouth_1 = 1      UMETA(DisplayName = "South to Castle South 1"),
	South_SouthMine = 2          UMETA(DisplayName = "South to South Mine"),
	East_CastleEast = 3          UMETA(DisplayName = "East to Castle East"),
	East_SouthMine = 4           UMETA(DisplayName = "East to South Mine"),
	East_Stone = 5               UMETA(DisplayName = "East to Stone"),
	East_Food  = 6               UMETA(DisplayName = "East to Food"),
	North_CastleNorth = 7        UMETA(DisplayName = "North to Castle North"),
	North_Wood_Trooper = 8       UMETA(DisplayName = "North to Wood Trooper"),
	North_Wood_Shooter = 9       UMETA(DisplayName = "North to Wood Shooter")
};

USTRUCT(BlueprintType)
struct FMarchPath
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MakeEditWidget), Category = Path)
		/** All Unit Spawn Position*/
		TArray<FTransform> WayPoints;
};

USTRUCT(BlueprintType)
struct FWaveUnit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		TSubclassOf<class AEnemyGroup> GroupClass_Bp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		EPath Path;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		float SpawnDelay = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		class USoundCue* WarningSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		int SpawnTotalNumber = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		int SpawnMaxColumn = 3;

	//FORCEINLINE void SetWaveUnit(TSubclassOf<class AEnemyGroup> _class, int _path) { GroupClass_Bp = _class; Path = _path; }
};

USTRUCT(BlueprintType)
struct FWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		TArray<FWaveUnit> WaveUnits;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BGM)
		USoundCue* WaitingBGM;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BGM)
		USoundCue* BattleBGM;
};




UCLASS()
class THELASTBASTION_API AEnemyGroupSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyGroupSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MakeEditWidget), Category = Spawning)
		/** All Availble Path*/
		TArray<FMarchPath> Paths;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class ASinglePlayerGM* SpGameMode;

	FTimerHandle SpawnTimer;

	FTimerHandle MusicFadeInTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		TArray<FWave> AllWaves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnTest, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class AEnemyGroup> TestGroup;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Music)
		class UAudioComponent* MusicPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Music)
		class USoundCue* CurrentBGM;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnTest, meta = (AllowPrivateAccess = "true"))
		/** The amount of testing group*/
		int TestGroupAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnTest, meta = (AllowPrivateAccess = "true"))
		/** The amount of group member for each testing group*/
		int TestGroupSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnTest, meta = (AllowPrivateAccess = "true"))
		/** The max row size*/
		int TestMaxCol;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnTest)
		/** Time between Spawn during testing mode*/
		float TestingSpawnFreq;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
		float SpawnDelay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
		/** which surviving wave we are currently fight against*/
		int CurrentWaveIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
		/** How many wave units we have spawned so far in current wave*/
		int CurrentWaveUnitIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
		/** How many wave units we are going to spawn in current wave*/
		int MaxWaveUnitAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** Starting wave*/
		int StartWaveIndex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnTest)
		/** How many wave units we are going to spawn in current wave*/
		EPath TestingRoute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SpawnTest)
		/** Spawn a group without any pathing settings*/
		bool bTestingMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
		bool bVictory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
		/** All Unit Spawn Position*/
		bool bEnableSpawning;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawning)
		bool bIsCurrentWaveFinishSpawning;

private:

	UPROPERTY()
	/** Single Player Character ref*/
		const class ATheLastBastionHeroCharacter* Hero;

	/** Hard Coded spawn waves for game cycle*/
	void EditWaves();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FTransform GetNextWayPointFrom(int _pathIndex, int _nextWaypoint) const;

	FORCEINLINE bool IsCurrentWaveFinishSpawning() const { return bIsCurrentWaveFinishSpawning; }
	/** The resting gap between waves */
    bool IsDuringWait() const;

	FORCEINLINE int GetCurrentWaveIndex() const { return CurrentWaveIndex; }
	FORCEINLINE bool GetIsVictory() const { return bVictory; }
	FORCEINLINE bool HasNextWayPointOnPath
	(int _pathIndex, int _wayPointIndex) const { return Paths[_pathIndex].WayPoints.IsValidIndex(_wayPointIndex); }

	void EnableSpawning();

	//void OnCastleDestroy();

private:
	/** Setup indexes that manipulate the spawn process */
	void InitWaveSpawner();

	/** Reset CurrentWaveUnitIndex, If CurrentWaveIndex is out of scope, assigne the currentWaveIndex with max index*/
	void InitCurrentWave();

	/** Called EveryTime after successfully spawn a Group */
	void OnSpawnFinished();

	void FindAllEnemyGroupPreset();

	void Spawn();

	/** Get Spawn Location and direction by path*/
	void GetSpawnTransform(FVector& _location, FQuat& _rotation, int _pathIndex) const;

	/** Play BGM on current Wave, fade out current bgm*/
	void PlayBGM(bool _duringWait = true);

	void BGMFadeIn();

	void BGMFadeOut();

public:

	FORCEINLINE void PlayWaitBGM() { PlayBGM(); }
	FORCEINLINE void PlayBattleBGM() { PlayBGM(false); }
};
