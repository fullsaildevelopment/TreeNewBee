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
	CrossBow = 4 UMETA(DisplayName = "CrossBow"),
	CrossBowBolt = 5 UMETA(DisplayName = "CrossBowBolt"),
	TwinBlade = 6 UMETA(DisplayName = "TwinBlade")
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

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true", BlueprintProtected = "true"))
		EGearType GearType;
	UPROPERTY(BlueprintReadOnly)
		class ATheLastBastionCharacter* GearOwner;

private:

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

	// if stamina = -1, that means this character has infinite stamina
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float StaminaBonus;
	
public:

	virtual void Equip(class USkeletalMeshComponent* const _skeletonMeshComponent);

	virtual void Arm(class USkeletalMeshComponent* const _skeletonMeshComponent);
	
	FORCEINLINE class ATheLastBastionCharacter* GetGearOwner() const { return GearOwner; }

	FORCEINLINE float GetHpBonus() const { return HpBonus;}
	FORCEINLINE float GetStaminaBonus() const { return StaminaBonus; }
	FORCEINLINE float GetPhysicalDamage() const { return PhysicalDamage; }
	FORCEINLINE float GetElementalDamage() const { return ElementalDamage; }
	FORCEINLINE float GetMagicalDamage() const { return MagicalDamage; }
	FORCEINLINE float GetPhysicalDefence() const { return PhysicalDefence; }
	FORCEINLINE float GetElementalDefence() const { return ElementalDefence; }
	FORCEINLINE float GetMagicalDefence() const { return MagicalDefence; }






};


