// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheLastBastionCharacter.h"
#include "TheLastBastionHeroCharacter.generated.h"

/**
 * 
 */

UCLASS()
class THELASTBASTION_API ATheLastBastionHeroCharacter : public ATheLastBastionCharacter
{
	GENERATED_BODY()

public:

	ATheLastBastionHeroCharacter();

protected:

	virtual void BeginPlay();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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

	void OnCommandMarch();

	void OnCommandHold();

	// Set group to be scatter or compact
	void OnCommandDistribute();

	// Set group to be square or row
	void OnCommandReform();

	void OnCommandForward();

	void OnCommandBackward();

	void OnCommandFollowing();

	void OnCommandPressed();

	void OnCommandReleased();
#pragma endregion

	UFUNCTION()
		void OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType
			, class AController* InstigatedBy, AActor* DamageCauser) override;

	UFUNCTION()
		void OnTakePointDamageHandle(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
			class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
			const class UDamageType* DamageType, AActor* DamageCauser) override;


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		class UHeroStatsComponent* HeroStats;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Targeting)
		class USphereComponent*    TargetDetector;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (BlueprintProtected))
		float Unfocus_CamRotationLagging = 30.0f;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (BlueprintProtected))
		float Focus_CamRotationLagging = 15.0f;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimationMovementControl)
		float MoveForwardAxis;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimationMovementControl)
		float MoveRightAxis;


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

	UPROPERTY()
		/** swat under player command */
		class AAllyGroup* CommandedGroup;


private:

	/** Animation Bp Reference */
	UPROPERTY()
		class UHero_AnimInstance*  mAnimInstanceRef;
	UPROPERTY()
		class UInGameHUD*          mInGameHUD;

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



	bool bIsInCommandMode;

public:

	// Override A Parent class function
	FVector GetPawnViewLocation() const override;	
	void ToggleFireMode(bool _val);

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
	
	UFUNCTION(BlueprintPure)
		FORCEINLINE float GetMoveForwardAxis() const { return MoveForwardAxis; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE float GetMoveRightAxis() const { return MoveRightAxis; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsInCommandMode() const { return bIsInCommandMode; }

	class AGear* GetCurrentWeapon() const;


};
