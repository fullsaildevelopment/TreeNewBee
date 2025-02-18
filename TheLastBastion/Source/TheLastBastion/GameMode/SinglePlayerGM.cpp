// Fill out your copyright notice in the Description page of Project Settings.

#include "SinglePlayerGM.h"
#include "PCs/SinglePlayerPC.h"
#include "CustomType.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/SpawnLocation.h"
#include "TimerManager.h"
#include "TheLastBastionHeroCharacter.h"
#include "AI/AllyGroup.h"
#include "AudioManager.h"
#include "Environment/Barracks.h"
#include "Environment/EnemyGroupSpawner.h"
#include "Environment/Castle.h"

#include "UI/InGameHUD.h"
#include "UI/Gameplay/TradeMenu.h"
#include "UI/Gameplay/RecruitMenu.h"


ASinglePlayerGM::ASinglePlayerGM(const FObjectInitializer & _objectInitilizer) : Super(_objectInitilizer)
{
	UAudioManager* audioManager = NewObject<UAudioManager>(UAudioManager::StaticClass());
	if (audioManager == nullptr)
		UE_LOG(LogTemp, Error, TEXT("audioManager"));

	PlayerControllerClass = ASinglePlayerPC::StaticClass();
	UCustomType::FindClass<APawn>(DefaultPawnClass, TEXT("/Game/Blueprints/Heros/SinglePlayer_Bp"));
	bUseSeamlessTravel = true;
	
	GetAllSpawnClass();

	// Change the tick rate
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 2.0f;

	Wood = 500.0f;
	Food = 500.0f;
	Metal = 500.0f;
	Stone = 500.0f;
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

	Allies.SetNum(AllyGroupMaxAmount);

	HeroPC = Cast<ASinglePlayerPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (HeroPC == nullptr)
		UE_LOG(LogTemp, Error, TEXT("HeroPC == nullptr - ASinglePlayerGM::BeginPlay"));

}

void ASinglePlayerGM::GetAllSpawnClass()
{

	if (AllyMeleeGroup_Bp == nullptr)
		UCustomType::FindClass<AAllyGroup>(AllyMeleeGroup_Bp, TEXT("/Game/Blueprints/AI/GroupPreset/AllyMeleeGroup_Bp"));

	if (AllyRangeGroup_Bp == nullptr)
		UCustomType::FindClass<AAllyGroup>(AllyRangeGroup_Bp, TEXT("/Game/Blueprints/AI/GroupPreset/AllyRangeGroup_Bp"));

}


void ASinglePlayerGM::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//CheckWaveState();
}

void ASinglePlayerGM::OnHeroLevelUp(int _heroLevel)
{
	for (size_t i = 0; i < Allies.Num(); i++)
	{
		if (Allies[i] != nullptr)
			Allies[i]->OnHeroLevelUp(_heroLevel);
	}
}

bool ASinglePlayerGM::HasAllyGroupUnitAt(int _index)
{
	return 	Allies.IsValidIndex(_index);
}

bool ASinglePlayerGM::IsDuringWait() const
{
	ATheLastBastionHeroCharacter* Hero = Cast<ATheLastBastionHeroCharacter>(HeroPC->GetCharacter());
	if (Hero)
		return Hero->GetCanStartNextWave();

	return false;
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

void ASinglePlayerGM::SpawnNewAllies(TSubclassOf<class ATheLastBastionAIBase> _classToSpawn, int _totalNum, int _index, bool _isMeleeUnit)
{
	TSubclassOf<AAllyGroup> groupToSpawn = (_isMeleeUnit) ? AllyMeleeGroup_Bp : AllyRangeGroup_Bp;

	FVector Offset = Barracks->GetSpawnLocationOffsetAt(_index);
	FVector groupSpawnLocation = Offset.X *  Barracks ->GetActorForwardVector() +
		Offset.Y * Barracks->GetActorRightVector() + Offset.Z * Barracks->GetActorUpVector() + 		
		Barracks->GetActorLocation();

	FRotator groupSpawnRotation = Barracks->GetActorRotation() + FRotator(0, 180, 0);
	
	FTransform groupSpawnTransform;
	groupSpawnTransform.SetLocation(groupSpawnLocation);
	groupSpawnTransform.SetRotation(groupSpawnRotation.Quaternion());
	groupSpawnTransform.SetScale3D(FVector(1, 1, 1));

	// spawn new group
	AAllyGroup* newAllyGroup 
		= GetWorld()->SpawnActorDeferred<AAllyGroup> 
		(groupToSpawn, groupSpawnTransform ,nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	FAISpawnInfo spawnInfo;
	spawnInfo.AIClassBP = _classToSpawn;
	spawnInfo.TotalNumber = _totalNum;
	newAllyGroup->SetSpawnInfo(spawnInfo);

	UGameplayStatics::FinishSpawningActor(newAllyGroup, groupSpawnTransform);
	//newAllyGroup->SpawnDefaultController();

	// register in controlled allies
	Allies[_index] = newAllyGroup;
	newAllyGroup->SetHUDIndex(_index);

	ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (pc)
	{
		UInGameHUD* gameHUD = pc->GetInGameHUD();
		// Add Ally on Radar
		gameHUD->AddAllyGroupIconAt(_index);
		// Add Ally on Crew Bar
		gameHUD->UpdateCrewInfoAt(_index, newAllyGroup->GetGroupSize(), newAllyGroup->GetThumbNail());

	}

	

}

void ASinglePlayerGM::DestroyAllyGroupAt(int _index)
{
	AAllyGroup* GroupToBeKilled = GetAllyGroupUnitAt(_index);
	if (GroupToBeKilled)
	{
		GroupToBeKilled->KillAllChild();
		Allies[_index] = nullptr;
		ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (pc)
			pc->GetInGameHUD()->RemoveAllyIconAt(_index);
	}
}

void ASinglePlayerGM::UnRegisterAllyGroupAt(int _index)
{
	Allies[_index] = nullptr;

	ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (pc)
		pc->GetInGameHUD()->RemoveAllyIconAt(_index);
}

void ASinglePlayerGM::RegisterEnemyGroup(AEnemyGroup * _enemyGroup)
{
	Enemies.Add(_enemyGroup);
	_enemyGroup->SetGroupIndex(Enemies.Num() - 1);

	ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (pc)
		pc->GetInGameHUD()->AddEnemyGroupIcon();
}

void ASinglePlayerGM::UnRegisterEnemyGroupAt(int _index)
{

	/// this is for the enemy group that is not spawn by the game mode
	if (Enemies.IsValidIndex(_index) == false)
	{
		return;
	}

	Enemies.RemoveAtSwap(_index);

	HeroPC->GetInGameHUD()->RemoveEnemyGroupAt(_index);
	//ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//if (pc)
	//	pc->GetInGameHUD()->RemoveEnemyGroupAt(_index);
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("pc == nullptr, ASinglePlayerGM::UnRegisterEnemyGroupAt"));
	//	return;
	//}


	// check if player survive the current wave
	if (Enemies.Num() <= 0)
	{	
		OnEnemiesGroupAllDied();
		return;
	}




	// re - index each enemy group
	for (int iEnemyGroup = 0; iEnemyGroup < Enemies.Num(); iEnemyGroup++)
	{
		Enemies[iEnemyGroup]->SetGroupIndex(iEnemyGroup);
	}

}

void ASinglePlayerGM::OnEnemiesGroupAllDied()
{
	if (Castle->GetCastleDestroy())
	{
		HeroPC->GetInGameHUD()->SetWaveNotificationOnWait();
		// Fade in wait BGM
		EnemyGroupSpawner->PlayWaitBGM();

		ATheLastBastionHeroCharacter* Hero = Cast<ATheLastBastionHeroCharacter>(HeroPC->GetCharacter());
		if (Hero)
			Hero->SetCanStartNextWave(true);

		Castle->FixWall(this, true);
	}
	else if (EnemyGroupSpawner->GetIsVictory())
	{
		// TODO:  Display the notification for player once all the enemy groups in this wave get killed
		HeroPC->GetInGameHUD()->SetVictoryNotification();
		EnemyGroupSpawner->PlayWaitBGM();

	}
	else if (EnemyGroupSpawner->IsCurrentWaveFinishSpawning())
	{
		// TODO:  Display the notification for player once all the enemy groups in this wave get killed
		HeroPC->GetInGameHUD()->SetWaveNotificationOnWait();

		// Fade in wait BGM
		EnemyGroupSpawner->PlayWaitBGM();

		ATheLastBastionHeroCharacter* Hero = Cast<ATheLastBastionHeroCharacter>(HeroPC->GetCharacter());
		if (Hero)
			Hero->SetCanStartNextWave(true);

		Castle->FixWall(this);
	}
}


void ASinglePlayerGM::OnTradeMenuAccept(int _metal, int _wood)
{
	Metal = _metal;
	Wood = _wood;
	if (HeroPC)
	{
		HeroPC->GetInGameHUD()->SetMetalValue(Metal);
		HeroPC->GetInGameHUD()->SetWoodValue(Wood);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HeroPC == nullptr, ASinglePlayerGM::OnTradeMenuSuccess"));
	}

}

void ASinglePlayerGM::OnRecruitMenuAccept(int _food, int _metal, int _wood)
{
	Food = _food;
	Metal = _metal;
	Wood = _wood;
	if (HeroPC)
	{
		HeroPC->GetInGameHUD()->SetMetalValue(Metal);
		HeroPC->GetInGameHUD()->SetWoodValue(Wood);
		HeroPC->GetInGameHUD()->SetFoodValue(Food);

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HeroPC == nullptr, ASinglePlayerGM::OnRecruitMenuAccept"));
	}

}

bool ASinglePlayerGM::IsCastleDestroy() const
{
	return Castle->GetCastleDestroy();
}

void ASinglePlayerGM::AddFood(int _val)
{
	Food += _val;
	if (HeroPC)
	{
		HeroPC->GetInGameHUD()->SetFoodValue(Food);
		UTradeMenu* tradeMenu = HeroPC->GetTradeMenu();

		if (tradeMenu && tradeMenu->IsOpened())
			tradeMenu->AddFood(_val);
		else
		{
			URecruitMenu* recruitMenu = HeroPC->GetRecruitMenu();
			if (recruitMenu && recruitMenu->IsOpened())
				recruitMenu->AddFood(_val);
		}

	}
}

void ASinglePlayerGM::AddMetal(int _val)
{
	Metal += _val;
	if (HeroPC)
	{
		HeroPC->GetInGameHUD()->SetMetalValue(Metal);
		UTradeMenu* tradeMenu = HeroPC->GetTradeMenu();
		if (tradeMenu && tradeMenu->IsOpened())
			tradeMenu->AddMetal(_val);
		else
		{
			URecruitMenu* recruitMenu = HeroPC->GetRecruitMenu();
			if (recruitMenu && recruitMenu->IsOpened())
				recruitMenu->AddMetal(_val);
		}

	}
}

void ASinglePlayerGM::AddWood(int _val)
{
	Wood += _val;
	if (HeroPC)
	{
		HeroPC->GetInGameHUD()->SetWoodValue(Wood);
		UTradeMenu* tradeMenu = HeroPC->GetTradeMenu();
		if (tradeMenu && tradeMenu->IsOpened())
			tradeMenu->AddWood(_val);
		else
		{
			URecruitMenu* recruitMenu = HeroPC->GetRecruitMenu();
			if (recruitMenu && recruitMenu->IsOpened())
				recruitMenu->AddWood(_val);
		}


	}
}

void ASinglePlayerGM::AddStone(int _val)
{
	Stone += _val;
	if (HeroPC)
	{
		HeroPC->GetInGameHUD()->SetStoneValue(Stone);
		UTradeMenu* tradeMenu = HeroPC->GetTradeMenu();
		if (tradeMenu && tradeMenu->IsOpened())
			tradeMenu->AddStone(_val);
		else
		{
			URecruitMenu* recruitMenu = HeroPC->GetRecruitMenu();
			if (recruitMenu && recruitMenu->IsOpened())
				recruitMenu->AddStone(_val);
		}

	}
}


