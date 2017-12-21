// Fill out your copyright notice in the Description page of Project Settings.

#include "HeroStatsComponent.h"
//#include "TheLastBastionCharacter.h"
#include "TheLastBastionHeroCharacter.h"

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




UHeroStatsComponent::UHeroStatsComponent() : Super()
{
	if (mCharacter)
	{
		if (mCharacter != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("PawnStatsComponent is owned by TheLastBastion Character"));

			// Just some init armor for our melee hero
			UCustomType::FindClass<AWeapon>(LeftHandWeapon_ClassBp, TEXT("/Game/Blueprints/Gears/Tsun_Shield"));
			UCustomType::FindClass<AWeapon>(RightHandWeapon_ClassBp, TEXT("/Game/Blueprints/Gears/Tsun_SHSword"));
			UCustomType::FindClass<AArmor>(Armor_ClassBp, TEXT("/Game/Blueprints/Gears/Tsun_Armor"));

		}
		else
			UE_LOG(LogTemp, Warning, TEXT("PawnStatsComponent owner must be a TheLastBastion Character"));

		SetDamageDetectorsCollsionProfile(TEXT("HeroBody"));

		MeleeTargetDetector = CreateDefaultSubobject<USphereComponent>(TEXT("MeleeTargetDetector"));
		MeleeTargetDetector->SetupAttachment(mCharacter->GetMesh(), TEXT("Root"));
		MeleeTargetDetector->InitSphereRadius(1200);
		MeleeTargetDetector->SetCanEverAffectNavigation(false);
		MeleeTargetDetector->bGenerateOverlapEvents = true;
		MeleeTargetDetector->bHiddenInGame = false;
		MeleeTargetDetector->SetCollisionProfileName("EnemyDetector");

		mCurrentTarget = nullptr;
		mNextThreat = nullptr;
	}
}

void UHeroStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	mHeroCharacter = Cast<ATheLastBastionHeroCharacter>(mCharacter);

	if (!mHeroCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("HeroStatsComponent is not owned by TheLastBastionHero Character"));
	}

	MeleeTargetDetector->OnComponentBeginOverlap.AddDynamic(this, &UHeroStatsComponent::OnEnemyEnter);
	MeleeTargetDetector->OnComponentEndOverlap.AddDynamic(this, &UHeroStatsComponent::OnEnemyLeave);
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
			mCurrentTarget = nullptr;
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
	const ATheLastBastionEnemyCharacter* newPotentialTarget = Cast<ATheLastBastionEnemyCharacter>(_otherActor);
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
	const ATheLastBastionEnemyCharacter* leave = Cast<ATheLastBastionEnemyCharacter>(_otherActor);
	if (leave)
	{
		mPotentialTargets.Remove(leave);
		//UE_LOG(LogTemp, Warning, 
		//	TEXT("Detect Enemy Leave, remove %s, %d left in list"),
		//	*_otherActor->GetName(), mPotentialTargets.Num());
		if (leave == mCurrentTarget)
		{
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
			UE_LOG(LogTemp, Warning, 
				TEXT("%s, is not recently rendered"), *mPotentialTargets[i]->GetName());
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
		UE_LOG(LogTemp, Warning, TEXT("Current Target is %s,"), *mCurrentTarget->GetName());

		//mHeroCharacter->GetFollowCamera()->

	}
	else
		UE_LOG(LogTemp, Warning, TEXT("No Availble Target"));
}
