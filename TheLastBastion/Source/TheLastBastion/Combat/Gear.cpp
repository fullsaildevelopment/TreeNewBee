// Fill out your copyright notice in the Description page of Project Settings.

#include "Gear.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TheLastBastionCharacter.h"
#include "CustomType.h"
#include "Combat/TLBDamageType.h"
// Sets default values

TSubclassOf <class UDamageType> AGear::ComboBullets_Type = nullptr;
TSubclassOf <class UDamageType> AGear::NormalBullets_Type = nullptr;


AGear::AGear()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// SetReplicates(true);

	if (ComboBullets_Type == nullptr)
		UCustomType::FindClass<UDamageType>(ComboBullets_Type, TEXT("/Game/Blueprints/DamageType/DT_ComboBullets"));

	if (NormalBullets_Type == nullptr)
		UCustomType::FindClass<UDamageType>(NormalBullets_Type, TEXT("/Game/Blueprints/DamageType/DT_Bullets"));

}

// Called when the game starts or when spawned
void AGear::BeginPlay()
{
	Super::BeginPlay();	
}

void AGear::Equip(class USkeletalMeshComponent* const _skeletonMeshComponent)
{
	if (GearOwner == nullptr)
	{
		GearOwner = Cast<ATheLastBastionCharacter>(_skeletonMeshComponent->GetOwner());
		SetOwner(GearOwner);
	}

	FName SlotName;
	switch (GearType)
	{
	case EGearType::Shield:
		SlotName = TEXT("Shield");
		break;
	case EGearType::LongSword:
	case EGearType::WarAxe:
	case EGearType::Mace:

		SlotName = TEXT("SHSwordEquip");
		break;
	case EGearType::DoubleHandWeapon:
		SlotName = TEXT("DHSwordEquip");
		break;
	case EGearType::BattleAxe:
	case EGearType::GreatSword:
	case EGearType::Hammer:
	case EGearType::DaiKatana:
		SlotName = TEXT("HeavyEquip");
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
	default:
	case EGearType::Armor:
		SlotName = TEXT("Root");
		break;
	}

	this->AttachToComponent(_skeletonMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, SlotName);		
}

void AGear::Arm(USkeletalMeshComponent * const _skeletonMeshComponent)
{
	if (GearOwner == nullptr)
	{
		GearOwner = Cast<ATheLastBastionCharacter>(_skeletonMeshComponent->GetOwner());
		SetOwner(GearOwner);
	}

	FName SlotName;

	switch (GearType)
	{
	case EGearType::Armor:
		SlotName = TEXT("Root");
		break;
	case EGearType::Shield:
		SlotName = TEXT("Shield");
		break;
	case EGearType::LongSword:
	case EGearType::WarAxe:
	case EGearType::Mace:
		SlotName = TEXT("SingleHandWeapon");
		break;
	case EGearType::DoubleHandWeapon:
	case EGearType::DaiKatana:
		SlotName = TEXT("TwoHandWeapon");
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
	case EGearType::GreatSword:
	case EGearType::Hammer:
	case EGearType::BattleAxe:
		SlotName = TEXT("HeavyWeapon");
		break;
	default:
		break;
	}
	ToggleVisibilty(true);
	this->AttachToComponent(_skeletonMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, SlotName);
	MaintainScale();
}

TArray<int> AGear::GetResourceCost_Implementation() const
{

	TArray<int> Cost;
	Cost.SetNum(2);
	for (int i = 0; i < 2; i++)
	{
		Cost[i] = 100;
	}
	return Cost;
}

void AGear::SetDamageIsEnabled(bool _val)
{

}

void AGear::ToggleVisibilty(bool _val)
{

	USceneComponent* mesh = GetMesh();
	if (mesh)
	{
		mesh->SetVisibility(_val);
	}
}

//void AGear::CombineDamage(const AGear * _other)
//{
//	PhysicalDamage += _other->PhysicalDamage;
//	//ElementalDamage += _other->ElementalDamage;
//	//FireDamage += _other->FireDamage;
//	//IceDamage  += _other->IceDamage;
//}




