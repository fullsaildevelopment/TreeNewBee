// Fill out your copyright notice in the Description page of Project Settings.

#include "Armor.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"



AArmor::AArmor() 
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Appearence"));
	RootComponent = Mesh;
	Mesh->SetCollisionProfileName("NoCollsion");
}

void AArmor::Equip(USkeletalMeshComponent * const _skeletonMeshComponent)
{
	if (BodyMeshRef)
		_skeletonMeshComponent->SetSkeletalMesh(BodyMeshRef);

	if (BodyMaterialRef && BodyLogoMaterialRef)
	{
		_skeletonMeshComponent->SetMaterial(0, BodyMaterialRef);
		_skeletonMeshComponent->SetMaterial(1, BodyLogoMaterialRef);
	}

	Mesh->SetMasterPoseComponent(_skeletonMeshComponent);

}
