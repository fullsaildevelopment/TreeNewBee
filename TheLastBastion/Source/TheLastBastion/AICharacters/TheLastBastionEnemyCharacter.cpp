// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"

#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"

#include "Combat/PawnStatsComponent.h"

#include "UI/InGameAIHUD.h"
#include "UI/InGameFloatingText.h"

#include "AI/TheLastBastionBaseAIController.h"

#include "AIGroupBase.h"
#include "TheLastBastionHeroCharacter.h"





ATheLastBastionEnemyCharacter::ATheLastBastionEnemyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->SetCollisionProfileName("Enemy");
	GetMesh()->SetCollisionProfileName("EnemyBody");

	AiName = FText::FromString(TEXT("Base Enemy AI"));
	//AILevel = 1;
	bIsEnemy = true;

}



void ATheLastBastionEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	bIsEnemy = true;

	FAIHUDInitializer initializer;
	initializer.AIName = AiName;
	//initializer.AILevel = AILevel;
	initializer.AILevel = PawnStats->GetCharacterLevel();
	initializer.bIsEnemy = bIsEnemy;

	if (AI_HUD)
	{
		AI_HUD->InitRowHeader(initializer);
	}
}

void ATheLastBastionEnemyCharacter::HitResponse(AActor * DamageCauser, float _currentHp)
{

	if (bIsRangeUnit)
		return;

	const ATheLastBastionCharacter * attacker = Cast<ATheLastBastionCharacter>(DamageCauser);


	if (_currentHp <= 0)
	{
		// if this character is dead, then we just ask group to response

		if (mGroup->IsInBattle() == false)
			mGroup->OnChildTakingDamage(attacker);

	}
	else
	{
		if (mGroup->IsInBattle())
		{
			if (attacker && !attacker->IsEnemy() && !attacker->GetIsDead() && mGroup->HasThreat(attacker))
				SetTarget(DamageCauser);
		}
		else
			mGroup->OnChildTakingDamage(attacker);
	}


}

void ATheLastBastionEnemyCharacter::OnTakeDamageFromHero(const FVector & HitLocation,
	const ATheLastBastionHeroCharacter * heroAttacker, float totalDamage, bool isCritical, bool isStun)
{
	// if this AI is not being locked on, 
	// we will display UI temporary with a opacity animation
	if (!bAIHUDisDisplayedForLockedOn)
		AI_HUD->ToggleUI(true, true);

	// pop up some floating text
	TSubclassOf<UUserWidget> fT_WBP = AIStats->GetFloatingText_WBP();
	if (fT_WBP == nullptr)
	{
		UE_LOG(LogTemp, Error,
			TEXT("fT_WBP == nullptr - ATheLastBastionEnemyCharacter::OnTakePointDamageHandle"));
	}
	FVector2D screenPos;
	UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(this, 0), HitLocation, screenPos);
	UWorld* world = GetWorld();

	if (world && fT_WBP)
	{
		UInGameFloatingText* damageFT = Cast<UInGameFloatingText>(CreateWidget<UUserWidget>(world, fT_WBP));
		if (damageFT)
		{
			damageFT->SetInGameFTProperty(FText::AsNumber((int)totalDamage));
			damageFT->SetRenderTranslation(screenPos);
			damageFT->AddToViewport();
			UInGameFloatingText* criticalFT = nullptr;
			UInGameFloatingText* stunFT = nullptr;
			// if it is a critical hit
			if (isCritical)
			{
				criticalFT = Cast<UInGameFloatingText>(CreateWidget<UUserWidget>(world, fT_WBP));
				criticalFT->SetInGameFTProperty(FText::FromString(TEXT("Critical !!!")));
				criticalFT
					->SetRenderTranslation(
						FVector2D(screenPos.X + FMath::RandRange(-20.0f, 20.0f),
							screenPos.Y + FMath::RandRange(-10.0f, -30.0f)));
				criticalFT->AddToViewport();

				damageFT->SetFontSize(FontSize_Critical);
				criticalFT->SetFontSize(FontSize_Critical);
			}

			if (isStun)
			{
				stunFT = Cast<UInGameFloatingText>(CreateWidget<UUserWidget>(world, fT_WBP));
				stunFT->SetInGameFTProperty(FText::FromString(TEXT("Stun !!!")));
				stunFT
					->SetRenderTranslation(
						FVector2D(screenPos.X + FMath::RandRange(-20.0f, 20.0f),
							screenPos.Y + FMath::RandRange(10.0f, 20.0f)));

				stunFT->AddToViewport();
				stunFT->SetFontSize(FontSize_Critical);
				stunFT->SetStyle(EFloatingTextStyle::Stun);
			}

			// paint the text
			switch (heroAttacker->GetCurrentWeapon()->GetElementalType())
			{
			case::EElementalType::None:
				if (criticalFT)
				{
					damageFT->SetStyle(EFloatingTextStyle::Critical);
					criticalFT->SetStyle(EFloatingTextStyle::Critical);
				}
				break;
			case::EElementalType::Fire:

				damageFT->SetStyle(EFloatingTextStyle::Fire);
				if (criticalFT)
					criticalFT->SetStyle(EFloatingTextStyle::Fire);
				break;
			case::EElementalType::Ice:
				damageFT->SetStyle(EFloatingTextStyle::Ice);
				if (criticalFT)
					criticalFT->SetStyle(EFloatingTextStyle::Ice);
				break;
			}
		}
	}

}

void ATheLastBastionEnemyCharacter::EvaluateAttackerThreat(AActor * DamageCauser, float hp)
{
	ATheLastBastionCharacter* attacker = Cast<ATheLastBastionCharacter>(DamageCauser);
	// this an enemy is hit by an enemy, we dont put it threat map
	if (attacker->IsA(ATheLastBastionEnemyCharacter::StaticClass()))
		return;
	else
	{
		if (attacker && !attacker->GetIsDead())
		{
			//attacker->RegisterThreat(mGroup);
			if (hp > 0)
				mGroup->AddThreat(attacker, ThreatGain_ByHit, false);
			else
				mGroup->AddThreat(attacker, ThreatGain_ByKill,false);
		}

	}


}

void ATheLastBastionEnemyCharacter::ToggleAIHUD(bool _val)
{
	bAIHUDisDisplayedForLockedOn = _val;

	if (AI_HUD)
	{
		if (bAIHUDisDisplayedForLockedOn)
			AI_HUD->ToggleUI(true, false);
		else
			AI_HUD->ToggleUI(false, false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" aiHUD is NULL - ATheLastBastionEnemyCharacter::ToggleAIHUD"));
	}
}

void ATheLastBastionEnemyCharacter::OnGetUp()
{

	Super::OnGetUp();

	//GetCapsuleComponent()->SetCollisionProfileName("Enemy");
	//GetMesh()->SetCollisionProfileName("EnemyBody");

}




