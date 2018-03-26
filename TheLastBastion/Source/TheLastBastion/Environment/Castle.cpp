// Fill out your copyright notice in the Description page of Project Settings.

#include "Castle.h"
#include "Components/BoxComponent.h"
#include "TheLastBastionHeroCharacter.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UI/InGameHUD.h"


// Sets default values
ACastle::ACastle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;


	CastleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Castle_Mesh"));
	CastleMesh->SetMobility(EComponentMobility::Static);

	this->SetRootComponent(CastleMesh);

	CastleBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Castle_Volumn"));
	CastleBox->SetupAttachment(CastleMesh);
	CastleBox->bGenerateOverlapEvents = true;
	CastleBox->SetCollisionProfileName(TEXT("Castle"));
	CastleBox->InitBoxExtent(FVector(12000.0f, 12000.0f, 3000.0f));
	CastleBox->SetMobility(EComponentMobility::Static);

	// Init
	
	UpdateFreq = 1.0f;
	MaxHp = 2000.0f;
	CurrentHp = MaxHp;
	DamageReduction = 0.0f;

}

// Called when the game starts or when spawned
void ACastle::BeginPlay()
{
	Super::BeginPlay();

	MaxHpDiv = 1.0f / MaxHp;

	if (CastleBox)
	{
		CastleBox->OnComponentBeginOverlap.AddDynamic(this, &ACastle::OnCastleBoxOverlap_Start);
		CastleBox->OnComponentEndOverlap.AddDynamic(this, &ACastle::OnCastleBoxOverlap_End);
	}

	// Set Health update timer;
	GetWorldTimerManager().SetTimer(HealthTimer, this, &ACastle::UpdateCastleHealth, UpdateFreq, true, 0.1f);
}

// Called every frame
void ACastle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACastle::UpdateCastleHealth()
{

	float damageToCastle = 0.0f;
	bool bGetDamaged = false;
	for (int iEnemies = 0; iEnemies < Enemies.Num(); iEnemies++)
	{
		if (Enemies[iEnemies]->GetIsDead())
		{
			// if this is dead, clear it out of castle enemies list
			Enemies.RemoveAtSwap(iEnemies);
		}
		else
		{
			// if this is lived, add its siege point to the castle damage
			damageToCastle += Enemies[iEnemies]->GetSiegePoint();
			bGetDamaged = true;
		}
	}

	if (bGetDamaged)
	{
		CurrentHp -= (bCommanderPresence) ? damageToCastle * DamageReduction : damageToCastle;
		CurrentHp = FMath::Clamp(CurrentHp, 0.0f, MaxHp);
		float CurrentHpPercentage = CurrentHp * MaxHpDiv;
		// UI Call

		ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (hero)
		{
			UInGameHUD* gameHUD = hero->GetInGameHUD();
			if (gameHUD)
				gameHUD->SetCastleBarValue(CurrentHpPercentage);
		}
	}

	if (CurrentHp == 0)
	{
		// Game Over
	}

	UE_LOG(LogTemp, Log, TEXT("%f Current Hp"), CurrentHp);

}

void ACastle::OnEnemiesEnter(class ATheLastBastionAIBase* _enemy)
{
	Enemies.Add(_enemy);
}

void ACastle::OnEnemiesLeave(ATheLastBastionAIBase * _enemy)
{
	Enemies.RemoveSingleSwap(_enemy);
}

void ACastle::OnCastleBoxOverlap_Start(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ATheLastBastionCharacter* overlapCharacter = Cast<ATheLastBastionCharacter>(OtherActor);
	if (overlapCharacter)
	{
		if (overlapCharacter->IsEnemy())
		{
			ATheLastBastionAIBase* aiCharacter = Cast<ATheLastBastionAIBase>(overlapCharacter);
			if (aiCharacter)
				OnEnemiesEnter(aiCharacter);
		}
		else
		{
			ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(overlapCharacter);
			if (hero)
			{
				bCommanderPresence = true;
				DamageReduction = 1 - hero->GetCommandPresence();
			}
		}
	}
}

void ACastle::OnCastleBoxOverlap_End(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	ATheLastBastionCharacter* overlapCharacter = Cast<ATheLastBastionCharacter>(OtherActor);
	if (overlapCharacter)
	{
		if (overlapCharacter->IsEnemy())
		{
			ATheLastBastionAIBase* aiCharacter = Cast<ATheLastBastionAIBase>(overlapCharacter);
			if (aiCharacter)
				OnEnemiesLeave(aiCharacter);
		}
		else
		{
			ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(overlapCharacter);
			if (hero)
			{
				bCommanderPresence = false;
			}
		}
	}

}

