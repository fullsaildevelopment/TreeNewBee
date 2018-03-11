// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacters/TheLastBastionAIBase.h"
#include "TheLastBastionAllyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API ATheLastBastionAllyCharacter : public ATheLastBastionAIBase
{
	GENERATED_BODY()


	
public:

	ATheLastBastionAllyCharacter();

protected:

	void BeginPlay() override;


	void HitResponse(AActor* DamageCauser) override;

	void OnTakeDamageFromHero(const FVector& HitLocation, const class ATheLastBastionHeroCharacter* heroAttacker,
		float totalDamage, bool isCritical, bool isStun) override;

	void EvaluateAttackerThreat(AActor* DamageCauser, float hp) override;


public:

	/** Toggle the AI hud to screen*/
	void ToggleAIHUD(bool _val) override;

	void OnGetUp() override;

};
