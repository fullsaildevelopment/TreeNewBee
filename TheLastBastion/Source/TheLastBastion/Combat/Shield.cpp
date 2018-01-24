// Fill out your copyright notice in the Description page of Project Settings.

#include "Shield.h"
#include "Components/BoxComponent.h"



AShield::AShield()
{
	ShieldBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ShieldBox"));
	ShieldBox->SetupAttachment(RootComponent);
	ShieldBox->SetBoxExtent(FVector(40, 40, 40));
	ShieldBox->SetRelativeScale3D(FVector(0.2f, 1.0f, 1.0f));
	ShieldBox->SetCollisionProfileName(TEXT("Shield"));
}
