// Fill out your copyright notice in the Description page of Project Settings.

#include "RangeHero_AnimInstance.h"


#include "TheLastBastionHeroCharacter.h"
#include "Combat/HeroStatsComponent.h"
#include "Combat/RangeWeapon.h"



void URangeHero_AnimInstance::OnBeginPlay()
{
	Super::OnBeginPlay();
}
void URangeHero_AnimInstance::OnInit()
{
	Super::OnInit();
}

void URangeHero_AnimInstance::OnUpdate(float _deltaTime)
{
	Super::OnUpdate(_deltaTime);
}

void URangeHero_AnimInstance::OnPostEvaluate()
{
	Super::OnPostEvaluate();
}

bool URangeHero_AnimInstance::OnAttack()
{

	//if (mCharacter)
	//{
	//	UHeroStatsComponent* heroStats = mCharacter->GetHeroStatsComp();
	//	if (heroStats)
	//	{
	//		ARangeWeapon* rangeWeapon = Cast<ARangeWeapon>(heroStats->GetRightHandWeapon());

	//		if (rangeWeapon)
	//		{
	//			rangeWeapon->Fire();
	//			return true;
	//		}
	//	}
	//}
	return false;

	
}
