// Fill out your copyright notice in the Description page of Project Settings.

#include "Gear.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TheLastBastionCharacter.h"

// Sets default values
AGear::AGear()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	
	
}

// Called when the game starts or when spawned
void AGear::BeginPlay()
{
	Super::BeginPlay();	
}

void AGear::Equip(class USkeletalMeshComponent* const _skeletonMeshComponent)
{


	GearOwner = Cast<ATheLastBastionCharacter>(_skeletonMeshComponent->GetOwner());
	if (GearOwner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GearOwner is not a ATheLastBastionCharacter - AGear::Equip "));
		return;
	}
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
		SlotName = TEXT("SHSwordEquip");
		break;
	case EGearType::DoubleHandWeapon:
		SlotName = TEXT("DHSwordEquip");
		break;
	case EGearType::CrossBow:
		SlotName = TEXT("CrossBowEquip");
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




