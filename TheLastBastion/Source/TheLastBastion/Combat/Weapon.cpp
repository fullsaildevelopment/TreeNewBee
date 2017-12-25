// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"



AWeapon::AWeapon() 
{

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Appearence"));
	Mesh->SetCollisionProfileName("EnemyWeapon");
	RootComponent = Mesh;


}
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Arm(USkeletalMeshComponent * const _skeletonMeshComponent)
{
	FName SlotName;

	switch (GearType)
	{
	case EGearType::Armor:
	default:
		SlotName = TEXT("Root");
		break;
	case EGearType::Shield:
		SlotName = TEXT("Shield");
		break;
	case EGearType::SingleHandWeapon:
		SlotName = TEXT("SingleHandWeapon");
		break;
	case EGearType::DoubleHandWeapon:
		SlotName = TEXT("DHSwordEquip");
		break;
	case EGearType::CrossBow:
		SlotName = TEXT("CrossBow");
		break;
	case EGearType::CrossBowBolt:
		SlotName = TEXT("BoltsEquip");
		break;
	case EGearType::TwinBlade:
		SlotName = TEXT("ik_hand_r");
		break;

	}
	this->AttachToComponent(_skeletonMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, SlotName);

}


