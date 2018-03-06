// Fill out your copyright notice in the Description page of Project Settings.

#include "HeroStatsComponent.h"

#include "TheLastBastionHeroCharacter.h"
#include "UI/InGameHUD.h"
#include "PCs/SinglePlayerPC.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "Combat/Weapon.h"
#include "Combat/Armor.h"
#include "CustomType.h"
#include "Animation/Hero_AnimInstance.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "UI/Gameplay/InventoryUI.h"


UHeroStatsComponent::UHeroStatsComponent()
{	
	// Just some init armor for our melee hero
	UCustomType::FindClass<AArmor>(Armor_ClassBp, TEXT("/Game/Blueprints/Gears/Tsun/Tsun_Armor"));
	Level = 1;

}

void UHeroStatsComponent::BeginPlay()
{

	Super::BeginPlay();
	if (mCharacter)
	{
		mHeroCharacter = Cast<ATheLastBastionHeroCharacter>(mCharacter);
		if (mHeroCharacter != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("UHeroStatsComponent is owned by ATheLastBastionHeroCharacter"));
			TargetDetector = mHeroCharacter->GetTargetDetector();
		}
		else
			UE_LOG(LogTemp, Error, TEXT("UHeroStatsComponent is NOT owned by a ATheLastBastionHeroCharacter"));

		mCurrentTarget = nullptr;
		mNextThreat = nullptr;

		if (TargetDetector)
		{
			TargetDetector->OnComponentBeginOverlap.AddDynamic(this, &UHeroStatsComponent::OnEnemyEnter);
			TargetDetector->OnComponentEndOverlap.AddDynamic(this, &UHeroStatsComponent::OnEnemyLeave);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("TargetDetector is NULL UHeroStatsComponent::BeginPlay"));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("mCharacter is NULL - UHeroStatsComponent::BeginPlay"));
	}

}

void UHeroStatsComponent::OnFocus()
{
	if (mHeroCharacter && mHeroCharacter->GetCharacterType() == ECharacterType::Ranger)
	{
		// if not focused, enter focus mode, else quit
		UHero_AnimInstance* animRef = mHeroCharacter->GetAnimInstanceRef();
		if (animRef)
		{
			bool setTargetToNull
				= animRef->GetIsFocus() || animRef->GetFocusPendingExit();
			if (setTargetToNull)
			{
				if (mCurrentTarget)
				{
					mCurrentTarget->ToggleAIHUD(false);
					mCurrentTarget = nullptr;
				}
				//UE_LOG(LogTemp, Warning, TEXT("Quit Focus !!!"));
			}
			else
			{
				MeleeFocus();
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("animRef is NULL - UHeroStatsComponent::OnFocus"));
		}
		
	}
}

bool UHeroStatsComponent::OnSwapBetweenMeleeAndRange()
{

	int cBIndex = (int)EEquipType::CrossBow - 1;

	bool accept = WeaponSlots[cBIndex].RightHand && WeaponSlots[LastMeleeWeapon_Index].RightHand;

	if (accept)
	{

		bool currentUseCB = mHeroCharacter->GetAnimInstanceRef()->GetCurrentEquipmentType()
			== EEquipType::CrossBow;
		if (currentUseCB)
		{
			// use last melee
			EEquipType nextWeapon = (EEquipType)(LastMeleeWeapon_Index + 1);
			OnSwitchWeapon(nextWeapon);
		}
		else
		{
			OnSwitchWeapon(EEquipType::CrossBow);
		}

	}
	return accept;
}

bool UHeroStatsComponent::OnSwitchWeapon(EEquipType _nextEquip)
{
	// index for the next equip
	int nextEquip = (int)(_nextEquip) - 1;

	bool accept = false;

	// check the current index is valid or not to avoid crash
	if (nextEquip < 0 || nextEquip > GetMaxNumOfWeaponSlot())
	{
		UE_LOG(LogTemp, Error,
			TEXT("CurrentWeapon_Index is corrupted - UHeroStatsComponent::OnSwitchWeapon "));
		return accept;
	}

	// Swap Weapon
	accept = WeaponSlots[nextEquip].RightHand != nullptr;
	if (accept)
	{
		/// ***  put current weapon back to equip slot
		AGear* rightWeapon = WeaponSlots[CurrentWeapon_Index].RightHand;
		AGear* leftWeapon = WeaponSlots[CurrentWeapon_Index].LeftHand;

		if (rightWeapon == nullptr)
		{
			accept = false;
			UE_LOG(LogTemp, Error, 
				TEXT("Current Main Weapon is null - UHeroStatsComponent::OnSwitchWeapon"));
			return accept;
		}

		rightWeapon->Equip(mCharacter->GetMesh());
		if (WeaponSlots[CurrentWeapon_Index].bHideWhenEquip)
			rightWeapon->ToggleVisibilty(false);

		if (leftWeapon)
			leftWeapon->Equip(mCharacter->GetMesh());

		// Handle some special toggling based on current weapon type
		switch (rightWeapon->GetGearType())
		{
			case EGearType::LongSword:
			case EGearType::Mace:
			case EGearType::WarAxe:
			{
				if (leftWeapon)
					leftWeapon->ToggleVisibilty(false);
				break;
			}
			case EGearType::CrossBow:
			{
				mHeroCharacter->ToggleFireMode(false);
				break;
			}
			default:
				break;		
		}

		/// ***  put next weapon on arm slot

		// Toggle Visiblity for next weapon
		leftWeapon = WeaponSlots[nextEquip].LeftHand;
		if (leftWeapon)
			leftWeapon->ToggleVisibilty(true);

		rightWeapon = WeaponSlots[nextEquip].RightHand;

		if (_nextEquip == EEquipType::CrossBow)
			mHeroCharacter->ToggleFireMode(true);

		/// ***  Finish
		// Hold the last melee weapon Index for fast switch between range and melee;
		int EquipTypeInt = CurrentWeapon_Index + 1;
		if (EquipTypeInt != (int)EEquipType::CrossBow)
		{
			LastMeleeWeapon_Index = CurrentWeapon_Index;
		}
		CurrentWeapon_Index = nextEquip;
		GenerateMaxStats();
		mHeroCharacter->GetInGameHUD()->SetCurrentWeaponImage(rightWeapon);
	}

	return accept;
}

void UHeroStatsComponent::OnTradeMenuAccept(UInventoryUI * _inventoryMenu)
{
	UWorld* world = GetWorld();
	UHero_AnimInstance* animRef = mHeroCharacter->GetAnimInstanceRef();

	if (world == nullptr || animRef == nullptr)
		return;

	FActorSpawnParameters spawnParam;
	spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnParam.Owner = mCharacter;

	/// Check Armor
	if (Armor_ClassBp != _inventoryMenu->GetCurrentArmor())
	{
		Armor_ClassBp = _inventoryMenu->GetCurrentArmor();
		if (Armor_ClassBp)
		{
			Armor->Destroy();
			Armor = world->SpawnActor<AArmor>(Armor_ClassBp, spawnParam);
			Armor->Equip(mCharacter->GetMesh());
		}
	}
	
	/// Check Weapon
	for (int i = 0; i < GetMaxNumOfWeaponSlot(); i++)
	{
		TSubclassOf<AGear> weaponClass = _inventoryMenu->GetGearClassAt(i);
		// if the weapon class on this slot is changed
		if (WeaponSlots[i].WeaponClass != weaponClass)
		{
			// destory the old class instance
			WeaponSlots[i].WeaponClass = weaponClass;
			AGear* rightHand = WeaponSlots[i].RightHand;
			AGear* leftHand = WeaponSlots[i].LeftHand;

			if (rightHand)
			{
				rightHand->Destroy();
				rightHand = nullptr;
			}
			if (leftHand)
			{
				leftHand->Destroy();
				leftHand = nullptr;
			}

			// spawn the new weapon by the new class
			rightHand = world->SpawnActor<AGear>(weaponClass, spawnParam);
			rightHand->Equip(mCharacter->GetMesh());
			if (WeaponSlots[i].bHideWhenEquip)
				rightHand->ToggleVisibilty(false);

			TSubclassOf<AGear> LeftHandClass = rightHand->GetLeftHandGear();
			if (LeftHandClass)
			{
				leftHand = world->SpawnActor<AGear>(LeftHandClass, spawnParam);
				leftHand->Equip(mCharacter->GetMesh());
			}

			WeaponSlots[i].RightHand = rightHand;
			WeaponSlots[i].LeftHand = leftHand;
		}
	}

	/// Check if we should arm any weapon?
	if (animRef->GetActivatedEquipmentType() != EEquipType::Travel)
	{
		GetCurrentRightHandWeapon()->Arm(mCharacter->GetMesh());
	}
	if (animRef->GetCurrentEquipmentType() != EEquipType::ShieldSword)
	{
		// Hide Shield if we are not dont armed with SH weapon
		AGear* shield = WeaponSlots[int(EEquipType::ShieldSword) - 1].LeftHand;
		if (shield)
			shield->ToggleVisibilty(false);
	}



	animRef->UpdateComboList(GetCurrentRightHandWeapon()->GetGearType());
	GenerateMaxStats();
}

void UHeroStatsComponent::UpdateEquipHideOption()
{
	
	bool inTravel = mHeroCharacter->GetAnimInstanceRef()->GetActivatedEquipmentType() == EEquipType::Travel;

	if (inTravel)
	{
		for (int i = 0; i < GetMaxNumOfWeaponSlot(); i++)
		{
			if (WeaponSlots[i].RightHand)
				WeaponSlots[i].RightHand->ToggleVisibilty(!WeaponSlots[i].bHideWhenEquip);
		}
	}
	else
	{
		// if it is not in travel, we are hide the weapon we are armming
		for (int i = 0; i < GetMaxNumOfWeaponSlot(); i++)
		{
			if (WeaponSlots[i].RightHand  && i != CurrentWeapon_Index)
				WeaponSlots[i].RightHand->ToggleVisibilty(!WeaponSlots[i].bHideWhenEquip);
		}
	}
}

void UHeroStatsComponent::OnEnemyEnter(UPrimitiveComponent * _overlappedComponent, AActor * _otherActor, UPrimitiveComponent * _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult & _SweepResult)
{
	ATheLastBastionEnemyCharacter* newPotentialTarget = Cast<ATheLastBastionEnemyCharacter>(_otherActor);
	if (newPotentialTarget)
	{
		mPotentialTargets.Add(newPotentialTarget);
		//UE_LOG(LogTemp, Warning, 
		//	TEXT("Detect Enemy Enter, %s, %d left in list "), 
		//	*_otherActor->GetName(), mPotentialTargets.Num());
	}
	
}

void UHeroStatsComponent::OnEnemyLeave(UPrimitiveComponent * _overlappedComponent, AActor * _otherActor, UPrimitiveComponent * _otherComp, int32 _otherBodyIndex)
{
	if (mHeroCharacter)
	{
		if (mHeroCharacter->GetCharacterType() == ECharacterType::Ranger)
		{
			OnEnemyLeaveMelee(_otherActor);
		}
	}
}

void UHeroStatsComponent::OnEnemyLeaveMelee(AActor * _otherActor)
{
	ATheLastBastionEnemyCharacter* leave = Cast<ATheLastBastionEnemyCharacter>(_otherActor);
	if (leave)
	{
		mPotentialTargets.Remove(leave);
		//UE_LOG(LogTemp, Warning, 
		//	TEXT("Detect Enemy Leave, remove %s, %d left in list"),
		//	*_otherActor->GetName(), mPotentialTargets.Num());
		if (leave == mCurrentTarget)
		{
			mCurrentTarget->ToggleAIHUD(false);
			mCurrentTarget = nullptr;
			// if our current target is leaving the detector, quit Focus
			// Stay away from enemy will leave focus mode

			UHero_AnimInstance* animInstanceRef = Cast<UHero_AnimInstance>(mHeroCharacter->GetAnimInstanceRef());
			if (animInstanceRef && animInstanceRef->GetIsFocus())
			{
				animInstanceRef->OnFocus();
			}
		}
	}

}

void UHeroStatsComponent::MeleeFocus()
{
	mCurrentTarget = nullptr;
	//UE_LOG(LogTemp, Warning, TEXT("Ranger Search Focus Target -  UHeroStatsComponent::MeleeFocus()"));
	// Grab 

	float maxDirection = -1.0f;
	FVector camForwardVector = mHeroCharacter->GetFollowCamera()->GetForwardVector();
	camForwardVector.Z = 0;
	camForwardVector = camForwardVector.GetSafeNormal();
	for (size_t i = 0; i < mPotentialTargets.Num(); i++)
	{
		// if this guy was not in our frustum recently, skip it
		if (!mPotentialTargets[i]->WasRecentlyRendered() || mPotentialTargets[i]->GetIsDead())
		{
			//UE_LOG(LogTemp, Warning, 
			//	TEXT("%s, is not recently rendered"), *mPotentialTargets[i]->GetName());
			continue;
		}

		FVector away = mPotentialTargets[i]->GetActorLocation() - mHeroCharacter->GetActorLocation();
		away.Z = 0;
		away = away.GetUnsafeNormal();
		float direction = FVector::DotProduct(camForwardVector, away);
		if (direction > maxDirection)
		{
			mCurrentTarget = mPotentialTargets[i];
			maxDirection = direction;
		}

	}
	if (mCurrentTarget)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Current Target is %s,"), *mCurrentTarget->GetName());
		mCurrentTarget->ToggleAIHUD(true);
		//mHeroCharacter->GetFollowCamera()->

	}
	else
		UE_LOG(LogTemp, Warning, TEXT("No Availble Target"));
}
