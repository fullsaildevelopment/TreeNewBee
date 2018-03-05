// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionAllyCharacter.h"
#include "Combat/Gear.h"

#include "UI/InGameAIHUD.h"
#include "UI/InGameFloatingText.h"

#include "Kismet/GameplayStatics.h"

#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"

#include "AIGroupBase.h"

#include "TheLastBastionHeroCharacter.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"



ATheLastBastionAllyCharacter::ATheLastBastionAllyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->SetCollisionProfileName("Ally");
	GetMesh()->SetCollisionProfileName("HeroBody");
	AiName = FText::FromString(TEXT("Base Ally"));
	AILevel = 1;

	bIsEnemy = false;

}

void ATheLastBastionAllyCharacter::BeginPlay()
{
	Super::BeginPlay();
	FAIHUDInitializer initializer;
	initializer.AIName = AiName;
	initializer.AILevel = AILevel;
	initializer.bIsEnemy = bIsEnemy;

	if (AI_HUD)
	{
		AI_HUD->InitRowHeader(initializer);
	}
}

void ATheLastBastionAllyCharacter::HitResponse(AActor * DamageCauser)
{	
	if (bIsRangeUnit)
		return;

	if (mGroup->IsInBattle())
	{
		const ATheLastBastionCharacter * attacker = Cast<ATheLastBastionCharacter>(DamageCauser);
		if (attacker && attacker->IsEnemy() && !attacker->GetIsDead())
			SetTarget(DamageCauser);
	}
}

void ATheLastBastionAllyCharacter::GenerateFloatingText(const FVector & HitLocation,
	const ATheLastBastionHeroCharacter * heroAttacker, float totalDamage, bool isCritical, bool isStun)
{
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
			UInGameFloatingText* friendlyFireFT = nullptr;
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

			friendlyFireFT = Cast<UInGameFloatingText>(CreateWidget<UUserWidget>(world, fT_WBP));
			friendlyFireFT->SetInGameFTProperty(FText::FromString(TEXT("Friendly Fire")));
			friendlyFireFT
				->SetRenderTranslation(
					FVector2D(screenPos.X + FMath::RandRange(-20.0f, 20.0f),
						screenPos.Y + FMath::RandRange(10.0f, 20.0f)));

			friendlyFireFT->AddToViewport();
			friendlyFireFT->SetFontSize(FontSize_Critical);
			friendlyFireFT->SetStyle(EFloatingTextStyle::Enemy);

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

void ATheLastBastionAllyCharacter::EvaluateAttackerThreat(AActor * DamageCauser, float hp)
{
	ATheLastBastionEnemyCharacter* attacker = Cast<ATheLastBastionEnemyCharacter>(DamageCauser);
	// this an enemy is hit by an enemy, we dont put it threat map
	if (attacker && !attacker->GetIsDead())
	{
		//attacker->RegisterThreat(mGroup);
		if (hp > 0)
			mGroup->AddThreat(attacker, ThreatGain_ByHit);
		else
		{
			mGroup->AddThreat(attacker, ThreatGain_ByKill);
		}
	}
}

void ATheLastBastionAllyCharacter::ToggleAIHUD(bool _val)
{

	if (AI_HUD)
	{
		AI_HUD->ToggleUI(_val, true);
	}
}

void ATheLastBastionAllyCharacter::OnGetUp()
{

	Super::OnGetUp();

	//GetCapsuleComponent()->SetCollisionProfileName("Ally");
	//GetMesh()->SetCollisionProfileName("HeroBody");

}
