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



#define ECC_EnemyBody ECollisionChannel::ECC_GameTraceChannel3
#define ECC_HeroBody  ECollisionChannel::ECC_GameTraceChannel1

#define ECC_EnemyWeapon ECollisionChannel::ECC_GameTraceChannel4
#define ECC_HeroWeapon  ECollisionChannel::ECC_GameTraceChannel2



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

	//UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = UI)
	//	/** Visual appearence as UI element*/
	//	UTexture2D* ThumbNail;

private:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float PhysicalDamage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float PhysicalDefence;

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
		float Force;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float CriticalForce;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		EElementalType ElementalType;

public:

	FORCEINLINE class ATheLastBastionCharacter* GetGearOwner() const { return GearOwner; }
	FORCEINLINE TSubclassOf<AGear> GetLeftHandGear() const { return LeftHand; }
	FORCEINLINE EGearType GetGearType() const { return GearType; }
	FORCEINLINE virtual class USceneComponent* GetMesh() const { return nullptr; }
	FORCEINLINE virtual class UParticleSystemComponent* GetParticleSystemComp() const { return nullptr; }
	FORCEINLINE TSubclassOf<class UDamageType> GetDamageType() { return DamageType; }

	FORCEINLINE float GetHpBonus() const { return HpBonus;}
	FORCEINLINE float GetStaminaBonus() const { return StaminaBonus; }
	FORCEINLINE float GetPhysicalDamage() const { return PhysicalDamage; }
	//FORCEINLINE float GetElementalDamage() const { return ElementalDamage; }
	//FORCEINLINE float GetFireDamage() const { return FireDamage; }
	//FORCEINLINE float GetIceDamage() const { return IceDamage; }
	FORCEINLINE float GetPhysicalDefence() const { return PhysicalDefence; }
	//FORCEINLINE float GetElementalDefence() const { return ElementalDefence; }
	FORCEINLINE float GetFireDefence() const { return FireDefence; }
	FORCEINLINE float GetIceDefence() const { return IceDefence; }
	FORCEINLINE float GetHpAdditive() const { return HpAdditive; }
	FORCEINLINE float GetSpAdditive() const { return SpAdditive; }
	FORCEINLINE float GetCriticalChance() const { return CriticalChance; }
	FORCEINLINE float GetStunChance() const { return StunChance; }
	FORCEINLINE float GetForce() const { return Force; }
	FORCEINLINE float GetCriticalForce() const { return CriticalForce; }
	FORCEINLINE EElementalType GetElementalType() const { return ElementalType; }

	////UFUNCTION(BlueprintPure)
	////	FORCEINLINE FText GetGearName() const { return Name; }

	//UFUNCTION(BlueprintPure)
	//	FORCEINLINE UTexture2D* GetThumbNailImage() const { return ThumbNail; }

	UFUNCTION(BlueprintNativeEvent)
		FORCEINLINE UTexture2D* GetThumbNailImage() const;
	FORCEINLINE UTexture2D* GetThumbNailImage_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintNativeEvent)
		FORCEINLINE FText GetGearName() const;
	FORCEINLINE FText GetGearName_Implementation() const { return FText::FromString("Gear Name"); }

	UFUNCTION(BlueprintNativeEvent)
		FORCEINLINE FText GetGearDescription() const;
	FORCEINLINE FText GetGearDescription_Implementation() const { return FText::FromString("Gear Description"); }

	UFUNCTION(BlueprintNativeEvent)
		/** Return the metal and wood cost to craft this weapon*/
		 TArray<int> GetResourceCost() const;
	TArray<int> GetResourceCost_Implementation() const;

	virtual void SetDamageIsEnabled(bool _val);

	virtual void Equip(class USkeletalMeshComponent* const _skeletonMeshComponent);
	virtual void Arm(class USkeletalMeshComponent* const _skeletonMeshComponent);
	void ToggleVisibilty(bool _val);
	//void CombineDamage(const AGear* _other);

};


