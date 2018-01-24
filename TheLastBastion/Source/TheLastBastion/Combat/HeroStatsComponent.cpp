// Fill out your copyright notice in the Description page of Project Settings.

#include "HeroStatsComponent.h"
#include "Net/UnrealNetwork.h"

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


	// Let Server to call client to Init UI
	if (mHeroCharacter)
	{
		ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(mHeroCharacter->GetController());
		if (pc)
			pc->InitUIOnBeginPlay(this);
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
	bool accept = WeaponSlots[0].RightHand && WeaponSlots[1].RightHand;

	if (accept)
	{
		AGear* leftWeapon =  WeaponSlots[CurrentWeapon_Index].LeftHand;
		AGear* rightWeapon = WeaponSlots[CurrentWeapon_Index].RightHand;

		switch (mHeroCharacter->GetAnimInstanceRef()->GetCurrentEquipmentType())
		{
			// hide the shield, equip sword
		case EEquipType::ShieldSword:
		{
			if (leftWeapon)
				leftWeapon->ToggleVisibilty(false);
			rightWeapon->Equip(mCharacter->GetMesh());
			break;
		}
		case EEquipType::CrossBow:
		{
			rightWeapon->Equip(mCharacter->GetMesh());
			mHeroCharacter->GetInGameHUD()->ToggleFireMode(false);
			break;
		}
		}

		// Get the next weapon slot
		CurrentWeapon_Index++;
		if (CurrentWeapon_Index >= GetMaxWeaponSlot())
			CurrentWeapon_Index =0;

		// Toggle Visiblity for next weapon
		leftWeapon = WeaponSlots[CurrentWeapon_Index].LeftHand;
		if (leftWeapon)
			leftWeapon->ToggleVisibilty(true);

		rightWeapon = WeaponSlots[CurrentWeapon_Index].RightHand;
		if (rightWeapon->GetGearType() == EGearType::CrossBow)
			mHeroCharacter->GetInGameHUD()->ToggleFireMode(true);
		rightWeapon->ToggleVisibilty(true);
	}

	return accept;
}

void UHeroStatsComponent::OnTradeMenuAccept(UInventoryUI * _inventoryMenu)
{
	UWorld* world = GetWorld();
	if (world == nullptr)
		return;

	FActorSpawnParameters spawnParam;
	spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnParam.Owner = mCharacter;

	// Check Armor
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
	
	// Check Weapon

	if (WeaponWheels[0] != _inventoryMenu->GetCurrentSHWeapon())
	{
		WeaponWheels[0] = _inventoryMenu->GetCurrentSHWeapon();
		if (WeaponWheels[0])
		{
			AGear* rightHand = WeaponSlots[0].RightHand;
			AGear* leftHand = WeaponSlots[0].LeftHand;

			if (WeaponSlots[0].RightHand)
				rightHand->Destroy();
			if (WeaponSlots[0].LeftHand)
				leftHand->Destroy();

			WeaponSlots[0].RightHand = world->SpawnActor<AGear>(WeaponWheels[0], spawnParam);
			TSubclassOf<AGear> LeftHandClass = WeaponSlots[0].RightHand->GetLeftHandGear();
			if (LeftHandClass)
				WeaponSlots[0].LeftHand = world->SpawnActor<AGear>(LeftHandClass, spawnParam);

			WeaponSlots[0].RightHand->Equip(mCharacter->GetMesh());
			WeaponSlots[0].LeftHand->Equip(mCharacter->GetMesh());

		}
	}

	GenerateMaxStats();
}

void UHeroStatsComponent::OnEnemyEnter(UPrimitiveComponent * _overlappedComponent, AActor * _otherActor, UPrimitiveComponent * _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult & _SweepResult)
{
	ATheLastBastionEnemyCharacter* newPotentialTarget = Cast<ATheLastBastionEnemyCharacter>(_otherActor);
	if (newPotentialTarget)
	{
		mPotentialTargets.Add(newPotentialTarget);
		UE_LOG(LogTemp, Warning, 
			TEXT("Detect Enemy Enter, %s, %d left in list "), 
			*_otherActor->GetName(), mPotentialTargets.Num());
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
	UE_LOG(LogTemp, Warning, TEXT("Ranger Search Focus Target"));
	// Grab 

	float maxDirection = -1.0f;
	FVector camForwardVector = mHeroCharacter->GetFollowCamera()->GetForwardVector();
	camForwardVector.Z = 0;
	camForwardVector = camForwardVector.GetSafeNormal();
	for (size_t i = 0; i < mPotentialTargets.Num(); i++)
	{
		// if this guy was not in our frustum recently, skip it
		if (!mPotentialTargets[i]->WasRecentlyRendered())
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

void UHeroStatsComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHeroStatsComponent, mHeroCharacter);
}
