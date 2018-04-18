// Fill out your copyright notice in the Description page of Project Settings.

#include "BlackSmith.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "TheLastBastionHeroCharacter.h"

// Sets default values
ABlackSmith::ABlackSmith()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GearShop_Mesh"));
	SetRootComponent(Mesh);

	InteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Interact_Box"));
	InteractBox->SetupAttachment(Mesh);
	InteractBox->SetCollisionProfileName("InteractBox");
	InteractBox->bGenerateOverlapEvents = true;
	InteractBox->SetCanEverAffectNavigation(false);
	InteractBox->InitBoxExtent(FVector(500, 500, 500));
}

// Called when the game starts or when spawned
void ABlackSmith::BeginPlay()
{
	Super::BeginPlay();
	
	// Register overlap event
	if (InteractBox)
	{
		InteractBox->OnComponentBeginOverlap.AddDynamic(this, &ABlackSmith::OnInteractBoxOverlapBegin);
		InteractBox->OnComponentEndOverlap.AddDynamic(this, &ABlackSmith::OnInteractBoxOverlapEnd);
	}
}

void ABlackSmith::OnInteractBoxOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ATheLastBastionHeroCharacter* Player = Cast<ATheLastBastionHeroCharacter>(OtherActor);
	if (Player)
	{
		Player->SetTradeMenuState(true);
	}
}

void ABlackSmith::OnInteractBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATheLastBastionHeroCharacter* Player = Cast<ATheLastBastionHeroCharacter>(OtherActor);
	if (Player)
	{
		Player->SetTradeMenuState(false);
	}
}


