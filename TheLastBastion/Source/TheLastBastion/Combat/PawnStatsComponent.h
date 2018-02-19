//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PawnStatsComponent.generated.h"



enum class EGearType : uint8;

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

	UPROPERTY()
		bool bIsProjectile;
};


USTRUCT(BlueprintType)
struct FWeaponSlot
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<class AGear> WeaponClass;

	UPROPERTY()
		class AGear* LeftHand;

	UPROPERTY()
		class AGear* RightHand;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bHideWhenEquip;
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

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Gear)
		TArray<FWeaponSlot> WeaponSlots;

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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		float CriticalRow;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		float StunRow;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		float CriticalMax;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		float StunMax;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		int Level;


#pragma endregion
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	//UFUNCTION()
	//	void OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType
	//	, class AController* InstigatedBy, AActor* DamageCauser);

	//UFUNCTION()
	//	void OnTakePointDamageHandle(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
	//		class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
	//		const class UDamageType* DamageType, AActor* DamageCauser);


	virtual float GetBaseDamage();

	void GenerateStatsAtBeginPlay();


public:	

	//void GenerateFloatingText(const FVector& _worldPos, bool _isCritical, bool _isStun, bool _isHeadHit);

	// Calculate the damage, remain health that this character suffered
	virtual float CalculateDamage(float baseDamage, AActor* _damageCauser, bool& _isCritical, bool& _isStun);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetEnableWeapon(bool _bIsEnabled, bool _bIsRightHand = true, bool _bIsAll = false);

	/** Attach the current weapon to the weapon slot*/
	void OnEquipWeapon();

	void OnSheathWeapon();

	void OnKill();

	virtual bool OnSwapBetweenMeleeAndRange();

	// Called after a character is spawned, generate the raw stats according to its level
	void GenerateRawStatsByLevel(int Level);

	// Called after equip and unequip 
	void GenerateMaxStats(bool _setCurrentToMax = true);

	// Called after character is level up, generater raw && max stats, recover full health
	void LevelUp();

	// Called 
	void Born();

	UFUNCTION()
		void ApplyDamage(const FDamageInfo& _hit);

public:

	AGear* GetCurrentArmor() const;

	FORCEINLINE float GetHpRaw() const { return HpRaw; }
	FORCEINLINE float GetStamina() const { return StaminaRaw; }
	FORCEINLINE float GetHpCurrent() const { return HpCurrent; }
	FORCEINLINE float GetStaminaCurrent() const { return StaminaCurrent; }
	FORCEINLINE float GetHpMax() const { return HpMax; }
	FORCEINLINE float GetStaminaMax() const { return StaminaMax; }
	FORCEINLINE float GetCriticalMax() const { return CriticalMax; }
	FORCEINLINE float GetStunMax() const { return StunMax; }

	FORCEINLINE float GetDivByHpMax() const { return DivByHpMax; }
	FORCEINLINE float GetDivBySpMax() const { return DivByStaminaMax; }
	FORCEINLINE int   GetLevel() const { return Level; }
	FORCEINLINE AGear* GetCurrentRightHandWeapon() const { return WeaponSlots[CurrentWeapon_Index].RightHand; }
	FORCEINLINE AGear* GetCurrentLeftHandWeapon() const { return WeaponSlots[CurrentWeapon_Index].LeftHand; }
	FORCEINLINE int GetMaxNumOfWeaponSlot() const { return WeaponSlots.Num(); }
	FORCEINLINE FWeaponSlot GetWeaponSlotAt(int _index) const { return WeaponSlots[_index]; }
	FORCEINLINE void SetWeaponEquipVisibility(int _index, bool _val) { WeaponSlots[_index].bHideWhenEquip = _val; }

	static TSubclassOf<class UUserWidget> GetFloatingText_WBP();

protected:


	void GetEffectsForImpact(class UParticleSystem*& _vfx, class USoundCue*& _sfx, int _surfaceType, EGearType _gearType) const;
	//// Calculate the health that this character left after being attacked
	//virtual float CalculateHealth(AActor* _otherActor);


};
