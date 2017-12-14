// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gear.generated.h"

UENUM(BlueprintType)
enum class EGearType : uint8
{
	Armor = 0 UMETA(DisplayName = "Armor"),
	Shield = 1 UMETA(DisplayName = "Shield"),
	SingleHandWeapon = 2 UMETA(DisplayName = "SingleHandWeapon"),
	DoubleHandWeapon = 3 UMETA(DisplayName = "DoubleHandWeapon"),
	CrossBow = 4 UMETA(DisplayName = "CrossBow")

};

UCLASS()
class THELASTBASTION_API AGear : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGear();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* Mesh;
	

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		EGearType GearType;


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float PhysicalDamage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float ElementalDamage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float MagicalDamage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float PhysicalDefence;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float ElementalDefence;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float MagicalDefence;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float HpBonus;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float StaminaBonus;
	
};
