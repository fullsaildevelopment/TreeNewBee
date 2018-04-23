// Copyright 1998-201	7 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TheLastBastionCharacter.generated.h"

#define FontSize_Regular 22
#define FontSize_Critical 26
#define RagDoll_RecoverLerpSpeed 5.0f
#define RagDoll_MinimumGetUpTime 2.0f

DECLARE_MULTICAST_DELEGATE(FOnBecomeUnavailbleTargetEvent);


UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	None 				     UMETA(DisplayName = "None"),
	Ranger 				     UMETA(DisplayName = "Ranger"),
	Builder 			     UMETA(DisplayName = "Builder"),
	LanTrooper_Rookie  	     UMETA(DisplayName = "Lannester Trooper"),
	LanTrooper_Fast          UMETA(DisplayName = "Lannester Legion"),
	LanTrooper_Power         UMETA(DisplayName = "Lannester Elite"),
	LanTrooper_Shield        UMETA(DisplayName = "Lannester Guardian"),
	LanTrooper_HeavyShield   UMETA(DisplayName = "Lannester UtliGuardian"),
	Lan_Heavy                UMETA(DisplayName = "Lannester Heavy"),
	Lan_QueenGuard           UMETA(DisplayName = "Lannester Queen Guard"),
	Lan_Shooter              UMETA(DisplayName = "Lannester Shooter"),
	Lan_SharpShooter         UMETA(DisplayName = "Lannester SharpShooter"),
	HillTribe_Rookie_LS      UMETA(DisplayName = "HillTribe Bandit"),
	HillTribe_Heavy_BA       UMETA(DisplayName = "HillTribe Barbarian"),
	HillTribe_Shooter        UMETA(DisplayName = "HillTribe Hunter"),
	Wolf_Shield_Mace         UMETA(DisplayName = "Wolf Patriot"),
	Wolf_HV_GS               UMETA(DisplayName = "Wolf Warrior"),
	Wolf_Fast_LS             UMETA(DisplayName = "Wolf Striker"),
	Nord_HeavyShield_Axe	 UMETA(DisplayName = "Nord Protecter"),
	Nord_Power_LS			 UMETA(DisplayName = "Nord Vanguard"),
	Nord_Shooter			 UMETA(DisplayName = "Nord SharpShooter"),
	Dwarven_HeavyShield_Mace UMETA(DisplayName = "Dwarven Champion"),
	Dwarven_Shooter			 UMETA(DisplayName = "Dwarven Sniper"),
	Ebony_Heavy_BA	 UMETA(DisplayName = "Ebony Legend")
};

UENUM(BlueprintType)
enum class ECharacterVoiceType : uint8
{
	Hero = 0  UMETA(DisplayName = "Hero"),
	Slim      UMETA(DisplayName = "Slim"),
	Strong    UMETA(DisplayName = "Strong"),
	Undead    UMETA(DisplayName = "Undead")
};

UCLASS(config=Game)
class ATheLastBastionCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay();

public:

	ATheLastBastionCharacter();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UAudioComponent* AudioComp;

	UPROPERTY()
	    class UPawnStatsComponent* PawnStats;

	UPROPERTY()
		class UBase_AnimInstance* mBaseAnimRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterType)
		ECharacterType CharacterType;

	UPROPERTY()
		/** Timer Handle after death, and get up*/
		FTimerHandle mRagDollTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = RagDoll)
		/** Toggle body velocity check after min ragdoll period*/
		bool bCheckBodyVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = RagDoll)
		uint8 newRagDollIndex;
				
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = RagDoll)
		uint8 oldRagDollIndex;

#pragma region Movement Stats


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = RagDoll)
		bool bIsRagDoll;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = RagDoll)
		bool bIsRecoveringFromRagDoll;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = RagDoll)
		float RagDollBlendWeight;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (BlueprintProtected))
		float SprintSpeed = 850.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (BlueprintProtected))
		float JogSpeed = 595.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (BlueprintProtected))
		float walkSpeed = 255.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (BlueprintProtected))
		float minTurnRate_Travel = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (BlueprintProtected))
		float maxTurnRate_Travel = 630.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (BlueprintProtected))
		float minTurnRate_Combat = 360.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (BlueprintProtected))
		float maxTurnRate_Combat = 1440.0f;

#pragma endregion

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BlueprintProtected))
	    bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected))
		bool bIsGodMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsEnemy;

protected:


	/** Config character based on character type during beginplay*/
	void CharacterCustomInit();

	//FVector GetRagDollPulse(const FVector& dir, const AActor* _damageCauser) const;


	UFUNCTION()
		virtual void OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType
			, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
		virtual void OnTakePointDamageHandle(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
			class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
			const class UDamageType* DamageType, AActor* DamageCauser);

	/** Turn character to ragdoll*/
	virtual void KnockOut(const FVector& dir, const AActor* _damageCauser, FName _boneName);

	/** Unregister all the group that I am a threat to it
	* put character to ragdoll, Called on Hp = 0 */
	virtual void OnDead(const FVector& dir, const AActor* _damageCauser,  FName _boneName);


	// Called ragdoll recover start
	virtual void OnGetUp();

public:
	
	FOnBecomeUnavailbleTargetEvent OnBecomeUnvailbleTargetEvent;


private:

	void ClampCapsuleToMesh();

	void ToggleBodyStopCheck();


public:

	FORCEINLINE bool IsEnemy() const { return bIsEnemy; }
	void DuringRagDoll();
	virtual void RagDollRecoverOnFinish();
	void DuringRagDollRecovering(float _deltaTime);
	FORCEINLINE bool IsOldKnockOut() const { return oldRagDollIndex == newRagDollIndex; }

	UFUNCTION(BlueprintNativeEvent)
		ECharacterVoiceType GetCharacterVoiceType() const;
	ECharacterVoiceType GetCharacterVoiceType_Implementation() const { return ECharacterVoiceType::Hero; }
	
	/** Check if this character is able to Parry this melee attack*/
	virtual bool OnParry(const struct FDamageInfo* const _damageInfo, 
		const class UPawnStatsComponent* const _damageCauserPawnStats) { return false; }

	/** Check if this character is able to automatically dodge this melee attack*/
	virtual bool OnAutoDodge(const struct FDamageInfo* const _damageInfo,
		const class UPawnStatsComponent* const _damageCauserPawnStats) { return false;}

	/** Check if this character is able to counter attack this attack*/
	virtual bool OnCounterAttack(const struct FDamageInfo* const _damageInfo,
		const class UPawnStatsComponent* const _damageCauserPawnStats) {return false;}

	// Called on actor destroyed
	virtual void Kill();


public:

	virtual bool IsStuned() const { return false; }

	FORCEINLINE float GetMaxTurnRateForTravel() const { return  maxTurnRate_Travel; }
	FORCEINLINE float GetMaxTurnRateForCombat() const { return  maxTurnRate_Combat; }

	FORCEINLINE float GetMinTurnRateForTravel() const { return  minTurnRate_Travel; }
	FORCEINLINE float GetMinTurnRateForCombat() const { return  minTurnRate_Combat; }

	FORCEINLINE float GetWalkSpeed() const { return walkSpeed; }
	FORCEINLINE float GetJogSpeed() const { return JogSpeed; }
	FORCEINLINE float GetSprintSpeed() const { return SprintSpeed; }

	FORCEINLINE ECharacterType GetCharacterType() const { return CharacterType; }
	FORCEINLINE class UPawnStatsComponent*  GetPawnStatsComp() const { return PawnStats; }
	FORCEINLINE const class UPawnStatsComponent*  GetPawnStatsComp_Const() const { return PawnStats; }
	FORCEINLINE void SetCharacterType(ECharacterType _val) { CharacterType = _val; }
	FORCEINLINE bool GetIsDead() const { return bIsDead; }
	FORCEINLINE bool GetIsGodMode() const { return bIsGodMode; }

	FORCEINLINE bool IsRagDoll() const { return bIsRagDoll; }
	FORCEINLINE bool IsRagDollRecovereing() const { return bIsRecoveringFromRagDoll; }
	FORCEINLINE class UAudioComponent* GetAudioComp() const { return AudioComp; }

	virtual bool ShouldPlayHitAnimation() const { return true; }
	

	// Called during Spawn, setup the ai level, so the its raw stats can use
	// its level to build the ai character stats
	void SetCharacterLevel(int _level);
	int GetCharacerLevel() const;


	bool IsOnDefend() const;
	bool IsUsingHeavyWeapon() const;

	float GetCurrentMaxSpeed() const;
	class AArmor* GetCurrentArmor() const;
	/** Get Current Right Hand Weapon*/
	class AGear* GetCurrentWeapon() const;
	/** Get Current Left Hand Weapon*/
	class AGear* GetCurrentSecondaryWeapon() const;

};

