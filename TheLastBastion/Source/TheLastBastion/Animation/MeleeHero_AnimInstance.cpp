// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeHero_AnimInstance.h"




void UMeleeHero_AnimInstance::OnBeginPlay()
{
	Super::OnBeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("UMeleeHero_AnimInstance Call BeginPlay"));

}

void UMeleeHero_AnimInstance::OnInit()
{
	Super::OnInit();
	//UE_LOG(LogTemp, Warning, TEXT("UMeleeHero_AnimInstance Call OnInit"));

}

void UMeleeHero_AnimInstance::OnUpdate(float _deltaTime)
{
	Super::OnUpdate(_deltaTime);
}

void UMeleeHero_AnimInstance::OnPostEvaluate()
{

}

void UMeleeHero_AnimInstance::OnAttack()
{
	Super::OnAttack();
	activatedStateMachine++;
	if (activatedStateMachine == 2)
	{
		activatedStateMachine = 0;
	}
	UE_LOG(LogTemp, Warning, TEXT("Attack ! - UMeleeHero_AnimInstance"));
}
