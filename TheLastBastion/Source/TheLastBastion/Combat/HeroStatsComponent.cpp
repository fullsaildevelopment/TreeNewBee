// Fill out your copyright notice in the Description page of Project Settings.

#include "HeroStatsComponent.h"
//#include "TheLastBastionCharacter.h"
#include "TheLastBastionHeroCharacter.h"
#include "GI_TheLastBastion.h"
#include "UI/InGameHUD.h"
#include "PCs/GamePC.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"

#include "Combat/Weapon.h"
#include "Combat/Armor.h"
#include "CustomType.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "Animation/Hero_AnimInstance.h"




UHeroStatsComponent::UHeroStatsComponent()
{
	
	// Just some init armor for our melee hero
	UCustomType::FindClass<AWeapon>(LeftHandWeapon_ClassBp, TEXT("/Game/Blueprints/Gears/Tsun_Shield"));
	UCustomType::FindClass<AWeapon>(RightHandWeapon_ClassBp, TEXT("/Game/Blueprints/Gears/Tsun_SHSword"));
	UCustomType::FindClass<AArmor>(Armor_ClassBp, TEXT("/Game/Blueprints/Gears/Tsun_Armor"));
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

		// Let Our UI to pop our stats on our screen
		AGamePC* gamePC = Cast<AGamePC>( mHeroCharacter->GetController());
		if (gamePC)
		{
			gamePC->CLIENT_InitUI(this);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("mCharacter is NULL - UHeroStatsComponent::BeginPlay"));
	}
}

void UHeroStatsComponent::OnFocus()
{
	if (mHeroCharacter)
	{
		// if not focused, enter focus mode, else quit
		bool setTargetToNull
			= mHeroCharacter->GetAnimInstanceRef()->GetIsFocus() ||
			mHeroCharacter->GetAnimInstanceRef()->GetFocusPendingExit();
		if (setTargetToNull)
		{
			if (mCurrentTarget)
			{
				mCurrentTarget->ToggleAIHUD(false);
				mCurrentTarget = nullptr;
			}
			UE_LOG(LogTemp, Warning, TEXT("Quit Focus !!!"));
		}
		else
		{
			if (mCharacter->GetCharacterType() == ECharacterType::Ranger)
			{
				// Melee hero focus target determination
				MeleeFocus();
			}
			else
			{

			}

		}
	}
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
			if (mHeroCharacter->GetAnimInstanceRef()->GetIsFocus())
			{
				mHeroCharacter->GetAnimInstanceRef()->OnFocus();
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

