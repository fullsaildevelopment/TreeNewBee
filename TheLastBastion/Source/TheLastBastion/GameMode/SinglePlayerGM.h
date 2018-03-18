// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SinglePlayerGM.generated.h"

/**
 * 
 */

#define MaxEnemyGroupAmount 1

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GroupClass)
		TSubclassOf<class AAllyGroup> AllyMeleeGroup_Bp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GroupClass)
		TSubclassOf<class AAllyGroup> AllyRangeGroup_Bp;



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

	
private:

	void GetAllSpawnClass();

public:

	void Tick(float DeltaSeconds) override;

	bool HasAllyGroupUnitAt(int _index);

	FORCEINLINE class AAllyGroup* GetAllyGroupUnitAt(int _index) {return (Allies.IsValidIndex(_index)) ? Allies[_index] : nullptr;}
	FORCEINLINE class AEnemyGroup* GetEnemyGroupUnitAt(int _index) { return (Enemies.IsValidIndex(_index)) ? Enemies[_index] : nullptr; }

	FORCEINLINE bool HasValidAlliesAt(int _index) const { return Allies.IsValidIndex(_index); }

	void ToggleAllGroupUI(bool _val);

	void SpawnNewAllies(TSubclassOf<class ATheLastBastionAIBase> _classToSpawn,
		int _totalNum, int _index, bool _isMeleeUnit);

	FORCEINLINE int GetEnemyGroupAmount() const { return Enemies.Num(); }

	FORCEINLINE bool HasRoomNewEnemyGroup() const { return Enemies.Num() < MaxEnemyGroupAmount; }


	// Called when player dismiss a group, destroy the group unit and group itself
	void DestroyAllyGroupAt(int _index);

	// Make a copy of a barrack ref to game mode
	FORCEINLINE void RegisterBarracks(class ABarracks* _barrack) { Barracks = _barrack; }

	// Make a copy of a enemySpawner ref to game mode
	FORCEINLINE void RegisterEnemySpawner(class AEnemyGroupSpawner* _spawner) {EnemyGroupSpawner = _spawner;}

	// Clear the copy of ally group ref at index 
	void UnRegisterAllyGroupAt(int _index);

	// Make a copy of a enemy group ref to game mode
	void RegisterEnemyGroup(class AEnemyGroup* _enemyGroup); 

	// Clear the copy of enemy group ref at index 
	void UnRegisterEnemyGroupAt(int _index); 


	FORCEINLINE class AEnemyGroupSpawner* GetEnemyGroupSpawner() const { return EnemyGroupSpawner; }
};
