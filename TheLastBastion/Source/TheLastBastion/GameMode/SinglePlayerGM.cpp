// Fill out your copyright notice in the Description page of Project Settings.

#include "SinglePlayerGM.h"
#include "PCs/SinglePlayerPC.h"
#include "CustomType.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/SpawnLocation.h"
#include "TimerManager.h"
#include "TheLastBastionCharacter.h"
#include "AI/AllyGroup.h"
#include "AudioManager.h"


ASinglePlayerGM::ASinglePlayerGM(const FObjectInitializer & _objectInitilizer) : Super(_objectInitilizer)
{
	UAudioManager* audioManager = NewObject<UAudioManager>(UAudioManager::StaticClass());
	if (audioManager == nullptr)
		UE_LOG(LogTemp, Error, TEXT("audioManager"));

	PlayerControllerClass = ASinglePlayerPC::StaticClass();
	UCustomType::FindClass<APawn>(DefaultPawnClass, TEXT("/Game/Blueprints/Heros/SinglePlayer_Bp"));
	bUseSeamlessTravel = true;
	
	GetAllCharacterClass();
	EnemyAmount = 0;
	WaveCount = 0;
	NumOfEnemiesToSpawn = 0;
	TimeBetweenWaves = 10.0f;

	// Change the tick rate
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 2.0f;
}

void ASinglePlayerGM::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(NewPlayer);

	pc->OnPostLogin();

}

void ASinglePlayerGM::HandleSeamlessTravelPlayer(AController *& C)
{

	// Default behavior is to spawn new controllers and copy data
	APlayerController* PC = Cast<APlayerController>(C);
	if (PC && PC->Player)
	{
		// We need to spawn a new PlayerController to replace the old one
		APlayerController* NewPC = SpawnPlayerController(PC->IsLocalPlayerController() ? ROLE_SimulatedProxy : ROLE_AutonomousProxy, PC->GetFocalLocation(), PC->GetControlRotation());

		if (NewPC)
		{
			PC->SeamlessTravelTo(NewPC);
			NewPC->SeamlessTravelFrom(PC);
			SwapPlayerControllers(PC, NewPC);
			PC = NewPC;
			C = NewPC;
		}
		else
		{
			UE_LOG(LogGameMode, Warning, TEXT("HandleSeamlessTravelPlayer: Failed to spawn new PlayerController for %s (old class %s)"), *PC->GetHumanReadableName(), *PC->GetClass()->GetName());
			PC->Destroy();
			return;
		}
	}

	InitSeamlessTravelPlayer(C);

	// Initialize hud and other player details, shared with PostLogin
	GenericPlayerInitialization(C);

	if (PC)
	{
		// This may spawn the player pawn if the game is in progress
		ASinglePlayerPC* spPC = Cast<ASinglePlayerPC>(PC);
		if (spPC)
		{
			spPC->OnFinishSeamlessTravel();
		}
	}

}

void ASinglePlayerGM::BeginPlay()
{
	TArray<AActor*> AllSpawnLocations;
	ASpawnLocation* spawner = nullptr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnLocation::StaticClass(), AllSpawnLocations);
	for (size_t i = 0; i < AllSpawnLocations.Num(); i++)
	{
		spawner = Cast<ASpawnLocation>(AllSpawnLocations[i]);
		if (spawner)
		{
			switch (spawner->GetSpawnLocationType())
			{
			case ESpawnWorldLocation::Lannester_S1:
			{   
				LannesterSpawnLocations_One.Add(spawner);
				break;
			}

			case ESpawnWorldLocation::Lannester_S2:
			{
				//LannesterSpawnLocations_One.Add(spawner);
				break;
			}

			case ESpawnWorldLocation::Lannester_S3:
			{
				//LannesterSpawnLocations_One.Add(spawner);
				break;
			}

			case ESpawnWorldLocation::WhiteWalker_S1:
			{
				//LannesterSpawnLocations_One.Add(spawner);
				break;
			}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("%s"), *spawner->GetName());

	}

	/// Temp Code
	/// Get All Allies on the scene, since we cant not spawn any ally unit yet
	TArray<AActor*> AllAllies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAllyGroup::StaticClass(), AllAllies);
	int AlliesCount = AllAllies.Num();
	AlliesCount = (AlliesCount > 4) ? 4 : AlliesCount;

	Allies.SetNum(AlliesCount);
	for (int iAllies = 0; iAllies < AlliesCount; iAllies++)
	{
		Allies[iAllies] = Cast<AAllyGroup>(AllAllies[iAllies]);
		if (Allies[iAllies])
		{
			Allies[iAllies]->SetHUDIndex(iAllies + 1);
		}
	}
}

void ASinglePlayerGM::SpawnNewEnemy()
{
	if (LannesterTrooper_T0_BP && LannesterShooter_T0_BP)
	{
		UWorld* const world = GetWorld();
		if (world && LannesterSpawnLocations_One.Num() != 0)
		{   
			for (int32 i = 0; i < LannesterSpawnLocations_One.Num(); i++)
			{   
				if (LannesterSpawnLocations_One[i]->IsWorking())
				{
					int32 EnemyTypeToSpawn = FMath::RandRange(0, 1);
					FVector RandomSpawnPoint = LannesterSpawnLocations_One[i]->GetRandomSpawnPoint();
					FActorSpawnParameters spawnParam;
					spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
					switch (EnemyTypeToSpawn)
					{
					case 0:
					{
						ATheLastBastionCharacter* aiPawn = world->SpawnActor<ATheLastBastionCharacter>(LannesterTrooper_T0_BP, RandomSpawnPoint, FRotator::ZeroRotator, spawnParam);
						if (aiPawn)
							aiPawn->SpawnDefaultController();
						else
							UE_LOG(LogTemp, Warning, TEXT("SomeHow spawner cant spawn .... - ASinglePlayerGM::SpawnNewEnemy"));

						break;
					}
					case 1:
					{
						ATheLastBastionCharacter* aiPawn = world->SpawnActor<ATheLastBastionCharacter>(LannesterShooter_T0_BP, RandomSpawnPoint, FRotator::ZeroRotator, spawnParam);
						if (aiPawn)
							aiPawn->SpawnDefaultController();
						else
							UE_LOG(LogTemp, Warning, TEXT("SomeHow spawner cant spawn .... - ASinglePlayerGM::SpawnNewEnemy"));

						break;
					}
					}
					EnemyAmount++;

				}
			}
		}
	}

}

void ASinglePlayerGM::SpawnEnemyTimerElapsed()
{
	SpawnNewEnemy();
	NumOfEnemiesToSpawn--;
	if (NumOfEnemiesToSpawn <= 0)
	{
		EndWave();
	}
}

void ASinglePlayerGM::StartWave()
{   
	WaveCount++;
	NumOfEnemiesToSpawn = WaveCount * 2.0f;
	GetWorldTimerManager().SetTimer(TimerHandle_EnemySpawner, this, &ASinglePlayerGM::SpawnEnemyTimerElapsed, 1.0f, true, 0.0f);
}

void ASinglePlayerGM::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_EnemySpawner);
}

void ASinglePlayerGM::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASinglePlayerGM::StartWave, TimeBetweenWaves, false);
}

void ASinglePlayerGM::CheckWaveState()
{   
	bool bIsPreparingForNextWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	// If we still have enemies to spawn or we already prepare for next wave, do not execute the preparefornextwave function
	if (NumOfEnemiesToSpawn > 0 || bIsPreparingForNextWave)
	{
		return;
	}
	if (EnemyAmount == 0)
	{
		PrepareForNextWave();
	}
}

void ASinglePlayerGM::GetAllCharacterClass()
{

	if (LannesterTrooper_T0_BP == nullptr)
		UCustomType::FindClass<ATheLastBastionCharacter>(LannesterTrooper_T0_BP, TEXT("/Game/Blueprints/AI/Lannester/Lan_Trooper_T0"));

	if (LannesterShooter_T0_BP == nullptr)
		UCustomType::FindClass<ATheLastBastionCharacter>(LannesterShooter_T0_BP, TEXT("/Game/Blueprints/AI/Lannester/Lan_Crossbow_T0"));

}

void ASinglePlayerGM::UpdateEnemyAmount(int _val)
{
	EnemyAmount += _val;
}

void ASinglePlayerGM::StartPlay()
{
	Super::StartPlay();
	PrepareForNextWave();
}

void ASinglePlayerGM::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CheckWaveState();
}

bool ASinglePlayerGM::HasAllyGroupUnitAt(int _index)
{
	return 	Allies.IsValidIndex(_index);
}

AAllyGroup * ASinglePlayerGM::GetAllyGroupUnitAt(int _index)
{
	if (Allies.IsValidIndex(_index))
	{
		return Allies[_index];
	}
	return nullptr;
}

void ASinglePlayerGM::ToggleAllGroupUI(bool _val)
{
	// toggle all allies HUD

	for (int i = 0; i < Allies.Num(); i++)
	{
		if (Allies[i] != nullptr)
		{
			Allies[i]->ToggleHUDVisibility(_val);
		}
	}

	// toggle all enemy group hud
}
