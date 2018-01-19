//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PawnStatsComponent.generated.h"


#define SURFACE_FLESH EPhysicalSurface::SurfaceType1
#define SURFACE_METAL EPhysicalSurface::SurfaceType2

UENUM(BlueprintType)
enum class EApplyDamageType : uint8
{
	None         = 0 UMETA(DisplayName = "None"),
	Common       = 1 UMETA(DisplayName = "Common"),
	Point        = 2 UMETA(DisplayName = "Point"),
	Radius       = 3 UMETA(DisplayName = "Radius"),
	RadiusFallOf = 4 UMETA(DisplayName = "RadiusFallOf")
};

USTRUCT()
struct FDamageInfo
{
	GENERATED_BODY()

	UPROPERTY()
		FHitResult hitResult;

	UPROPERTY()
		/** For shooting mechanim, unNormalized */
		FVector hitDirection;

	UPROPERTY()
		/** Is it a point, or radius or just common damage ?*/
		EApplyDamageType applyDamageType;

	UPROPERTY()
		/** Damage class depends on weapon */
		TSubclassOf<class UDamageType> damageType;
};


USTRUCT()
struct FWeaponSlot
{
	GENERATED_BODY()

	UPROPERTY()
	class AGear* LeftHand;

	UPROPERTY()
	class AGear* RightHand;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, const UPawnStatsComponent*, pawnStatsComp, float, damage, const class UDamageType*, _damageType, FName, _boneNmame, const FVector&, _shotFromDirection, const FVector&, _hitPosition);

UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THELASTBASTION_API UPawnStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPawnStatsComponent();

public:

	UPROPERTY(BlueprintAssignable)
		FOnHealthChangedSignature OnHealthChanged;


protected:

	bool bGenerateRawStatsAtBeginPlay;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Combat)
	    bool bArmedFromBeginPlay;

	UPROPERTY()
	class ATheLastBastionCharacter* mCharacter;

	UPROPERTY()
		FWeaponSlot WeaponSlots[2];

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Gear)
		TArray<TSubclassOf<class AGear>> WeaponWheels;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Gear)
		/** Index to the WeaponWheels*/
		int CurrentWeapon_Index;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Gear)
		TSubclassOf <class AArmor> Armor_ClassBp;

	UPROPERTY()
		/** Once the armor is updated, update all connected client this change*/
	class AArmor*    Armor;
	

#pragma region Character Stats

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = CharactorStats)
		// Hp without any armor
		float HpRaw;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = CharactorStats)
		// Sp without any armor
		float StaminaRaw;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		float HpMax;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		float HpCurrent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		float StaminaMax;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		float StaminaCurrent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		float DivByHpMax;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		float DivByStaminaMax;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		int Level;


#pragma endregion
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	UFUNCTION()
		void OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType
		, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
		void OnTakePointDamageHandle(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
			class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
			const class UDamageType* DamageType, AActor* DamageCauser);

	// Calculate the damage that this character cause as the attacker
	virtual float CalculateDamage();

	virtual float GetBaseDamage();


	void GenerateStatsAtBeginPlay();

	class UInGameFloatingText* GenerateFloatingText(const FVector& _worldPos);

public:	

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetEnableWeapon(bool _bIsEnabled, bool _bIsRightHand = true, bool _bIsAll = false);

	/** Attach the current weapon to the weapon slot*/
	void OnEquipWeapon();

	void OnSheathWeapon();

	void OnKill();

	virtual bool OnSwapBetweenMeleeAndRange();


public:

	static int GetMaxWeaponSlot();
	FORCEINLINE float GetHpRaw() const { return HpRaw; }
	FORCEINLINE float GetStamina() const { return StaminaRaw; }
	FORCEINLINE float GetHpCurrent() const { return HpCurrent; }
	FORCEINLINE float GetStaminaCurrent() const { return StaminaCurrent; }
	FORCEINLINE float GetHpMax() const { return HpMax; }
	FORCEINLINE float GetStaminaMax() const { return StaminaMax; }
	FORCEINLINE float GetDivByHpMax() const { return DivByHpMax; }
	FORCEINLINE float GetDivBySpMax() const { return DivByStaminaMax; }
	FORCEINLINE int   GetLevel() const { return Level; }
	FORCEINLINE AGear* GetCurrentRightHandWeapon() const { return WeaponSlots[CurrentWeapon_Index].RightHand; }

	// Called after a character is spawned, generate the raw stats according to its level
	void GenerateRawStatsByLevel(int Level);

	// Called after equip and unequip 
	void GenerateMaxStats(bool _setCurrentToMax = true);

	// Called after character is level up, generater raw && max stats, recover full health
	void LevelUp();
	
	// Called 
	void Born();

	UFUNCTION()
		void ApplyDamage( const FDamageInfo& _hit);

protected:

	// Calculate the health that this character left after being attacked
	virtual float CalculateHealth(AActor* _otherActor);


};
