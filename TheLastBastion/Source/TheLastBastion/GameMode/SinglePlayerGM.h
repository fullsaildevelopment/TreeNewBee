// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SinglePlayerGM.generated.h"

/**
 * 
 */

#define MaxEnemyGroupAmount 5
#define AllyGroupMaxAmount 4

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

	UPROPERTY()
		class ASinglePlayerPC* HeroPC;

	/// resource
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		TArray<class AAllyGroup*> Allies;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		TArray<class AOutpost*> AllOutpost;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		class ABarracks* Barracks;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		class ACastle* Castle;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		int Food;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		int Wood;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		int Metal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		int Stone;



	/// Enemy
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		TArray<class AEnemyGroup*> Enemies;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
		class AEnemyGroupSpawner* EnemyGroupSpawner;

	
private:

	void GetAllSpawnClass();

public:

	void Tick(float DeltaSeconds) override;

	void OnHeroLevelUp(int _heroLevel);

	bool HasAllyGroupUnitAt(int _index);

	FORCEINLINE class AAllyGroup* GetAllyGroupUnitAt(int _index) {return (Allies.IsValidIndex(_index)) ? Allies[_index] : nullptr;}
	FORCEINLINE class AEnemyGroup* GetEnemyGroupUnitAt(int _index) { return (Enemies.IsValidIndex(_index)) ? Enemies[_index] : nullptr; }

	FORCEINLINE bool HasValidAlliesAt(int _index) const { return Allies.IsValidIndex(_index); }

	void ToggleAllGroupUI(bool _val);

	void SpawnNewAllies(TSubclassOf<class ATheLastBastionAIBase> _classToSpawn,
		int _totalNum, int _index, bool _isMeleeUnit);

	FORCEINLINE int GetEnemyGroupAmount() const { return Enemies.Num(); }

	FORCEINLINE bool HasRoomNewEnemyGroup() const { return Enemies.Num() < MaxEnemyGroupAmount; }
	///** Spawn Valibility check during test*/
	//FORCEINLINE bool HasRoomNewEnemyGroup_SingleTest() const { return Enemies.Num() < 1; }

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

	// Resouce Interface

	FORCEINLINE int GetFoodTotal() const { return Food; }
	FORCEINLINE int GetMetalTotal() const { return Metal; }
	FORCEINLINE int GetWoodTotal() const { return Wood; }
	FORCEINLINE int GetStoneTotal() const { return Stone; }

	FORCEINLINE void RegisterOutpost(class AOutpost* _outpost) { AllOutpost.Add(_outpost); }
	FORCEINLINE void RegisterCastle(class ACastle* _castle) { Castle = _castle; }
	FORCEINLINE class AOutpost* GetOutpostAt(int _index) { return AllOutpost[_index]; }
	FORCEINLINE int GetAllOutpostAmount() const { return AllOutpost.Num(); }

	void AddFood(int _val);
	void AddMetal(int _val);
	void AddWood(int _val);
	void AddStone(int _val);

	void OnTradeMenuAccept(int _metal, int _wood);
	void OnRecruitMenuAccept(int _food, int _metal, int _wood);

};
