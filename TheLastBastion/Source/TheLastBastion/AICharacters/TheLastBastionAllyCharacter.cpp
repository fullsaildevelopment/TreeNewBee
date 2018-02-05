// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionAllyCharacter.h"
#include "Combat/PawnStatsComponent.h"
#include "Combat/Gear.h"

#include "UI/InGameAIHUD.h"
#include "UI/InGameFloatingText.h"

#include "Kismet/GameplayStatics.h"

#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"

#include "Animation/AIBase_AnimInstance.h"

#include "TheLastBastionHeroCharacter.h"



ATheLastBastionAllyCharacter::ATheLastBastionAllyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->SetCollisionProfileName("Hero");
	GetMesh()->SetCollisionProfileName("HeroBody");

	AiName = FText::FromString(TEXT("Base Ally"));
	AILevel = 1;
}


void ATheLastBastionAllyCharacter::OnTakeAnyDamageHandle(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
}

void ATheLastBastionAllyCharacter::OnTakePointDamageHandle(AActor * DamagedActor, float Damage, AController * InstigatedBy, FVector HitLocation, UPrimitiveComponent * FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType * DamageType, AActor * DamageCauser)
{
	if (bIsDead)
		return;

	bool isCritical = false, isStun = false;

	float totalDamage = AIStats->CalculateDamage(Damage, DamageCauser, isCritical, isStun);

	/// if the attack is our hero, we will render the hp bar, the floating text for this AI
	const ATheLastBastionHeroCharacter* heroAttacker = Cast<ATheLastBastionHeroCharacter>(DamageCauser);
	if (heroAttacker)
	{
		UInGameAIHUD* aiHUD = Cast<UInGameAIHUD>(InfoHUD->GetUserWidgetObject());
		aiHUD->UpdateHealthBar(AIStats);
		aiHUD->ToggleUI(true, true);

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

	if (AIStats->GetHpCurrent() <= 0)
	{
		OnDead();
		return;
	}

	// if this ai is not get stunned, play hit animation
	mAnimInstanceRef->OnBeingHit(BoneName, ShotFromDirection, HitLocation);

	if (isStun)
	{
		// if this ai is not get simulate ragdoll physics, play hit animation

	}
	else
	{
		// if this ai is not get stunned, play hit animation
		mAnimInstanceRef->OnBeingHit(BoneName, ShotFromDirection, HitLocation);
	}

}

void ATheLastBastionAllyCharacter::ToggleAIHUD(bool _val)
{

	UInGameAIHUD* aiHUD = Cast<UInGameAIHUD>(InfoHUD->GetUserWidgetObject());

	if (aiHUD)
	{
		aiHUD->ToggleUI(_val, true);
	}
}
