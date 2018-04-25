// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyGroupSpawner.h"
#include "CustomType.h"
#include "AI/EnemyGroup.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/SinglePlayerGM.h"
#include "Sound/SoundCue.h"
#include "TheLastBastionCharacter.h"
#include "AudioManager.h"
#include "Components/AudioComponent.h"
#include "TimerManager.h"

// Sets default values
AEnemyGroupSpawner::AEnemyGroupSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Initialize music player
	MusicPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicPlayer"));

	//FindAllEnemyGroupPreset();

	SpawnDelay = 5.0f;
	TestGroupAmount = 1;
	TestGroupSize = 2;
	TestingRoute = EPath::South_TrooperRoute_0;

	// initialize default spawning state
	bEnableSpawning = false;
	bIsCurrentWaveFinished = true;
	CurrentWaveIndex = 0;
}

// Called when the game starts or when spawned
void AEnemyGroupSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	//EditWaves();

	Hero = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	//GetWorldTimerManager().SetTimer(SpawnTimer, this, 
	//	&AEnemyGroupSpawner::Spawn, SpawnFrequency, true, FirstSpawnDelay);

	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("gm is null - ABarracks::BeginPlay"));
		return;
	}


	bIsCurrentWaveFinished = true;
	gm->RegisterEnemySpawner(this);

	//
	PlayDefaultTheme();

}

void AEnemyGroupSpawner::EditWaves()
{
	//AllWaves.SetNum(1);
	//// # Wave 1:
	//AllWaves[0].WaveUnits.SetNum(10);
	//AllWaves[0].WaveUnits[0].SetWaveUnit(LanT0, South_TrooperRoute_0);
	//AllWaves[0].WaveUnits[1].SetWaveUnit(LanT0, South_TrooperRoute_1);
	//AllWaves[0].WaveUnits[2].SetWaveUnit(LanT0_CB, South_ShooterRoute_0);
	//AllWaves[0].WaveUnits[3].SetWaveUnit(LanT0, South_TrooperRoute_0);
	//AllWaves[0].WaveUnits[4].SetWaveUnit(LanT0, South_TrooperRoute_1);
	//AllWaves[0].WaveUnits[5].SetWaveUnit(LanT0_CB, South_ShooterRoute_0);
	//AllWaves[0].WaveUnits[6].SetWaveUnit(LanT0, South_TrooperRoute_0);
	//AllWaves[0].WaveUnits[7].SetWaveUnit(LanT0, South_TrooperRoute_1);
	//AllWaves[0].WaveUnits[8].SetWaveUnit(LanT0_CB, South_ShooterRoute_0);
	//AllWaves[0].WaveUnits[9].SetWaveUnit(LanT0, South_TrooperRoute_1);
}

// Called every frame
void AEnemyGroupSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FTransform AEnemyGroupSpawner::GetNextWayPointFrom(int _pathIndex, int _nextWaypoint) const
{
	return Paths[_pathIndex].WayPoints[_nextWaypoint];
}

void AEnemyGroupSpawner::InitWaveSpawner()
{
	CurrentWaveIndex = 0;

	InitCurrentWave();
}

void AEnemyGroupSpawner::InitCurrentWave()
{
	// if we are given a unvalid wave index, just restart from first wave
	if (AllWaves.IsValidIndex(CurrentWaveIndex) == false)
		CurrentWaveIndex = AllWaves.Num() - 1;

	MaxWaveUnitAmount = AllWaves[CurrentWaveIndex].WaveUnits.Num();
	CurrentWaveUnitIndex = 0;

}

//void AEnemyGroupSpawner::OnWaveBegin(int _waveIndex)
//{
//	// if we are given a unvalid wave index, just restart from first wave
//}
//
//void AEnemyGroupSpawner::OnWaveEnd(int _waveIndex)
//{
//}

void AEnemyGroupSpawner::OnSpawnFinished()
{
	// play warning sound if we have
	USoundCue* warningSound = AllWaves[CurrentWaveIndex].WaveUnits[CurrentWaveUnitIndex].WarningSound;
	if (warningSound)
		UGameplayStatics::SpawnSoundAttached(warningSound, Hero->GetMesh());


	CurrentWaveUnitIndex++;

	// if the current wave is finished spawning
	if (CurrentWaveUnitIndex >= MaxWaveUnitAmount)
	{
		bIsCurrentWaveFinished = true;
		CurrentWaveIndex++;
		return;
	}

	// update the spawn delay, setup the timer for next spawn
	SpawnDelay = AllWaves[CurrentWaveIndex].WaveUnits[CurrentWaveUnitIndex].SpawnDelay;
	GetWorldTimerManager().ClearTimer(SpawnTimer);
	GetWorldTimerManager().SetTimer(SpawnTimer, this,
		&AEnemyGroupSpawner::Spawn, 0.1f, false, SpawnDelay);



}

void AEnemyGroupSpawner::FindAllEnemyGroupPreset()
{
	//UCustomType::FindClass<AEnemyGroup>(LanT0,
	//	TEXT("/Game/Blueprints/AI/GroupPreset/EnemyGroupPreset/LanT0_Bp"));

	//UCustomType::FindClass<AEnemyGroup>(LanT0_CB,
	//	TEXT("/Game/Blueprints/AI/GroupPreset/EnemyGroupPreset/LanT0_CB_Bp"));

}

void AEnemyGroupSpawner::Spawn()
{
	if (!bEnableSpawning)
		return;

	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>
		(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm == nullptr)
	{
		UE_LOG(LogTemp, Error,
			TEXT("gm = nullptr - AEnemyGroupSpawner::Spawn"));
		return;
	}

	bool HasRoomToSpawn = (bTestingMode) ? 
		gm->GetEnemyGroupAmount() < TestGroupAmount : true;

	if (HasRoomToSpawn)
	{
		FActorSpawnParameters spawnParam;
		spawnParam.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		/** Get What to spawn, and path */		

		TSubclassOf<AEnemyGroup> classToSpawn; 
		int totalNumber, maxColumn;
		EPath pathIndex;


		if (bTestingMode)
		{
			classToSpawn = TestGroup;
			pathIndex = TestingRoute;
			totalNumber = TestGroupSize;
			maxColumn = TestGroupSize;
		}
		else
		{
			FWaveUnit waveUnit = AllWaves[CurrentWaveIndex].WaveUnits[CurrentWaveUnitIndex];
			classToSpawn = waveUnit.GroupClass_Bp;
			pathIndex = waveUnit.Path;
			totalNumber = waveUnit.SpawnTotalNumber;;
			maxColumn = waveUnit.SpawnMaxColumn;;
		}


		/** Spawn Group*/
		FQuat spawnRotation;
		FVector spawnLocation;
		GetSpawnTransform(spawnLocation, spawnRotation, (int)pathIndex);

		FTransform spawnTransform;
		spawnTransform.SetLocation(spawnLocation);
		spawnTransform.SetRotation(spawnRotation);
		spawnTransform.SetScale3D(FVector(1, 1, 1));
		
		// spawn new group
		AEnemyGroup* newEnemyGroup
			= GetWorld()->SpawnActorDeferred<AEnemyGroup>
			(classToSpawn, spawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (newEnemyGroup)
		{
			FAISpawnInfo spawnInfo;

			newEnemyGroup->SetSpawnInfoAtSection_TotalNum(totalNumber, 0);
			newEnemyGroup->SetSpawnInfoAtSection_MaxCol(maxColumn, 0);
			newEnemyGroup->SetPath((int)pathIndex);

			UGameplayStatics::FinishSpawningActor(newEnemyGroup, spawnTransform);
			newEnemyGroup->SpawnDefaultController();

			gm->RegisterEnemyGroup(newEnemyGroup);

			if (bTestingMode == false)
				OnSpawnFinished();
		}

	}
}

void AEnemyGroupSpawner::GetSpawnTransform(FVector& _location, FQuat& _rotation, int _pathIndex) const
{
	//_pathIndex = 0;// FMath::RandRange(0, Paths.Num() - 1);
	_location =  this->GetActorLocation() + Paths[_pathIndex].WayPoints[0].GetLocation();

	_rotation = Paths[_pathIndex].WayPoints[0].GetRotation();
	
	//FVector forward = Paths[spawnSelection].WayPoints[0].GetUnitAxis(EAxis::Type::X);
	//UE_LOG(LogTemp, Log, TEXT("forward: %f, %f, %f -- AEnemyGroupSpawner::SelectedPath"), 
	//	forward.X, forward.Y, forward.Z );
}


void AEnemyGroupSpawner::EnableSpawning()
{
	// Turn off the text notification
	ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (pc)
		pc->GetInGameHUD()->SetStartWaveNotificationVisibility(false);

	bIsCurrentWaveFinished = false;

	InitCurrentWave();

	if (AllWaves.IsValidIndex(CurrentWaveIndex) && AllWaves[CurrentWaveIndex].WaveUnits.IsValidIndex(0))
	{
		SpawnDelay = AllWaves[CurrentWaveIndex].WaveUnits[CurrentWaveUnitIndex].SpawnDelay;
		GetWorldTimerManager().ClearTimer(SpawnTimer);
		GetWorldTimerManager().SetTimer(SpawnTimer, this,
			&AEnemyGroupSpawner::Spawn, 0.1f, false, SpawnDelay);
		
		// Fade In BGM
		if (CurrentWaveIndex >= LannisterFirstWave && CurrentWaveIndex < LannisterMiddleWave)
			PlayLannisterFirstTheme();

		else if (CurrentWaveIndex >= LannisterMiddleWave && CurrentWaveIndex <= LannisterLastWave)
			PlayLannisterSecondTheme();

		else if (CurrentWaveIndex >= WhiteWalkerFirstWave && CurrentWaveIndex <= WhiteWalkerLastWave)
			PlayWhiteWalkerTheme();

	}

}

void AEnemyGroupSpawner::PlayDefaultTheme()
{   
	if (MusicPlayer)
	{
		// Check if music play is playing another song
		if (MusicPlayer->IsPlaying() == true)
		{   
			float FadeOutTime = 5.0f;
			MusicPlayer->FadeOut(FadeOutTime, 0.0f);

			// After FadeOut, FadeIn new music
			GetWorldTimerManager().ClearTimer(MusicFadeInTimer);
			GetWorldTimerManager().SetTimer(MusicFadeInTimer, this, &AEnemyGroupSpawner::FadeInDefaultTheme, 1.0F, false, FadeOutTime);
		}

		else
			FadeInDefaultTheme();

	}
}

void AEnemyGroupSpawner::PlayLannisterFirstTheme()
{
	if (MusicPlayer)
	{
		// Check if music play is playing another song
		if (MusicPlayer->IsPlaying() == true)
		{
			float FadeOutTime = 5.0f;
			MusicPlayer->FadeOut(FadeOutTime, 0.0f);

			// After FadeOut, FadeIn new music
			GetWorldTimerManager().ClearTimer(MusicFadeInTimer);
			GetWorldTimerManager().SetTimer(MusicFadeInTimer, this, &AEnemyGroupSpawner::FadeInLannisterTheme1, 1.0F, false, FadeOutTime);
		}
	}
}

void AEnemyGroupSpawner::PlayLannisterSecondTheme()
{
	if (MusicPlayer)
	{
		// Check if music play is playing another song
		if (MusicPlayer->IsPlaying() == true)
		{
			float FadeOutTime = 5.0f;
			MusicPlayer->FadeOut(FadeOutTime, 0.0f);

			// After FadeOut, FadeIn new music
			GetWorldTimerManager().ClearTimer(MusicFadeInTimer);
			GetWorldTimerManager().SetTimer(MusicFadeInTimer, this, &AEnemyGroupSpawner::FadeInLannisterTheme2, 1.0F, false, FadeOutTime);
		}
	}
}

void AEnemyGroupSpawner::PlayWhiteWalkerTheme()
{
	if (MusicPlayer)
	{
		// Check if music play is playing another song
		if (MusicPlayer->IsPlaying() == true)
		{
			float FadeOutTime = 5.0f;
			MusicPlayer->FadeOut(FadeOutTime, 0.0f);

			// After FadeOut, FadeIn new music
			GetWorldTimerManager().ClearTimer(MusicFadeInTimer);
			GetWorldTimerManager().SetTimer(MusicFadeInTimer, this, &AEnemyGroupSpawner::FadeInWhiteWalkerTheme, 0.0F, false, FadeOutTime);
		}
	}
}

void AEnemyGroupSpawner::FadeInDefaultTheme()
{
	// Set Music for Music Player
	MusicPlayer->SetSound(UAudioManager::GetSFX(ESoundEffectType::EDefaultTheme));
	int StartPoint = FMath::RandRange(0, 1);
	if (StartPoint == 0)
		MusicPlayer->FadeIn(10.0f, 1.0f, DefaultTheme_StartPoint_0);
	else
		MusicPlayer->FadeIn(10.0f, 1.0f, DefaultTheme_StartPoint_1);
}

void AEnemyGroupSpawner::FadeInLannisterTheme1()
{
	// Set Music for Music Player
	MusicPlayer->SetSound(UAudioManager::GetSFX(ESoundEffectType::ELannisterTheme1));
	int StartPoint = FMath::RandRange(0, 2);
	if (StartPoint == 0)
		MusicPlayer->FadeIn(10.0f, 1.0f, LannisterTheme1_StartPoint_0);
	else if (StartPoint == 1)
		MusicPlayer->FadeIn(10.0f, 1.0f, LannisterTheme1_StartPoint_1);
	else
		MusicPlayer->FadeIn(10.0f, 1.0f, LannisterTheme1_StartPoint_2);
}

void AEnemyGroupSpawner::FadeInLannisterTheme2()
{
	// Set Music for Music Player
	MusicPlayer->SetSound(UAudioManager::GetSFX(ESoundEffectType::ELannisterTheme2));
	int StartPoint = FMath::RandRange(0, 1);
	if (StartPoint == 0)
		MusicPlayer->FadeIn(10.0f, 1.0f, LannisterTheme2_StartPoint_0);
	else
		MusicPlayer->FadeIn(10.0f, 1.0f, LannisterTheme2_StartPoint_1);
}

void AEnemyGroupSpawner::FadeInWhiteWalkerTheme()
{
	// Set Music for Music Player
	if (CurrentWaveIndex == WhiteWalkerFirstWave)
	{
		MusicPlayer->SetSound(UAudioManager::GetSFX(ESoundEffectType::ESoldierDialog));
		MusicPlayer->FadeIn(2.0f);

		// Setup a delay
		float DialogDuration = 21.0f;
		GetWorldTimerManager().ClearTimer(MusicFadeInTimer);
		GetWorldTimerManager().SetTimer(MusicFadeInTimer, this, &AEnemyGroupSpawner::FirstTimeFadeInWhiteWalkerTheme, 1.0F, false, DialogDuration);

	}
	else
	{
		MusicPlayer->SetSound(UAudioManager::GetSFX(ESoundEffectType::EWhiteWalkerTheme));
		int StartPoint = FMath::RandRange(0, 1);
		if (StartPoint == 0)
			MusicPlayer->FadeIn(10.0f, 1.0f, WhiteWalkerTheme_StartPoint_0);
		else
			MusicPlayer->FadeIn(10.0f, 1.0f, WhiteWalkerTheme_StartPoint_1);
	}
}

void AEnemyGroupSpawner::FirstTimeFadeInWhiteWalkerTheme()
{
	MusicPlayer->SetSound(UAudioManager::GetSFX(ESoundEffectType::EWhiteWalkerTheme));
	MusicPlayer->FadeIn(5.0f, 1.0f, WhiteWalkerTheme_StartPoint_0);
}

