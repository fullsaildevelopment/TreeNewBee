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

	bool bIsSprinting;


protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float SprintSpeed = 850.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float JogSpeed = 595.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float walkSpeed = 255.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float minTurnRate = 180.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
		float maxTurnRate = 540.0f;



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

#pragma endregion







protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

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
};

