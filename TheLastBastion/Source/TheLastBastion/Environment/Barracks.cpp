// Fill out your copyright notice in the Description page of Project Settings.

#include "Barracks.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/SinglePlayerGM.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "CustomType.h"
#include "TheLastBastionHeroCharacter.h"

// Sets default values
ABarracks::ABarracks()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barracks_Mesh"));
	SetRootComponent(Mesh);

	InteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Interact_Box"));
	InteractBox->SetupAttachment(Mesh);
	InteractBox->SetCollisionProfileName("InteractBox");
	InteractBox->bGenerateOverlapEvents = true;
	InteractBox->SetCanEverAffectNavigation(false);
	InteractBox->InitBoxExtent(FVector(500, 2000, 200));
}

// Called when the game starts or when spawned
void ABarracks::BeginPlay()
{
	Super::BeginPlay();
	
	// register in game mode
	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("gm is null - ABarracks::BeginPlay"));
		return;
	}

	gm->RegisterBarracks(this);
	
	// Register overlap event
	if (InteractBox)
	{
		InteractBox->OnComponentBeginOverlap.AddDynamic(this, &ABarracks::OnInteractBoxOverlapBegin);
		InteractBox->OnComponentEndOverlap.AddDynamic(this, &ABarracks::OnInteractBoxOverlapEnd);
	}
}

void ABarracks::OnInteractBoxOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ATheLastBastionHeroCharacter* Player = Cast<ATheLastBastionHeroCharacter>(OtherActor);
	if (Player)
	{
		Player->SetRecruitMenuState(true);
	}
}

void ABarracks::OnInteractBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATheLastBastionHeroCharacter* Player = Cast<ATheLastBastionHeroCharacter>(OtherActor);
	if (Player)
	{
		Player->SetRecruitMenuState(false);
	}
}


