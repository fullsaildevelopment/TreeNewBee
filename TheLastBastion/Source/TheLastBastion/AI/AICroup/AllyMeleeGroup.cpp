// Fill out your copyright notice in the Description page of Project Settings.

#include "AllyMeleeGroup.h"
#include "AI/EnemyGroup.h"



void AAllyMeleeGroup::OnMeleeVisionOverrlapBegin(UPrimitiveComponent * OverlappedComponent,
	AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

	// if this group is not in battle, 
	// it can choose target will it overlap with other group
	AEnemyGroup* targetGroup = Cast<AEnemyGroup>(OtherActor);
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

void AAllyMeleeGroup::OnMeleeVisionOverrlapEnd(UPrimitiveComponent * OverlappedComponent,
	AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	//AEnemyGroup* targetGroup = Cast<AEnemyGroup>(OtherActor);
	//if (targetGroup)
	//{
	//	RemoveThreatByGroup(targetGroup);
	//}

}
