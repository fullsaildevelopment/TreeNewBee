//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PawnStatsComponent.generated.h"


#define DpPhysicalDamageReduction 0.5f
#define RangerInitHp 230.0f


enum class EGearType : uint8;
enum class ECharacterType :uint8;

UENUM(BlueprintType)
enum class EApplyDamageType : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Common = 1 UMETA(DisplayName = "Common"),
	Point = 2 UMETA(DisplayName = "Point"),
	Radius = 3 UMETA(DisplayName = "Radius"),
	RadiusFallOf = 4 UMETA(DisplayName = "RadiusFallOf")
};

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

	UPROPERTY()
		FHitResult hitResult;

	UPROPERTY()
		/** Normalized damageCauserRelative*/
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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Combat)
		bool bUsingLeftHandWeapon;

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
		// Hp & Dp without any armor
		float HpRaw;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = CharactorStats)
		// Sp without any armor
		float StaminaRaw;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		// currrent maximum Hp & Dp
		float HpMax;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		// currrent Hp
		float HpCurrent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		// currrent Dp
		float DpCurrent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		float StaminaMax;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		float StaminaCurrent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		// divider for current max dp or hp
		float DivByHpMax;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		float DivByStaminaMax;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		float CriticalRow;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		float StunRow;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		float CriticalMax;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		float StunMax;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		/** character base damage that is improved by level*/
		float BaseDamage;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CharacterStats)
		int Level;


#pragma endregion
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	virtual float GetBaseDamage();

	void GenerateStatsAtBeginPlay();

	// Calculate and output the raw stats according to its level
	void GenerateRawStatsByLevel(int Level, float& _baseDamage, float& _hpRaw);


public:	

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

	void CalculateRawStatsByType(int _level, ECharacterType _type, float& damage, float& _hp);

	// Called after equip and unequip 
	void GenerateMaxStats(bool _setCurrentToMax = true);

	// Called after character is level up, generater raw && max stats, recover full health
	void LevelUp();

	// Called 
	void Born();

	virtual bool ApplyDamage(const FDamageInfo& _hit);

	bool IsUsingHeavyWeapon() const;

public:

	class AArmor* GetCurrentArmor() const;

	FORCEINLINE float GetHpRaw() const { return HpRaw; }
	FORCEINLINE float GetStamina() const { return StaminaRaw; }
	FORCEINLINE float GetHpCurrent() const { return HpCurrent; }
	FORCEINLINE float GetDpCurrent() const { return DpCurrent; }
	FORCEINLINE float GetStaminaCurrent() const { return StaminaCurrent; }
	/** Get Current Max Dp or Hp*/
	FORCEINLINE float GetHpMax() const { return HpMax; }
	FORCEINLINE float GetStaminaMax() const { return StaminaMax; }
	FORCEINLINE float GetCriticalMax() const { return CriticalMax; }
	FORCEINLINE float GetStunMax() const { return StunMax; }
	FORCEINLINE float GetDivByHpMax() const { return DivByHpMax; }
	FORCEINLINE float GetDivBySpMax() const { return DivByStaminaMax; }
	FORCEINLINE int   GetCharacterLevel() const { return Level; }
	FORCEINLINE bool IsFullHp() const { return HpCurrent == HpMax; }
	FORCEINLINE bool IsFullSp() const { return StaminaCurrent == StaminaMax; }

	/** Check if the left hand weapon is current use to deal damage */
	FORCEINLINE bool IsUsingLeftWeapon() const { return bUsingLeftHandWeapon; }
	FORCEINLINE AGear* GetCurrentRightHandWeapon() const { return WeaponSlots[CurrentWeapon_Index].RightHand; }
	FORCEINLINE AGear* GetCurrentLeftHandWeapon() const { return WeaponSlots[CurrentWeapon_Index].LeftHand; }
	FORCEINLINE AGear* GetCurrentActivatedWeapon() const { return bUsingLeftHandWeapon? GetCurrentLeftHandWeapon() : GetCurrentRightHandWeapon(); }
	FORCEINLINE int GetMaxNumOfWeaponSlot() const { return WeaponSlots.Num(); }
	FORCEINLINE FWeaponSlot GetWeaponSlotAt(int _index) const { return WeaponSlots[_index]; }
	static TSubclassOf<class UUserWidget> GetFloatingText_WBP();


	FORCEINLINE void SetWeaponEquipVisibility(int _index, bool _val) { WeaponSlots[_index].bHideWhenEquip = _val; }

	FORCEINLINE void SetCharacterLevel(int _level) { Level = _level; }


	/** Change hp by value, clamp is involved*/
	FORCEINLINE void AddHpBy(float _delta) { HpCurrent = FMath::Clamp(HpCurrent + _delta, 0.0f, HpMax); }
	/** Change dp by value, clamp is involved*/
	FORCEINLINE void AddDpBy(float _delta) { DpCurrent = FMath::Clamp(DpCurrent + _delta, 0.0f, HpMax); }
	/** Change sp by value, clamp is involved*/
	FORCEINLINE void AddSpBy(float _delta) { StaminaCurrent = FMath::Clamp(StaminaCurrent + _delta, 0.0f, StaminaMax); }

	/** Change hp by percentage, clamp is involved*/
	FORCEINLINE void AddHpByPercent(float _delta) { HpCurrent = FMath::Clamp(HpCurrent + HpMax *_delta, 0.0f, HpMax); }
	/** Change dp by percentage, clamp is involved*/
	FORCEINLINE void AddDpByPercent(float _delta) { DpCurrent = FMath::Clamp(DpCurrent + HpMax * _delta, 0.0f, HpMax); }
	/** Change sp by percentage, clamp is involved*/
	FORCEINLINE void AddSpByPercent(float _delta) { StaminaCurrent = FMath::Clamp(StaminaCurrent + StaminaMax *_delta, 0.0f, StaminaMax); }

	FORCEINLINE void SetSp(float _val) { StaminaCurrent = _val; }


protected:
	void PlaySFXForImpact(class USoundCue* _sfx, int _surfaceType, ATheLastBastionCharacter* _damagedCharacter) const;
};
