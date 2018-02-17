// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/Weapon.h"
#include "Shield.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class THELASTBASTION_API AShield : public AWeapon
{
	GENERATED_BODY()

public:
	AShield();

protected:
	virtual void BeginPlay() override;

protected:


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ShieldShape)
		/** Actual mesh area of the shield*/
		FVector2D MeshArea;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ShieldShape)
		/** Actual collision area of the shield*/
		FVector2D CollisionArea;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ShieldShape)
		class UBoxComponent* ShieldBox;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ShieldShape)
		float Thickness;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ShieldShape)
		float HeightMeshColRatio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ShieldShape)
		float WidthMeshColRatio;


public:

	void Equip(class USkeletalMeshComponent* const _skeletonMeshComponent);
	void Arm(class USkeletalMeshComponent* const _skeletonMeshComponent);
	FVector GetArrowAttachLocation(const FVector& _hitLocation) const;
};
