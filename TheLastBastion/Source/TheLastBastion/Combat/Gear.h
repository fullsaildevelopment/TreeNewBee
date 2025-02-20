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
	DaiKatana = 14             UMETA(DisplayName = "DaiKatana"),
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

	static TSubclassOf <class UDamageType> ComboBullets_Type;
	static TSubclassOf <class UDamageType> NormalBullets_Type;

	//UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = UI)
	//	/** Visual appearence as UI element*/
	//	UTexture2D* ThumbNail;

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float PhysicalDamage;

	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	//	float PhysicalDefence;

	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	//	float FireDefence;

	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	//	float IceDefence;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		/** Percentage */
		float HpBonus;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float StaminaBonus;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		/** Additive Hp on the max Hp*/
	    float HpAdditive;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		/** Additive Sp on the max Hp*/
		float SpAdditive;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float CriticalChance;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float StunChance;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float Force;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float CriticalForce;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//	bool bIsUnlocked;

	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	//	EElementalType ElementalType;

public:

	FORCEINLINE class ATheLastBastionCharacter* GetGearOwner() const { return GearOwner; }
	FORCEINLINE TSubclassOf<AGear> GetLeftHandGear() const { return LeftHand; }
	FORCEINLINE EGearType GetGearType() const { return GearType; }
	FORCEINLINE virtual class USceneComponent* GetMesh() const { return nullptr; }
	FORCEINLINE virtual class UParticleSystemComponent* GetParticleSystemComp() const { return nullptr; }
	FORCEINLINE TSubclassOf<class UDamageType> GetDamageType() { return DamageType; }

	
	FORCEINLINE float GetHpBonus() const { return HpBonus;}
	FORCEINLINE float GetStaminaBonus() const { return StaminaBonus; }
	FORCEINLINE float GetHpBonus_uint() const { return HpBonus * 0.01f; }
	FORCEINLINE float GetStaminaBonus_uint() const { return StaminaBonus * 0.01f; }


	FORCEINLINE float GetPhysicalDamage() const { return PhysicalDamage; }
	//FORCEINLINE float GetElementalDamage() const { return ElementalDamage; }
	//FORCEINLINE float GetFireDamage() const { return FireDamage; }
	//FORCEINLINE float GetIceDamage() const { return IceDamage; }
	//FORCEINLINE float GetPhysicalDefence() const { return PhysicalDefence; }
	////FORCEINLINE float GetElementalDefence() const { return ElementalDefence; }
	//FORCEINLINE float GetFireDefence() const { return FireDefence; }
	//FORCEINLINE float GetIceDefence() const { return IceDefence; }
	FORCEINLINE float GetHpAdditive() const { return HpAdditive; }
	FORCEINLINE float GetSpAdditive() const { return SpAdditive; }
	FORCEINLINE float GetCriticalChance() const { return CriticalChance; }
	FORCEINLINE float GetStunChance() const { return StunChance; }
	FORCEINLINE float GetCriticalChance_unit() const { return CriticalChance * 0.01f; }
	FORCEINLINE float GetStunChance_unit() const { return StunChance * 0.01f; }
	FORCEINLINE float GetForce() const { return Force; }
	FORCEINLINE float GetCriticalForce() const { return CriticalForce; }
	//FORCEINLINE bool GetIsUnlocked() const { return bIsUnlocked; }
	////FORCEINLINE EElementalType GetElementalType() const { return ElementalType; }
	//FORCEINLINE void Unlock()  {  bIsUnlocked = true; }

	////UFUNCTION(BlueprintPure)
	////	FORCEINLINE FText GetGearName() const { return Name; }

	//UFUNCTION(BlueprintPure)
	//	FORCEINLINE UTexture2D* GetThumbNailImage() const { return ThumbNail; }

	UFUNCTION(BlueprintNativeEvent)
		 UTexture2D* GetThumbNailImage() const;
	UTexture2D* GetThumbNailImage_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintNativeEvent)
		 FText GetGearName() const;
	FText GetGearName_Implementation() const { return FText::FromString("Gear Name"); }

	UFUNCTION(BlueprintNativeEvent)
		 FText GetGearDescription() const;
	FText GetGearDescription_Implementation() const { return FText::FromString("Gear Description"); }

	UFUNCTION(BlueprintNativeEvent)
		/** Return the metal and wood cost to craft this weapon*/
		 TArray<int> GetResourceCost() const;
	TArray<int> GetResourceCost_Implementation() const;

	virtual void SetDamageIsEnabled(bool _val);

	virtual void Equip(class USkeletalMeshComponent* const _skeletonMeshComponent);
	virtual void Arm(class USkeletalMeshComponent* const _skeletonMeshComponent);
	void ToggleVisibilty(bool _val);


	virtual class UParticleSystemComponent* OnWeaponEnchant() { return nullptr; }
	//void CombineDamage(const AGear* _other);

protected:

	/** Scale other parametar that is not covered by engine*/
	virtual void MaintainScale() {};


};


