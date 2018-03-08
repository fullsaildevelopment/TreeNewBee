// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyMeleeGroup.h"
#include "TheLastBastionHeroCharacter.h"
#include "AI/AllyGroup.h"
#include "UObject/ConstructorHelpers.h"

AEnemyMeleeGroup::AEnemyMeleeGroup()
{
	ConstructorHelpers::FObjectFinder<UBehaviorTree> bt(TEXT("/Game/Blueprints/AI/GroupPreset/BT_EnemyMeleeGroupAI"));
	if (bt.Succeeded())
		BehaviorTree = bt.Object;
	else
		UE_LOG(LogTemp, Error, TEXT("Can not find behaviorTree - AEnemyMeleeGroup::AEnemyMeleeGroup"));
}

void AEnemyMeleeGroup::OnMeleeVisionOverrlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// check if this is a hero character
	ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(OtherActor);
	if (hero)
	{
		AddThreat(hero, ThreatGain_HeroInit);
		if (!bInBattle)
		{
			bInBattle = true;
			MeleeGroupAgainstPlayer();
		}
	}
	else
	{
		AAllyGroup* targetGroup = Cast<AAllyGroup>(OtherActor);
		if (targetGroup)
		{
			AddThreatByGroup(targetGroup);
			if (!bInBattle)
			{
				MeleeTargetSelectionOnOverlap(targetGroup);
				bInBattle = true;
			}
		}

	}
}

void AEnemyMeleeGroup::OnMeleeVisionOverrlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}
