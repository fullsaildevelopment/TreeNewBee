// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheLastBastionCharacter.h"
#include "TheLastBastionAIBase.generated.h"

/**
 * 
 */

#define RangeUnitMinimumFirePlayBackTime 3.0f
#define AICharacter_RotatingRate 360.0f

#define HV_ComboSel_InPlace_Jr_Min 0
#define HV_ComboSel_InPlace_Jr_Max 2

#define HV_ComboSel_Move_Jr_Min 4
#define HV_ComboSel_Move_Jr_Max 6

#define HV_ComboSel_InPlace_Sr_Min 0
#define HV_ComboSel_InPlace_Sr_Max 3

#define HV_ComboSel_Move_Sr_Min 4
#define HV_ComboSel_Move_Sr_Max 9

#define HV_Combo_Counter_Jr_Min 1
#define HV_Combo_Counter_Jr_Max 2

#define HV_Combo_Counter_Sr_Min 2
#define HV_Combo_Counter_Sr_Max 3


#define SH_Roo_InPlace_Left_Min 0
#define SH_Roo_InPlace_Left_Max 1
#define SH_Roo_InPlace_Right_Min 2
#define SH_Roo_InPlace_Right_Max 4
#define SH_Roo_Move_Left_Min 5
#define SH_Roo_Move_Left_Max 6
#define SH_Roo_Move_Right_Min 7
#define SH_Roo_Move_Right_Max 9

#define SH_Fast_InPlace_Left_Min 0
#define SH_Fast_InPlace_Left_Max 2
#define SH_Fast_InPlace_Right_Min 3
#define SH_Fast_InPlace_Right_Max 6
#define SH_Fast_Move_Min 7
#define SH_Fast_Move_Max 11

#define SH_Pow_InPlace_Left_Min 4
#define SH_Pow_InPlace_Left_Max 6
#define SH_Pow_InPlace_Right_Min 0
#define SH_Pow_InPlace_Right_Max 3
#define SH_Pow_Move_Left_Min 7
#define SH_Pow_Move_Left_Max 9
#define SH_Pow_Move_Right_Min 10
#define SH_Pow_Move_Right_Max 12

#define SH_Roo_Combo_Counter_Min 1
#define SH_Roo_Combo_Counter_Max 2

#define SH_PowFast_Combo_Counter_Min 5
#define SH_PowFast_Combo_Counter_Max 8
#define SH_Pow_Parry_End_Min 3
#define SH_Pow_Parry_End_Max 4
#define SH_Fast_Dodge_End_Min 2
#define SH_Fast_Dodge_End_Max 4

#define Sns_Combo_Counter_Min 1
#define Sns_Combo_Counter_Max 3
#define Sns_Ulti_Combo_Counter_Min 3
#define Sns_Ulti_Combo_Counter_Max 4

#define Sns_Parry_End_Min 3
#define Sns_Parry_End_Max 5

#define Sns_Ulti_InPlace_Left_Min 4
#define Sns_Ulti_InPlace_Left_Max 6
#define Sns_Ulti_InPlace_Right_Min 0
#define Sns_Ulti_InPlace_Right_Max 3
#define Sns_Ulti_Move_Min 7
#define Sns_Ulti_Move_Max 9
#define Sns_Ulti_Counter_Min 2
#define Sns_Ulti_Counter_Max 3

#define Sns_Ulti_ShieldBash 10

#define Sns_InPlace_Min 0
#define Sns_InPlace_Max 2
#define Sns_Move_Min 3
#define Sns_Move_Max 5

#define Parry_Up_Right 0 
#define Parry_Up_Left 1 
#define Parry_Top_Left 2 
#define Parry_Right_Down_Weak 3 
#define Parry_Left_Mid_Down_Weak 4 
#define Parry_Left_Mid_Up_Weak 5 
#define Parry_Top_Mid_Weak 6 


#define Block_Top_Down_Right 7 
#define Block_Top_Up_Mid 8
#define Block_Down 9 
#define Block_Right_Top_Weak 10 
#define Block_Right_Mid_Up_Weak 11 
#define Block_Left_Top_Weak 12 

#define Easy_Parry_Threshold 2

const FName SnsParryAndBlockSectionName[13] =
{ FName("Parry_Up_Right"),FName("Parry_Up_Left"), FName("Parry_Top_Left"), FName("Parry_Right_Down_Weak"),
FName("Parry_Left_Mid_Down_Weak") , FName("Parry_Left_Mid_Up_Weak"),FName("Parry_Top_Mid_Weak"),
FName("Block_Top_Down_Right"),FName("Block_Top_Up_Mid") ,FName("Block_Down"),
FName("Block_Right_Top_Weak"), FName("Block_Right_Mid_Up_Weak"), FName("Block_Left_Top_Weak") };

UCLASS()
class THELASTBASTION_API ATheLastBastionAIBase : public ATheLastBastionCharacter
{
	GENERATED_BODY()
	
public:

	ATheLastBastionAIBase();

protected:

	//UPROPERTY(EditDefaultsOnly, Category = AiProperty)
	//	FText AiName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		class UPawnStatsComponent* AIStats;

	UPROPERTY()
		class UAIBase_AnimInstance* mAnimInstanceRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Behavior)
		class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AiHud)
		class UWidgetComponent* InfoHUD;

	UPROPERTY()
		class AAIGroupBase* mGroup;

	UPROPERTY()
		class UInGameAIHUD* AI_HUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GroupBehavior)
		FVector ChildtargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GroupBehavior)
		int mGroupIndex;

	UPROPERTY(EditDefaultsOnly, Category = AiProperty)
		int AITier;

	UPROPERTY(EditAnywhere, Category = AiProperty)
		float MeleeAttackDistSq;

	UPROPERTY(EditAnywhere, Category = AiProperty)
		float MinimumMoveAttackDistSq;

	UPROPERTY(EditAnywhere, Category = Behavior)
		/** The waiting time between each attack */
		float AttackWait;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Behavior)
		/** updated value to judge when parry should happend or stop*/
		int ParryEndurance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Behavior)
		/** updated value to judge when dodge should happend or stop*/
		int DodgeEndurance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Behavior)
		/** updated value to judge when counter should happend or stop*/
		int CounterEndurance;

	UPROPERTY(VisibleAnywhere, Category = Behavior)
		/** Should we attack from right ? A toggled variable to control ai to attack from different direction */
		bool bAttackFromRight;

	UPROPERTY(EditDefaultsOnly, Category = AiProperty)
		bool bIsRangeUnit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		/* if true, this character will be walking instead of jogging*/
		bool bIsWalkingUnit;



protected:

	void BeginPlay() override;

public:



	UFUNCTION(BlueprintNativeEvent)
		UTexture2D* GetThumbNailImage() const;
	UTexture2D* GetThumbNailImage_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintNativeEvent)
		float GetSiegePoint() const;
	float GetSiegePoint_Implementation() const { return 1.0f; }

	UFUNCTION(BlueprintNativeEvent)
		/** The amount of exp that hero gain, when this character is defeated*/
		float GetExperience() const;
	float GetExperience_Implementation() const { return 5.0f; }


	UFUNCTION(BlueprintNativeEvent)
		/** The amount of resouce that cost hero to hire this character
		0 = food, 1 = metal, 2 = wood */
		TArray<int> GetResourceCost() const;
	TArray<int> GetResourceCost_Implementation() const;

	UFUNCTION(BlueprintNativeEvent)
		FText GetAIName() const;
	FText GetAIName_Implementation() const { return FText::FromString("AI Name"); }

	UFUNCTION(BlueprintNativeEvent)
		FText GetAIDescription() const;
	FText GetAIDescription_Implementation() const { return FText::FromString("AI Description"); }

	UFUNCTION()
		virtual void OnTargetDeathHandle();



	/// Melee Combo Interface
	FORCEINLINE float GetMinimumMoveAttackDist_Sq() const { return MinimumMoveAttackDistSq; }
	FORCEINLINE float GetMeleeAttackDist_Sq() const { return MeleeAttackDistSq; }

	/** Get Melee Attack selection by distance, toggle bAttackFromRight */
	int MeleeComboSelection(float _distSq); 

	/** Get How many combos the ai character will do in a row*/
	FORCEINLINE virtual int GetComboCounter() const { return 1; }
	/** Get how many hit can this AI endurance before it has to parry this attack*/
	FORCEINLINE virtual int GetParryEndurance() const { return 1; }
	/** Get how many hit can this AI endurance before it has to dodge this attack*/
	FORCEINLINE virtual int GetAutoDodgeEndurance() const { return 1; }
	/** Get how many hit can this AI endurance before it counter this attack*/
	FORCEINLINE virtual int GetCounterEndurance() const { return 1; }

	/** Called during the start of ai melee combo, if there is anything needs to reset*/
	virtual void ClearEndurance() {}

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsRangeUnit() const { return bIsRangeUnit; }


	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsNearTargetLocation(float _distSq) const { return FVector::DistSquared2D(GetActorLocation(), ChildtargetLocation) < _distSq; }


	UFUNCTION(BlueprintPure)
		FORCEINLINE class UPawnStatsComponent* GetEnemyStatsComponent() const { return AIStats; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class UAIBase_AnimInstance* GetAnimInstanceRef() const { return mAnimInstanceRef; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE class AAIGroupBase* GetGroup() const { return mGroup; }

	/** Get Waiting Time between two attack */
	FORCEINLINE float GetAttackWait() const { return (bIsRangeUnit)? AttackWait : (AttackWait + RangeUnitMinimumFirePlayBackTime); }
	/** Check if this ai is walking or jogging*/
	FORCEINLINE bool IsWalking() const { return bIsWalkingUnit; }

	FORCEINLINE void SetGroupIndex(int _groupIndex) { mGroupIndex = _groupIndex; }

	UFUNCTION(BlueprintCallable)
			bool OnGroupTaskStart();

	/** Toggle the AI hud to screen*/
	virtual void ToggleAIHUD(bool _val);

	bool HasFullHealth() const;


	bool IsStuned() const override;

	bool OnFriendFireCheck(const ATheLastBastionCharacter* _target);

	/** Set target to this AI, default to binding the target request delegate*/
	void SetTarget(AActor* _target, bool _asGroupMember = false);

	void SetParent(class AAIGroupBase* _Group, int _groupIndex);

	AActor* GetTarget() const;

	void RequestAnotherTarget();

	// Called on actor destroyed
	void Kill() override;

protected:


	UFUNCTION()
		void OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType
			, class AController* InstigatedBy, AActor* DamageCauser) override;
	UFUNCTION()
		void OnTakePointDamageHandle(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
			class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
			const class UDamageType* DamageType, AActor* DamageCauser) override;

	/** Generate floating text to represent damage*/
	virtual void OnTakeDamageFromHero(const FVector& HitLocation, const class ATheLastBastionHeroCharacter* heroAttacker,
		float totalDamage, bool isCritical, bool isStun) {}

	virtual void EvaluateAttackerThreat(AActor* DamageCauser, float hp);

	// Define how does this AI response to the damage causer
	virtual void HitResponse(AActor* DamageCauser, float _currentHp) {  }

	// Called on Hp = 0;
	void OnDead(const FVector& dir, const AActor* _damageCauser, FName _boneName) override;

	void AddExp(class ATheLastBastionHeroCharacter* _heroAttacker);

	/** Get Melee Attack selection by distance, called during GetMeleeSel*/
	virtual int GetMeleeComboSel(bool _IsMoving) const { return 0; }

	/** Update the endurance for */
	virtual void UpdateEnduranceOnBeingHit(const AActor* const _damageCauser);

	///** AI shared behavior*/
	//UFUNCTION(BlueprintCallable)
	//	bool MountainOnDodgeParry(FName _boneName, const FVector& _damageCauserRelative, const class UPawnStatsComponent* const _damageCauserPawnStats);

	
};
