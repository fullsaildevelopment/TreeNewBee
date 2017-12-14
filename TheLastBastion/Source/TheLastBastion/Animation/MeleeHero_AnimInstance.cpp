// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeHero_AnimInstance.h"
#include "TheLastBastionCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


const TArray<FName> Attack_Montage_SectionNames
= { FName("Combo_1") ,FName("Combo_2"),FName("Combo_3"), FName("Combo_4") };



UMeleeHero_AnimInstance ::UMeleeHero_AnimInstance(const FObjectInitializer& _objectInitalizer) : Super(_objectInitalizer)
{
	CurrentComboIndex = 0;
}

void UMeleeHero_AnimInstance::OnBeginPlay()
{
	Super::OnBeginPlay();
	CurrentEquipment = EEquipType::ShieldSword;
	UE_LOG(LogTemp, Warning, TEXT("UMeleeHero_AnimInstance Call BeginPlay, %d"), (int)AttackState);

}

void UMeleeHero_AnimInstance::OnInit()
{
	Super::OnInit();
	//UE_LOG(LogTemp, Warning, TEXT("UMeleeHero_AnimInstance Call OnInit"));

}

void UMeleeHero_AnimInstance::OnUpdate(float _deltaTime)
{
	Super::OnUpdate(_deltaTime);


	//if (bRotationRateOverrideByAnim)
	//{
	//}

	// disable head track during slash someone
	if (AttackState == EAttackState :: None)
		HeadTrackAlpha += 0.05f;
	else
		HeadTrackAlpha -= 0.05f;

	HeadTrackAlpha = FMath::Clamp(HeadTrackAlpha, 0.0f, 1.0f);
}

void UMeleeHero_AnimInstance::OnPostEvaluate()
{

}

void UMeleeHero_AnimInstance::OnAttack()
{
	Super::OnAttack();

	if (Attack_Montage)
	{		
	    //if this is the first attack, we will launch the first combo
		if (CurrentComboIndex == 0)
		{
			if (AttackState != EAttackState::None)
			{
				// this means our melee character is finish his last combo cycle but not ready for next, so do nothing
				UE_LOG(LogTemp, Warning, TEXT("mCurrentComboIndex: %d, finishing last combo cycle, attack ignore "), CurrentComboIndex);
				bTryToAttack = false;
				return;
			}

			UE_LOG(LogTemp, Warning, TEXT("mCurrentComboIndex: %d, first combo "), CurrentComboIndex);
			this->PlayMontage(Attack_Montage, 1.0f, TEXT("Combo_0_cl"));
			bTryToAttack = false;
			AttackState = EAttackState::PreWinding;
			CurrentComboIndex++;
		}
		else
		{
			// if the our character is waiting for next combo instruction, launch the next combo
			if (AttackState == EAttackState::ReadyForNext)
			{
				UE_LOG(LogTemp, Warning, TEXT("mCurrentComboIndex: %d, during Ready for next "), CurrentComboIndex);
				DoNextAttack();
			}			
		}
		bSpeedOverrideByAnim = true;

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack_Montage is Null ! - UMeleeHero_AnimInstance"));
	}

}

void UMeleeHero_AnimInstance::OnEquip()
{
	Super::OnEquip();

	switch (CurrentEquipment)
	{
	case EEquipType::ShieldSword:
		if (ActivatedEquipment == EEquipType::Travel)
			// Unequip
			this->PlayMontage(Equip_Montage, 1.0f, TEXT("UnequipSHSword"));
		else if (ActivatedEquipment == CurrentEquipment)
			// Equip
			this->PlayMontage(Equip_Montage, 1.0f, TEXT("EquipSHSword"));
		break;
	case EEquipType::TwoHandSword:
		if (ActivatedEquipment == EEquipType::Travel)
			// Unequip
			this->PlayMontage(Equip_Montage, 1.0f, TEXT("UnequipTHSword"));
		else if (ActivatedEquipment == CurrentEquipment)
			// Equip
			this->PlayMontage(Equip_Montage, 1.0f, TEXT("EquipTHSword"));
		break;
	}

	UE_LOG(LogTemp, Warning, TEXT("Equip ! - UMeleeHero_AnimInstance"));

}

void UMeleeHero_AnimInstance::OnNextAttack()
{

	Super::OnNextAttack();

	if (CurrentComboIndex == 0)
	{
		bTryToAttack = false;
		UE_LOG(LogTemp, Warning, 
			TEXT("mCurrentComboIndex: %d, on try next attack to the perform the first combo, action abort "), CurrentComboIndex);
		AttackState = EAttackState::None;

		// Unlock the character rotation
		bSpeedOverrideByAnim = false;
	}
	else
	{
		if (bTryToAttack)
		{
			UE_LOG(LogTemp, Warning, TEXT("mCurrentComboIndex: %d, AT Ready for next "), CurrentComboIndex);
			DoNextAttack();
		}
		
	}
	bRotationRateOverrideByAnim = false;



}

void UMeleeHero_AnimInstance::DoNextAttack()
{

	this->PlayMontage(Attack_Montage, 1.0f, Attack_Montage_SectionNames[CurrentComboIndex - 1]);
	bTryToAttack = false;
	AttackState = EAttackState::PreWinding;
	CurrentComboIndex++;
	if (CurrentComboIndex == Attack_Montage_SectionNames.Num() + 1)
		CurrentComboIndex = 0;
}

void UMeleeHero_AnimInstance::OnResetCombo()
{
	Super::OnResetCombo();
	CurrentComboIndex = 0;
	bSpeedOverrideByAnim = false;
	bRotationRateOverrideByAnim = false;


}

void UMeleeHero_AnimInstance::OnEnableDamage(bool bIsright, bool bIsAll)
{
	Super::OnEnableDamage(bIsright, bIsAll);

	//Lock the character rotation during attacking
	bRotationRateOverrideByAnim = true;
	mCharacter->GetCharacterMovement()->RotationRate.Yaw = 0;
}

void UMeleeHero_AnimInstance::OnDisableDamage(bool bIsright, bool bIsAll)
{
	Super::OnDisableDamage(bIsright, bIsAll);
}


