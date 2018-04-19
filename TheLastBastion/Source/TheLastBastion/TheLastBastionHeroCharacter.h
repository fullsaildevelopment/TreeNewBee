// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheLastBastionCharacter.h"
#include "TheLastBastionHeroCharacter.generated.h"

/**
 * 
 */

#define  COMMAND_RANGE 50000 // the maximum distance hero can send his troops to
#define  HOLD_POSITION_BWD_OFFSET 100 

#define  Skill__Combo 0
#define  Skill__PowerHit 1    
#define  Skill__Taunt 2 
#define  Skill__WeaponCastingFire 3
#define  Skill__Heal 4   
#define  Skill__BattleCommand 5        

#define CommandPresence_LevelUpDelta 0.05f

DECLARE_DELEGATE(FOnSkillUsed)

USTRUCT(BlueprintType)
struct FSkillSlot
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		// true, if the skill has already passed the cool down time
		bool bIsUnlocked = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		// the time delay between using this skill
		float CoolDownTime = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		// the Sp Cost this skill
		float SpCost = -25.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FName AM_sectionName;

	//FOnSkillUsed OnSkillUsedDelegate;

	///** Handle Cool Down*/
	//FTimerHandle SkillTimer;
};


UCLASS()
class THELASTBASTION_API ATheLastBastionHeroCharacter : public ATheLastBastionCharacter
{
	GENERATED_BODY()

public:

	ATheLastBastionHeroCharacter();

protected:

	virtual void BeginPlay();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Tick(float _deltaTime) override;


protected:

#pragma region KeyBindings Event
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	/** Called when Sprint button is Pressed*/
	void OnSprintPressed();

	/** Called when Sprint button is Released*/
	void OnSprintReleased();

	/** Called when Sprint button is Pressed*/
	void OnJumpPressed();

	/** Called when Sprint button is Released*/
	void OnJumpReleased();

	/** Called when Attack button is Pressed*/
	void OnAttackPressed();

	/** Called when Attack button is Pressed*/
	void OnEquipPressed();

	/** Called when Focus Button is Pressed*/
	void OnMiddleMouseButtonPressed();

	/** Called when Dodge Button is Pressed*/
	void OnCorLAltPressed();

	/** Called when RMB is pressed*/
	void OnRightMouseButtonPressed();

	/** Called when RMB is released*/
	void OnRightMouseButtonReleased();

	/** Called when player try to Switch between Melee and Range weapon*/
	void OnTABPressed();

	/** Preserve the Yaw input from Pawn interface, and capatable with camera Lock - on*/
	void AddControllerYaw(float _yaw);

	/** Called When player try to use range weapon*/
	void OnUseRangeWeapon();

	/** Called When player try to Single hand weapon (Extra shield)*/
	void OnUseSHWeapon();

	/** Called When player try to Two Hand weapon (High speed, critical)*/
	void OnUseTHWeapon();

	/** Called When player try to Heavy weapon (High Damage, Bleed or Stun, Slow)*/
	void OnUseHeavyWeapon();

	/** Called When player pressed ESC, toggle the In game menu*/
	void OnPause();

	/** Player can open trade menu when close to Black Smith */
	void OpenTradeMenu();

	/** Player can open recruit menu when close to Barracks */
	void OpenRecruitMenu();

	void OnCommandMarch();
	void OnCommandHold();
	// Set group to be scatter or compact
	void OnCommandDistribute();
	// Set group to be square or row
	void OnCommandReform();
	void OnCommandForward();
	void OnCommandBackward();
	void OnCommandFwdBwd(bool _Fwd);
	void OnCommandFollowing();
	void OnCommandPressed();
	void OnCommandReleased();

	void OnSelectedCrew_1();
	void OnSelectedCrew_2();
	void OnSelectedCrew_3();
	void OnSelectedCrew_4();
	void OnSelectedCrewOnIndex(int _index);

	void OnSkillPressed_0();
	void OnSkillPressed_1();
	void OnSkillPressed_2();
	void OnSkillPressed_3();
	void OnSkillPressed_4();
	void OnSkillPressed_5();

#pragma endregion

	UFUNCTION()
		void OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType
			, class AController* InstigatedBy, AActor* DamageCauser) override;

	UFUNCTION()
		void OnTakePointDamageHandle(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
			class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
			const class UDamageType* DamageType, AActor* DamageCauser) override;


	virtual void KnockOut(const FVector& dir, const AActor* _damageCauser, FName _boneName) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		class UHeroStatsComponent* HeroStats;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Targeting)
		class USphereComponent*    TargetDetector;


	UPROPERTY()
		/** swat under player command */
		class AAllyGroup* CommandedGroup;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (BlueprintProtected))
		float Unfocus_CamRotationLagging = 30.0f;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (BlueprintProtected))
		float Focus_CamRotationLagging = 15.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimationMovementControl)
		float MoveForwardAxis;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimationMovementControl)
		float MoveRightAxis;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		/** The index of skill that play try to use in the skill list*/
		int TryToUseSkill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		/** Hero Skill sets*/
		TArray<FSkillSlot> SkillSlots;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Control, meta = (BlueprintProtected))
		/** Disable updating the movement axis and use the value from previous frame*/
		bool bUsePreviousMovementAxis = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Control, meta = (BlueprintProtected))
		/** Disable the update the control yaw input*/
		bool bIsMovementDisabled = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Control, meta = (BlueprintProtected))
		/** Disable the update the control yaw input*/
		bool bIsYawControllDisabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GroupControl)
		bool bControlEnemyGroup;


private:

#pragma region  Camera

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float BaseLookUpRate;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

#pragma endregion

	/** Animation Bp Reference */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UHero_AnimInstance*  mAnimInstanceRef;

	UPROPERTY()
		//temp particle system component for weapon enchantment effect
		class UParticleSystemComponent* WeaponEnchantment_PSC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = vfx)
		float ParticleEffectCylinderHeight;

	UPROPERTY()
		class UInGameHUD*          mInGameHUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Castle)
		/** Castle damage reduction in percentage while hero is in castle */
		float CommanderPresence;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = CharacterStats)
		/** How many experience our hero has for current level */
		float CurrentExp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = CharacterStats)
		/** How many experience our hero need for level up */
		float MaxExp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = CharacterStats)
		/** Divider for How many experience our hero need for level up */
		float MaxExpDiv;

	/** Timer to handle hp recover delay after being hit*/
	FTimerHandle HpRecoverTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = CharacterStats)
		/** Is Hp recovering */
		bool bHpRecovering;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = CharacterStats)
		bool bHasEnchartedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = CharacterStats)
		bool bIsInCommandMode;

	// Can Player Open Recruit Menu or Trade Menu
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = CharacterStats)
		bool bCanOpenTradeMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = CharacterStats)
		bool bCanOpenRecruitMenu;

private:

	void UpdateHeroStats(float _deltaTime);
	void OnWeaponEnchantStart();

public:

	// Override A Parent class function
	FVector GetPawnViewLocation() const override;	
	void OnGetUp() override;
	void RagDollRecoverOnFinish() override;
	void ToggleFireMode(bool _val);

	/** true, if current sp is enought to perform melee attack, and consume sp*/
	bool MeleeAttackSpCheck();
	/** true, if current sp is enought to perform dodge, and consume sp*/
	bool DodgeSpCheck();
	/** true, if current sp is enought to perform counter attack, and consume sp*/
	bool CounterAttackSpCheck();
	/** true, 
	* if current sp is enought to perform skill, and consume sp
	* if cool down time is passed*/
	bool SkillCheck(int _skillIndex);

	/** Check if this character is able to counter attack this attack*/
	virtual bool OnCounterAttack(const struct FDamageInfo* const _damageInfo,
		const class UPawnStatsComponent* const _damageCauserPawnStats);

	bool IsDoingCounterAttack() const;

	FORCEINLINE class USphereComponent* GetTargetDetector() const { return TargetDetector; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UHeroStatsComponent* GetHeroStatsComp() const { return HeroStats; }	
	FORCEINLINE class UHero_AnimInstance* GetAnimInstanceRef() const { return mAnimInstanceRef; }

	FORCEINLINE float GetLockOnCameraRotationLag() const { return Focus_CamRotationLagging; }
	FORCEINLINE float GetNonLockOnCameraRotationLag() const { return Unfocus_CamRotationLagging; }
	FORCEINLINE class UInGameHUD* GetInGameHUD() const { return mInGameHUD; }

	bool IsSkillCooled(int _index) const;
	FORCEINLINE float GetSkillCoolDownTimeAt(int _index) const { return SkillSlots[_index].CoolDownTime; }
	FORCEINLINE float GetSkillSpCostAt(int _index) const { return SkillSlots[_index].SpCost; }

	bool IsIntentedSkillCooled() const;
	FORCEINLINE float GetIntentedSkillCoolDownTimeAt() const { return SkillSlots[TryToUseSkill].CoolDownTime; }
	FORCEINLINE float GetIntentedSkillSpCostAt() const { return SkillSlots[TryToUseSkill].SpCost; }
	FORCEINLINE FName GetSkillSectionNameAt(int _skillIndex) const { return SkillSlots[_skillIndex].AM_sectionName; }

	FORCEINLINE void SetSkillSectionNameAt(int _skillIndex, FName _sectionName) { SkillSlots[_skillIndex].AM_sectionName = _sectionName; }
	FORCEINLINE void EnableHpRecovering() { bHpRecovering = true; }
	FORCEINLINE void DisnableHpRecovering() { bHpRecovering = false; }
	FORCEINLINE float GetCommandPresence() const { return CommanderPresence; }

	FORCEINLINE void SetCommandPresence(float _val) { CommanderPresence = _val; }
	FORCEINLINE void OnCommandPresenceLevelUp() { CommanderPresence += CommandPresence_LevelUpDelta; }

	// Encharting interface
	FORCEINLINE bool HasEnchartedWeapon() const { return bHasEnchartedWeapon; }

	// Turn on/off player's ablity to Open Menu
	FORCEINLINE void SetTradeMenuState(bool _val) { bCanOpenTradeMenu = _val; }
	FORCEINLINE void SetRecruitMenuState(bool _val) { bCanOpenRecruitMenu = _val; }

	/** Called to stop weapon enchant*/
	void OnWeaponEnchantStop();

	UFUNCTION(BlueprintPure)
		FORCEINLINE float GetMoveForwardAxis() const { return MoveForwardAxis; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE float GetMoveRightAxis() const { return MoveRightAxis; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsInCommandMode() const { return bIsInCommandMode; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE class AAllyGroup* GetCommandGroup() const { return CommandedGroup; }

	void AddExp(float _val);

	/** Play partical based on skill index, called after stanima check*/
	void OnPlaySkillParticle(int _skillIndex);
};
