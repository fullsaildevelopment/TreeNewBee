// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyMeleeGroup.h"
#include "TheLastBastionHeroCharacter.h"
#include "AI/AllyGroup.h"


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
