// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SinglePlayerGM.generated.h"

/**
 * 
 */

#define MaxEnemyGroupAmount 2

UCLASS()
class THELASTBASTION_API ASinglePlayerGM : public AGameModeBase
{
	GENERATED_BODY()
	

public:

	ASinglePlayerGM(const FObjectInitializer & _objectInitilizer);
	
	void PostLogin(APlayerController* NewPlayer) override;

	void HandleSeamlessTravelPlayer(AController*& C) override;

protected:

	void BeginPlay() override;


protected:

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CharacterClass)
	//	TSubclassOf<class ATheLastBastionCharacter> LannesterTrooper_T0_BP;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CharacterClass)
	//	TSubclassOf<class ATheLastBastionCharacter> LannesterShooter_T0_BP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GroupClass)
		TSubclassOf<class AAllyGroup> AllyMeleeGroup_Bp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GroupClass)
		TSubclassOf<class AAllyGroup> AllyRangeGroup_Bp;


	//FTimerHandle TimerHandle_EnemySpawner;

	//FTimerHandle TimerHandle_NextWaveStart;

	//// Bots to spawn in current wave
	//int32 NumOfEnemiesToSpawn;

	//int32 WaveCount;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawner")
	//float TimeBetweenWaves;

//protected:
//	// Spawn a single enemy
//	void SpawnNewEnemy();
//
//	void SpawnEnemyTimerElapsed();
//
//	// Start Spawning Bots
//	void StartWave();
//
//	// Stop Spawning Bots
//	void EndWave();
//
//	// Set timer for next startwave
//	void PrepareForNextWave();
//
//	// Check enemy amount to determine if we're ready for next wave
//	void CheckWaveState();


private:

	/// resource
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		TArray<class AAllyGroup*> Allies;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		class ABarracks* Barracks;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		int Gold;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		int Wood;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		int Metal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		int Rock;



	/// Enemy
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		TArray<class AEnemyGroup*> Enemies;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		class AEnemyGroupSpawner* EnemyGroupSpawner;


	//
	//UPROPERTY()
	//	TArray<class ASpawnLocation*> LannesterSpawnLocations_One;

	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	//	int32 EnemyAmount;

	
private:

	void GetAllSpawnClass();

public:

	//void UpdateEnemyAmount(int _val);

	//void StartPlay() override;

	void Tick(float DeltaSeconds) override;

	bool HasAllyGroupUnitAt(int _index);

	class AAllyGroup* GetAllyGroupUnitAt(int _index);

	FORCEINLINE bool HasValidAlliesAt(int _index) const { return Allies.IsValidIndex(_index); }

	void ToggleAllGroupUI(bool _val);

	void SpawnNewAllies(TSubclassOf<class ATheLastBastionAIBase> _classToSpawn,
		int _totalNum, int _index, bool _isMeleeUnit);

	FORCEINLINE int GetEnemyGroupAmount() const { Enemies.Num(); }

	FORCEINLINE bool HasRoomNewEnemyGroup() const { Enemies.Num() < MaxEnemyGroupAmount; }

	// Make a copy of a enemy group ref to game mode
	FORCEINLINE void RegisterEnemyGroup(class AEnemyGroup* _enemyGroup) { Enemies.Add(_enemyGroup); }

	// Called when player dismiss a group, destroy the group unit and group itself
	void DestroyAllyGroupAt(int _index);

	// Make a copy of a barrack ref to game mode
	FORCEINLINE void RegisterBarracks(class ABarracks* _barrack) { Barracks = _barrack; }

	// Make a copy of a enemySpawner ref to game mode
	FORCEINLINE void RegisterEnemySpawner(class AEnemyGroupSpawner* _spawner) {EnemyGroupSpawner = _spawner;}
};
