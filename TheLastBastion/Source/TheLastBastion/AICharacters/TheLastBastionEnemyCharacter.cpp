// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionEnemyCharacter.h"
#include "Combat/PawnStatsComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AIBase_AnimInstance.h"



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
	mAnimInstanceRef = Cast<UAIBase_AnimInstance>(this->GetMesh()->GetAnimInstance());
	if (mAnimInstanceRef == nullptr) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("ATheLastBastionEnemyCharacter can not take other AnimInstance other than AIBase_AnimInstance, - ATheLastBastionCharacter")); 
		return; 
	}

}
