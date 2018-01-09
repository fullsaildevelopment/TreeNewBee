// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/Gear.h"
#include "RangeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API ARangeWeapon : public AGear
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "RangeWeapon")
	TSubclassOf<class AProjectile> ProjectileClassBP;

	UPROPERTY(EditDefaultsOnly, Category = "RangeWeapon")
	float ShootingRange;

	UPROPERTY(EditDefaultsOnly, Category = "RangeWeapon")
	float BulletSpeed;

public:
	// Sets default values for this actor's properties
	ARangeWeapon();

	// 
	UFUNCTION(BlueprintCallable)
	virtual void Fire();

protected:
	virtual void BeginPlay() override;
};
