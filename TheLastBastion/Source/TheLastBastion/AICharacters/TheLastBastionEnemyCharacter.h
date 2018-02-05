// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacters/TheLastBastionAIBase.h"
#include "TheLastBastionEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API ATheLastBastionEnemyCharacter : public ATheLastBastionAIBase
{
	GENERATED_BODY()
	
public:

	ATheLastBastionEnemyCharacter();

protected:

	UPROPERTY(VisibleAnywhere, Category = AiHud)
		bool  bAIHUDisDisplayedForLockedOn;

protected:

	UFUNCTION()
		void OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType
			, class AController* InstigatedBy, AActor* DamageCauser) override;
	UFUNCTION()
		void OnTakePointDamageHandle(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
			class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
			const class UDamageType* DamageType, AActor* DamageCauser) override;

public:

	/** Toggle the AI hud to screen*/
	void ToggleAIHUD(bool _val) override;



};
