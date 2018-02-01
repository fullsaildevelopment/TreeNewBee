// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gear.generated.h"

UENUM(BlueprintType)
enum class EGearType : uint8
{
	Armor = 0                  UMETA(DisplayName = "Armor"),
	Shield = 1                 UMETA(DisplayName = "Shield"),
	LongSword = 2              UMETA(DisplayName = "LongSword"),
	DoubleHandWeapon = 3       UMETA(DisplayName = "Katana"),
	CrossBow = 4               UMETA(DisplayName = "CrossBow"),
	CrossBowBolt = 5           UMETA(DisplayName = "CrossBowBolt"),
	TwinBlade = 6              UMETA(DisplayName = "TwinBlade"),
	GreatSword = 7             UMETA(DisplayName = "GreatSword"),
	Bow = 8                    UMETA(DisplayName = "Bow"),
	Arrow = 9                  UMETA(DisplayName = "Arrow"),
	WarAxe = 10                UMETA(DisplayName = "WarAxe"),
	Mace = 11                  UMETA(DisplayName = "Mace"),
	BattleAxe = 12             UMETA(DisplayName = "BattleAxe"),
	Hammer = 13                UMETA(DisplayName = "Hammer"),
};

UENUM()
enum class EElementalType
{
	None = 0,
	Fire,
	Ice
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

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		/** When this gear is used as right hand gear, should we put other gear on the left hand*/
		TSubclassOf<AGear> LeftHand;

	UPROPERTY(EditDefaultsOnly, Category = DamageCalculation)
		TSubclassOf <class UDamageType> DamageType;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = UI)
		/** Visual appearence as UI element*/
		UTexture2D* ThumbNail;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = UI)
		/** Name Showed Up On UI */
		FText Name;

private:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float PhysicalDamage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float ElementalDamage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float FireDamage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float IceDamage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float PhysicalDefence;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float ElementalDefence;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float FireDefence;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float IceDefence;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		/** Percentage */
		float HpBonus;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float StaminaBonus;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		/** Additive Hp on the max Hp*/
	    float HpAdditive;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		/** Additive Sp on the max Hp*/
		float SpAdditive;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float CriticalChance;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float StunChance;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		EElementalType ElementalType;



public:


	FORCEINLINE class ATheLastBastionCharacter* GetGearOwner() const { return GearOwner; }
	FORCEINLINE TSubclassOf<AGear> GetLeftHandGear() const { return LeftHand; }
	FORCEINLINE EGearType GetGearType() const { return GearType; }
	FORCEINLINE virtual class USceneComponent* GetMesh() const { return nullptr; }
	FORCEINLINE TSubclassOf<class UDamageType> GetDamageType() { return DamageType; }

	FORCEINLINE float GetHpBonus() const { return HpBonus;}
	FORCEINLINE float GetStaminaBonus() const { return StaminaBonus; }
	FORCEINLINE float GetPhysicalDamage() const { return PhysicalDamage; }
	FORCEINLINE float GetElementalDamage() const { return ElementalDamage; }
	FORCEINLINE float GetFireDamage() const { return FireDamage; }
	FORCEINLINE float GetIceDamage() const { return IceDamage; }
	FORCEINLINE float GetPhysicalDefence() const { return PhysicalDefence; }
	FORCEINLINE float GetElementalDefence() const { return ElementalDefence; }
	FORCEINLINE float GetFireDefence() const { return FireDefence; }
	FORCEINLINE float GetIceDefence() const { return IceDefence; }
	FORCEINLINE float GetHpAdditive() const { return HpAdditive; }
	FORCEINLINE float GetSpAdditive() const { return SpAdditive; }
	FORCEINLINE float GetCriticalChance() const { return CriticalChance; }
	FORCEINLINE float GetStunChance() const { return StunChance; }
	FORCEINLINE EElementalType GetElementalType() const { return ElementalType; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE FText GetGearName() const { return Name; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE UTexture2D* GetThumbNailImage() const { return ThumbNail; }

	virtual void SetDamageIsEnabled(bool _val);


	virtual void Equip(class USkeletalMeshComponent* const _skeletonMeshComponent);
	virtual void Arm(class USkeletalMeshComponent* const _skeletonMeshComponent);
	void ToggleVisibilty(bool _val);
	void CombineDamage(const AGear* _other);

};


