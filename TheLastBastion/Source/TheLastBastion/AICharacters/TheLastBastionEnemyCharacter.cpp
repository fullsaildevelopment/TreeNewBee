// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionEnemyCharacter.h"
#include "Combat/PawnStatsComponent.h"
#include "Components/CapsuleComponent.h"




ATheLastBastionEnemyCharacter::ATheLastBastionEnemyCharacter() : Super()
{

	GetCapsuleComponent()->SetCollisionProfileName("Enemy");


	EnemyStats = CreateDefaultSubobject<UPawnStatsComponent>(TEXT("Stats"));
	EnemyStats->SetDamageDetectorsCollsionProfile(TEXT("EnemyBody"));
	PawnStats = EnemyStats;

}

void ATheLastBastionEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}
