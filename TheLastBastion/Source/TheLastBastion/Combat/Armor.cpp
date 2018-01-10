// Fill out your copyright notice in the Description page of Project Settings.

#include "Armor.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"



AArmor::AArmor() 
{
}

void AArmor::Equip(USkeletalMeshComponent * const _skeletonMeshComponent)
{
	Super::Equip(_skeletonMeshComponent);

	if (BodyMeshRef)
		_skeletonMeshComponent->SetSkeletalMesh(BodyMeshRef);

	for (int i = 0; i < BodyMeshRef->Materials.Num(); i++)
	{
		_skeletonMeshComponent->SetMaterial(i, BodyMeshRef->Materials[i].MaterialInterface);
	}

}
