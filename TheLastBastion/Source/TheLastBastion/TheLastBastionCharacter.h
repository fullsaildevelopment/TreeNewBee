// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TheLastBastionCharacter.generated.h"



UCLASS(config=Game)
class ATheLastBastionCharacter : public ACharacter
{
	GENERATED_BODY()



protected:
	virtual void BeginPlay();

public:
	ATheLastBastionCharacter();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;


private:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** Animation Bp Reference */
	UPROPERTY()
		class UHero_AnimInstance*  mAnimInstanceRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* RightHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* LeftHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DamageDetection, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* Head;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DamageDetection, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* Body;


#pragma region Combat Collision Capsole Vals Presets

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CombatCollision, meta = (AllowPrivateAccess = "true"))
		float CapHalfSize_Travel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CombatCollision, meta = (AllowPrivateAccess = "true"))
		float CapRadius_Travel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CombatCollision, meta = (AllowPrivateAccess = "true"))
		float CapHalfSize_ShieldSword;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CombatCollision, meta = (AllowPrivateAccess = "true"))
		float CapRadius_ShieldSword;

#pragma endregion


#pragma region Hero Movement Stats
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float SprintSpeed = 850.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float JogSpeed = 595.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float walkSpeed = 255.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float minTurnRate = 180.0f;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float maxTurnRate = 630.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float maxTurnRate_Travel = 630.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float maxTurnRate_Combat = 1440.0f;

	/** Is Character currently sprinting*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bIsSprinting;

	/** Is Sprinting button is still pressed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bTryToSprint;

#pragma endregion


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





#pragma endregion




protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface


private:

	void InitCombatComponentsCollision();

	void StartSprint();
	void StopSprint();

	void SetCapsuleSizeToFitTravel();
	void SetCapsuleSizeToFitSwordShield();


	UFUNCTION()
	void OnBeingHit(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor
		, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, 
		bool _bFromSweep, const FHitResult& _SweepResult);



public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE float GetMaxTurnRate() const { return  maxTurnRate; }
	FORCEINLINE float GetMinTurnRate() const { return  minTurnRate; }

	FORCEINLINE float GetJogSpeed() const { return JogSpeed; }
	FORCEINLINE float GetSprintSpeed() const { return SprintSpeed; }

	FORCEINLINE bool IsSprinting() const { return bIsSprinting; }
	FORCEINLINE bool IsTryingSprinting() const { return bTryToSprint; }

	public:

		void EnableDamage(bool _bIsRightHand = true, bool _bIsAll = false);
		void DisableDamage(bool _bIsRightHand = true, bool _bIsAll = false);

};

