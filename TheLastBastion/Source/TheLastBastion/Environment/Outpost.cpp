// Fill out your copyright notice in the Description page of Project Settings.

#include "Outpost.h"
#include "Components/BoxComponent.h"
#include "AI/EnemyGroup.h"
#include "Environment/EnemyGroupSpawner.h"
#include "Gamemode/SinglePlayerGM.h"
#include "Kismet/GameplayStatics.h"
#include "AudioManager.h"
#include "Sound/SoundCue.h"



// Sets default values
AOutpost::AOutpost()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	OutpostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Outpost_Mesh"));
	OutpostMesh->SetMobility(EComponentMobility::Static);

	this->SetRootComponent(OutpostMesh);

	OccupyVolumn = CreateDefaultSubobject<UBoxComponent>(TEXT("Occupy_Volumn"));
	OccupyVolumn->SetupAttachment(OutpostMesh);
	OccupyVolumn->bGenerateOverlapEvents = true;
	OccupyVolumn->SetCollisionProfileName(TEXT("Outpost"));
	OccupyVolumn->InitBoxExtent(FVector(12000.0f, 12000.0f, 3000.0f));
	OccupyVolumn->SetMobility(EComponentMobility::Static);

	UpdateFreq = 1.0f;
	bDisabled = false;
	AdditionAmount = 0;
}

// Called when the game starts or when spawned
void AOutpost::BeginPlay()
{
	Super::BeginPlay();
	
	if (OccupyVolumn)
	{
		OccupyVolumn->OnComponentBeginOverlap.AddDynamic(this, 
			&AOutpost::OnOutPostBoxOverlap_Start);
		OccupyVolumn->OnComponentEndOverlap.AddDynamic(this, 
			&AOutpost::OnOutPostBoxOverlap_End);
	}
	
	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("gm == nullptr"));
		return;
	}

	gm->RegisterOutpost(this);

	float delay = FMath::RandRange(0.1f, 3.0f);
	// Set Health update timer;
	GetWorldTimerManager().SetTimer(UpdateTimer, this, &AOutpost::UpdateByTimer, UpdateFreq, true, delay);
}

// Called every frame
void AOutpost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AOutpost::UpdateByTimer()
{


	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("gm == nullptr"));
		return;
	}

	if (bDisabled || gm->GetEnemyGroupSpawner()->IsDuringWait())
		return;

	for (int iGroup = 0; iGroup < EnemiesGroup.Num(); iGroup++)
	{
		// if this is an empty group
		if (EnemiesGroup[iGroup] == nullptr || EnemiesGroup[iGroup]->GetGroupSize() == 0)
		{
			EnemiesGroup.RemoveAtSwap(iGroup);
		}
	}

	if (EnemiesGroup.Num() == 0)
	{
		bIsOccupied = false;
	}

	if (bIsOccupied == false)
	{

		switch (OutpostType)
		{
		case EOutpostType::Food:
			gm->AddFood(FoodIncrement + AdditionAmount * FoodIncrement);
			break;
		case EOutpostType::Metal:
			gm->AddMetal(MetalIncrement + AdditionAmount * MetalIncrement);
			break;
		case EOutpostType::Stone:
			gm->AddStone(StoneIncrement + AdditionAmount * StoneIncrement);
			break;
		case EOutpostType::Wood:
			gm->AddWood(WoodIncrement + AdditionAmount * WoodIncrement);
			break;
		case EOutpostType::None:
		default:
			break;
		}
	}
}

void AOutpost::OnOutPostBoxOverlap_Start(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AEnemyGroup* enemyGroup = Cast<AEnemyGroup>(OtherActor);
	if (enemyGroup)
	{
		EnemiesGroup.Add(enemyGroup);
		bIsOccupied = true;

		// Play Vocal Notification
		USoundCue* VocalWarning = nullptr;
		switch (OutpostType)
		{
		case EOutpostType::Food:
			VocalWarning = UAudioManager::GetSFX(ESoundEffectType::EFoodWarning);
			break;
		case EOutpostType::Metal:
			VocalWarning = UAudioManager::GetSFX(ESoundEffectType::EMineWarning);
			break;
		case EOutpostType::Stone:
			VocalWarning = UAudioManager::GetSFX(ESoundEffectType::EStoneWarning);
			break;
		case EOutpostType::Wood:
			VocalWarning = UAudioManager::GetSFX(ESoundEffectType::EWoodWarning);
			break;
		case EOutpostType::None:
		default:
			break;
		}

		if (VocalWarning)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), VocalWarning, FVector::ZeroVector);
		}

	}

}

void AOutpost::OnOutPostBoxOverlap_End(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{

	AEnemyGroup* enemyGroup = Cast<AEnemyGroup>(OtherActor);
	if (enemyGroup)
	{
		EnemiesGroup.RemoveSingleSwap(enemyGroup);
		if (EnemiesGroup.Num() == 0)
		{
			bIsOccupied = false;
		}
	}
}

