// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyGroupSpawner.h"
#include "CustomType.h"
#include "AI/EnemyGroup.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/SinglePlayerGM.h"
#include "Sound/SoundCue.h"
#include "TheLastBastionHeroCharacter.h"
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
	TestingSpawnFreq = 5.0f;
	TestGroupAmount = 2;
	TestGroupSize = 2;
	TestMaxCol = 2;
	TestingRoute = EPath::South_CastleSouth_0;

	// initialize default spawning state
	bEnableSpawning = false;
	bVictory = false;
 
	bIsCurrentWaveFinishSpawning = true;
	StartWaveIndex = 0;
	CurrentWaveIndex = 0;
}

// Called when the game starts or when spawned
void AEnemyGroupSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	//EditWaves();
	CurrentWaveIndex = StartWaveIndex;

	Hero = Cast<ATheLastBastionHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Hero == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Hero is null - AEnemyGroupSpawner::BeginPlay"));
		return;

	}
	//GetWorldTimerManager().SetTimer(SpawnTimer, this, 
	//	&AEnemyGroupSpawner::Spawn, SpawnFrequency, true, FirstSpawnDelay);

	SpGameMode = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (SpGameMode == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SpGameMode is null - AEnemyGroupSpawner::BeginPlay"));
		return;
	}

	bIsCurrentWaveFinishSpawning = true;
	SpGameMode->RegisterEnemySpawner(this);

	//
	//PlayDefaultTheme();

	PlayWaitBGM();

}

void AEnemyGroupSpawner::EditWaves()
{
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

bool AEnemyGroupSpawner::IsDuringWait() const
{
	return (Hero)? Hero->GetCanStartNextWave() : true;
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
		CurrentWaveIndex = 0;

	MaxWaveUnitAmount = AllWaves[CurrentWaveIndex].WaveUnits.Num();
	CurrentWaveUnitIndex = 0;

}

void AEnemyGroupSpawner::OnSpawnFinished()
{
	// stop the spawning if the catsle is destroy
	if (SpGameMode->IsCastleDestroy())
		return;

	// play warning sound if we have
	USoundCue* warningSound = AllWaves[CurrentWaveIndex].WaveUnits[CurrentWaveUnitIndex].WarningSound;
	if (warningSound)
		UGameplayStatics::SpawnSoundAttached(warningSound, Hero->GetMesh());


	CurrentWaveUnitIndex++;

	// if the current wave is finished spawning
	if (CurrentWaveUnitIndex >= MaxWaveUnitAmount)
	{
		bIsCurrentWaveFinishSpawning = true;
		CurrentWaveIndex++;
		if (CurrentWaveIndex > WhiteWalkerWave)
			bVictory = true;
		return;
	}

	// update the spawn delay, setup the timer for next spawn, if the castle is alive
	SpawnDelay = AllWaves[CurrentWaveIndex].WaveUnits[CurrentWaveUnitIndex].SpawnDelay;
	GetWorldTimerManager().ClearTimer(SpawnTimer);
	GetWorldTimerManager().SetTimer(SpawnTimer, this,
		&AEnemyGroupSpawner::Spawn, 0.1f, false, SpawnDelay);

}

void AEnemyGroupSpawner::FindAllEnemyGroupPreset()
{
}

void AEnemyGroupSpawner::Spawn()
{
	if (!bEnableSpawning)
		return;

	//ASinglePlayerGM* gm = Cast<ASinglePlayerGM>
	//	(UGameplayStatics::GetGameMode(GetWorld()));
	//if (gm == nullptr)
	//{
	//	UE_LOG(LogTemp, Error,
	//		TEXT("gm = nullptr - AEnemyGroupSpawner::Spawn"));
	//	return;
	//}

	bool HasRoomToSpawn = (bTestingMode) ? 
		SpGameMode->GetEnemyGroupAmount() < TestGroupAmount : true;

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
			maxColumn = TestMaxCol;
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

			SpGameMode->RegisterEnemyGroup(newEnemyGroup);

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
		pc->GetInGameHUD()->SetWaveNotificationOnWave(CurrentWaveIndex);

	bIsCurrentWaveFinishSpawning = false;

	InitCurrentWave();

	if (AllWaves.IsValidIndex(CurrentWaveIndex) && AllWaves[CurrentWaveIndex].WaveUnits.IsValidIndex(0))
	{
		SpawnDelay = AllWaves[CurrentWaveIndex].WaveUnits[CurrentWaveUnitIndex].SpawnDelay;
		GetWorldTimerManager().ClearTimer(SpawnTimer);

		if (bTestingMode)
			GetWorldTimerManager().SetTimer(SpawnTimer, this,
				&AEnemyGroupSpawner::Spawn, TestingSpawnFreq, true, 5.0f);
		else
			GetWorldTimerManager().SetTimer(SpawnTimer, this,
				&AEnemyGroupSpawner::Spawn, 0.1f, false, SpawnDelay);
		
		// Fade In BGM


		if (CurrentWaveIndex == WhiteWalkerWave)
		{
			MusicPlayer->SetSound(UAudioManager::GetSFX(ESoundEffectType::EFinalBattleDialog));
			MusicPlayer->FadeIn(FinalBattleDialog_FadeInTime);
			
			// Setup a delay
			GetWorldTimerManager().ClearTimer(MusicFadeInTimer);
			GetWorldTimerManager().SetTimer(MusicFadeInTimer, this, &AEnemyGroupSpawner::PlayBattleBGM, 1.0F, false, FinalBattleDialogDuration);

		}
		else
			PlayBattleBGM();

		//if (CurrentWaveIndex >= LannisterFirstWave && CurrentWaveIndex < LannisterMiddleWave)
		//	PlayLannisterFirstTheme();
		//else if (CurrentWaveIndex >= LannisterMiddleWave && CurrentWaveIndex <= LannisterLastWave)
		//	PlayLannisterSecondTheme();
		//else if (CurrentWaveIndex >= WhiteWalkerFirstWave && CurrentWaveIndex <= WhiteWalkerLastWave)
		//	PlayWhiteWalkerTheme();

	}

}




//void AEnemyGroupSpawner::PlayDefaultTheme()
//{   
//	if (MusicPlayer)
//	{
//		// Check if music play is playing another song
//		if (MusicPlayer->IsPlaying() == true)
//		{   
//			float FadeOutTime = 5.0f;
//			MusicPlayer->FadeOut(FadeOutTime, 0.0f);
//
//			// After FadeOut, FadeIn new music
//			GetWorldTimerManager().ClearTimer(MusicFadeInTimer);
//			GetWorldTimerManager().SetTimer(MusicFadeInTimer, this, &AEnemyGroupSpawner::FadeInDefaultTheme, 1.0f, false, FadeOutTime);
//		}
//
//		else
//			FadeInDefaultTheme();
//
//	}
//}
//
//void AEnemyGroupSpawner::PlayLannisterFirstTheme()
//{
//	if (MusicPlayer)
//	{
//		// Check if music play is playing another song
//		if (MusicPlayer->IsPlaying() == true)
//		{
//			float FadeOutTime = 5.0f;
//			MusicPlayer->FadeOut(FadeOutTime, 0.0f);
//
//			// After FadeOut, FadeIn new music
//			GetWorldTimerManager().ClearTimer(MusicFadeInTimer);
//			GetWorldTimerManager().SetTimer(MusicFadeInTimer, this, &AEnemyGroupSpawner::FadeInLannisterTheme1, 1.0F, false, FadeOutTime);
//		}
//	}
//}
//
//void AEnemyGroupSpawner::PlayLannisterSecondTheme()
//{
//	if (MusicPlayer)
//	{
//		// Check if music play is playing another song
//		if (MusicPlayer->IsPlaying() == true)
//		{
//			float FadeOutTime = 5.0f;
//			MusicPlayer->FadeOut(FadeOutTime, 0.0f);
//
//			// After FadeOut, FadeIn new music
//			GetWorldTimerManager().ClearTimer(MusicFadeInTimer);
//			GetWorldTimerManager().SetTimer(MusicFadeInTimer, this, &AEnemyGroupSpawner::FadeInLannisterTheme2, 1.0F, false, FadeOutTime);
//		}
//	}
//}
//
//void AEnemyGroupSpawner::PlayWhiteWalkerTheme()
//{
//	if (MusicPlayer)
//	{
//		// Check if music play is playing another song
//		if (MusicPlayer->IsPlaying() == true)
//		{
//			float FadeOutTime = 5.0f;
//			MusicPlayer->FadeOut(FadeOutTime, 0.0f);
//
//			// After FadeOut, FadeIn new music
//			GetWorldTimerManager().ClearTimer(MusicFadeInTimer);
//			GetWorldTimerManager().SetTimer(MusicFadeInTimer, this, &AEnemyGroupSpawner::FadeInWhiteWalkerTheme, 0.0F, false, FadeOutTime);
//		}
//	}
//}
//
void AEnemyGroupSpawner::PlayBGM(bool _duringWait)
{
	if (MusicPlayer)
	{

		CurrentBGM = (_duringWait) ? AllWaves[CurrentWaveIndex].WaitingBGM : AllWaves[CurrentWaveIndex].BattleBGM;


		if (MusicPlayer->IsPlaying())
			BGMFadeOut();
		else
		{
			if (CurrentBGM)
				BGMFadeIn();
		}
	}
}

void AEnemyGroupSpawner::BGMFadeIn()
{

	MusicPlayer->SetSound(CurrentBGM);
	MusicPlayer->FadeIn(BGM_FadeInTime, 1.0f);
}

void AEnemyGroupSpawner::BGMFadeOut()
{
	MusicPlayer->FadeOut(BGM_FadeOutTime, 0.0f);

	// After FadeOut, FadeIn new music
	GetWorldTimerManager().ClearTimer(MusicFadeInTimer);
	GetWorldTimerManager().SetTimer(MusicFadeInTimer, this, &AEnemyGroupSpawner::BGMFadeIn, 0.1f, false, BGM_FadeOutTime);
}



