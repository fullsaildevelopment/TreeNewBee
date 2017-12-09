// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeHero_AnimInstance.h"




void UMeleeHero_AnimInstance::OnBeginPlay()
{
	Super::OnBeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("UMeleeHero_AnimInstance Call BeginPlay"));
}

void UMeleeHero_AnimInstance::OnInit()
{
}

void UMeleeHero_AnimInstance::OnUpdate(float _deltaTime)
{
}

void UMeleeHero_AnimInstance::OnPostEvaluate()
{

}
