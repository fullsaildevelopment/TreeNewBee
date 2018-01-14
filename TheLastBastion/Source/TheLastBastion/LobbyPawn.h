// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LobbyPawn.generated.h"

UCLASS(BlueprintType)
class THELASTBASTION_API ALobbyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALobbyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:



	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* InspectorCamera;

protected:

	/** Follow camera */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InspectorCamera)
		FVector RangerAvatarPosition;

	/** Follow camera */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InspectorCamera)
		FVector BuilderAvatarPosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InspectorCamera)
		/** Inspector camera switch speed between two hero avatar*/
		float InspectorCameraSwitchRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = InspectorCamera)
		bool bIsCheckingRanger;

public:

	FORCEINLINE void SetIsCheckingRanger(bool _val) { bIsCheckingRanger = _val; }

};
