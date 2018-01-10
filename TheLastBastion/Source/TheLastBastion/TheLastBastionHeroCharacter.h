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


	/** Preserve the Yaw input from Pawn interface, and capatable with camera Lock - on*/
	void AddControllerYaw(float _yaw);





#pragma endregion


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		class UHeroStatsComponent* HeroStats;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Targeting)
		class USphereComponent*    TargetDetector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		float NonLockOn_CamRotationLagging;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		float LockOn_CamRotationLagging;

private:

	/** Animation Bp Reference */
	UPROPERTY()
		class UHero_AnimInstance*  mAnimInstanceRef;

	float MoveForwardAxis;
	float MoveRightAxis;

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

	private:

		UFUNCTION()
			void OnHealthChangedHandle(const UPawnStatsComponent * _pawnStatsComp, float _damage, const UDamageType * _damageType, FName _boneNmame, FVector _shotFromDirection);

public:
	
	FORCEINLINE class USphereComponent* GetTargetDetector() const { return TargetDetector; }


	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UHeroStatsComponent* GetHeroStatsComp() const { return HeroStats; }	
	FORCEINLINE class UHero_AnimInstance* GetAnimInstanceRef() const { return mAnimInstanceRef; }

	FORCEINLINE float GetLockOnCameraRotationLag() const { return LockOn_CamRotationLagging; }
	FORCEINLINE float GetNonLockOnCameraRotationLag() const { return NonLockOn_CamRotationLagging; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE float GetMoveForwardAxis() const { return MoveForwardAxis; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE float GetMoveRightAxis() const { return MoveRightAxis; }


};
