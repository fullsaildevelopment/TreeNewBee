// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeHero_AnimInstance.h"




void UMeleeHero_AnimInstance::OnBeginPlay()
{
	Super::OnBeginPlay();
	currentEquipment = EEquipType::ShieldSword;
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

	if (attack)
		this->Montage_Play(attack, 1.0f);

	//UE_LOG(LogTemp, Warning, TEXT("Attack ! - UMeleeHero_AnimInstance"));
}

void UMeleeHero_AnimInstance::OnEquip()
{
	Super::OnEquip();

	switch (currentEquipment)
	{
	case EEquipType::ShieldSword:
		if (activatedEquipment == EEquipType::Travel)
			// Unequip
			this->PlayMontage(Equip_Montage, 1.0f, TEXT("UnequipSHSword"));
		else if (activatedEquipment == currentEquipment)
			// Equip
			this->PlayMontage(Equip_Montage, 1.0f, TEXT("EquipSHSword"));
		break;
	case EEquipType::TwoHandSword:
		if (activatedEquipment == EEquipType::Travel)
			// Unequip
			this->PlayMontage(Equip_Montage, 1.0f, TEXT("UnequipTHSword"));
		else if (activatedEquipment == currentEquipment)
			// Equip
			this->PlayMontage(Equip_Montage, 1.0f, TEXT("EquipTHSword"));
		break;
	}

	UE_LOG(LogTemp, Warning, TEXT("Equip ! - UMeleeHero_AnimInstance"));

}
