// Fill out your copyright notice in the Description page of Project Settings.

#include "Castle.h"
#include "Components/BoxComponent.h"
#include "TheLastBastionHeroCharacter.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "AI/EnemyGroup.h"
#include "AIGroupBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/InGameHUD.h"
#include "GameMode/SinglePlayerGM.h"
#include "Environment/EnemyGroupSpawner.h"


#define CastleBarColor_Healthy         FLinearColor(0.049797f,0.280313f,1.000000f,1.000000f)
#define CastleBarColor_UnderAttack     FLinearColor(1.000000f,0.000000f,0.049108f,1.000000f)

// Sets default values
ACastle::ACastle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	MaxHp = 2000.0f;
	CurrentHp = MaxHp;
	DamageReduction = 0.0f;
	OutpostType = EOutpostType::Castle;
	bIsCastleDestory = false;

}

// Called when the game starts or when spawned
void ACastle::BeginPlay()
{
	Super::BeginPlay();

	MaxHpDiv = 1.0f / MaxHp;


	SpGameMode = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (SpGameMode == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SpGameMode == nullptr"));
		return;
	}

	SpGameMode->RegisterCastle(this);

	ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (hero)
	{
		GameHUD = hero->GetInGameHUD();
		if (GameHUD == nullptr)
			UE_LOG(LogTemp, Error, TEXT("GameHUD == nullptr, ACastle::BeginPlay"));
	}
}

// Called every frame
void ACastle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACastle::FixWall(ASinglePlayerGM * _gm, bool _forFree)
{

	if (_forFree)
	{
		CurrentHp = MaxHp;
		GameHUD->SetCastleBarValue(1.0f);
		return;
	}


	int stone = _gm->GetStoneTotal();

	// stone expect to fix castle
	int stoneExpected = (MaxHp - CurrentHp);

	int stoneCost = (stone > stoneExpected) ? stoneExpected : stone;
	_gm->AddStone(-stoneCost);

	CurrentHp += stoneCost;

	// update UI
	float CurrentHpPercentage = CurrentHp * MaxHpDiv;

	GameHUD->SetCastleBarValue(CurrentHpPercentage);

	//ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	//if (hero)
	//{
	//	UInGameHUD* gameHUD = hero->GetInGameHUD();
	//	if (gameHUD)
	//}

}

void ACastle::SetIsOccupied(bool _val)
{
	bIsOccupied = _val;
	GameHUD->SetCastleBarColor((bIsOccupied) ? CastleBarColor_UnderAttack : CastleBarColor_Healthy);
}

void ACastle::OnCastleDestroy()
{
	if (SpGameMode->GetEnemyGroupSpawner()->GetCurrentWaveIndex() == WhiteWalkerWave)
		return;
	bIsCastleDestory = true;
	GameHUD->SetCastleDestoryNotification();
}

void ACastle::UpdateByTimer()
{

	float damageToCastle = 0.0f;

	AAIGroupBase* currentEnemyGroup = nullptr;
	ATheLastBastionAIBase* currentEnemy = nullptr;
	int currentGroupSize = 0;


	if (EnemiesGroup.Num() == 0)
	{
		SetIsOccupied(false);
		return;
	}


	for (int iGroup = 0; iGroup < EnemiesGroup.Num(); iGroup++)
	{
		// first get the group member out of group, if the group is not empty
		currentEnemyGroup = EnemiesGroup[iGroup];
		if (currentEnemyGroup)
		{
			currentGroupSize = currentEnemyGroup->GetGroupSize();
			// stack up the siege point by group member to damage castle
			for (int iEnemies = 0; iEnemies < currentGroupSize; iEnemies++)
			{
				currentEnemy = currentEnemyGroup->GetGroupMemberAt(iEnemies);
				if (currentEnemy && currentEnemy->GetIsDead() == false)
				{
					damageToCastle += currentEnemy->GetSiegePoint();
					SetIsOccupied(true);
					//bIsOccupied = true;
				}
			}
		}
		else
		{
			// if this is empty group, we will remove it
			EnemiesGroup.RemoveAtSwap(iGroup);
		}

	}



	if (bIsOccupied)
	{
		CurrentHp -= (bCommanderPresence) ? damageToCastle * DamageReduction : damageToCastle;
		CurrentHp = FMath::Clamp(CurrentHp, 0.0f, MaxHp);
		float CurrentHpPercentage = CurrentHp * MaxHpDiv;
		// UI Call

		GameHUD->SetCastleBarValue(CurrentHpPercentage);
		//ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		//if (hero)
		//{
		//	UInGameHUD* gameHUD = hero->GetInGameHUD();
		//	if (gameHUD)
		//		gameHUD->SetCastleBarValue(CurrentHpPercentage);
		//}
	}

	if (CurrentHp == 0)
	{
		// Game Over
		OnCastleDestroy();
	}

	//UE_LOG(LogTemp, Log, TEXT("%f Current Hp"), CurrentHp);

}


void ACastle::OnOutPostBoxOverlap_Start(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(OtherActor);
	if (hero)
	{
		bCommanderPresence = true;
		DamageReduction = 1 - hero->GetCommandPresence();
	}
	else
	{

		Super::OnOutPostBoxOverlap_Start(OverlappedComponent, 
			OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
		//AEnemyGroup* enemyGroup = Cast<AEnemyGroup>(OtherActor);
		//if (enemyGroup)
		//{
		//	EnemiesGroup.Add(enemyGroup);
		//}
	}

}

void ACastle::OnOutPostBoxOverlap_End(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{

	ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(OtherActor);
	if (hero)
	{
		bCommanderPresence = false;
	}
	else
	{

		Super::OnOutPostBoxOverlap_End(OverlappedComponent, 
			OtherActor, OtherComp, OtherBodyIndex);

		//AEnemyGroup* enemyGroup = Cast<AEnemyGroup>(OtherActor);
		//if (enemyGroup)
		//{
		//	EnemiesGroup.RemoveSingleSwap(enemyGroup);
		//}
	}

}

