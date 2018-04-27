// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/Gear.h"
#include "RangeWeapon.generated.h"

/**
 * 
 */
#define PowerFire_Range 1000.0f

#define Bullet_Spread_Range 80.0f

UCLASS()
class THELASTBASTION_API ARangeWeapon : public AGear
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ARangeWeapon();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "RangeWeapon")
	TSubclassOf<class AProjectile> ProjectileClassBP;

	UPROPERTY(EditDefaultsOnly, Category = "RangeWeapon")
	float ShootingRange;

	UPROPERTY(EditDefaultsOnly, Category = "RangeWeapon")
	//* horizontal velocity //
	float BulletSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "RangeWeapon")
	//* PowerShoot bullet amount //
	int BulletSpreadAmount;

	UPROPERTY(EditAnywhere, Category = "RangeWeapon")
		//* the less, the accurate for combo shot //
		float ComboShootVibrate;

	UPROPERTY(EditDefaultsOnly, Category = "RangeWeapon")
	float DivByBulletSpeed;

	// Bullet Spread in Degrees
	UPROPERTY(EditDefaultsOnly, Category = "AIRangeWeapon")
	float MinVerticalAimOffset;

	UPROPERTY(EditDefaultsOnly, Category = "AIRangeWeapon")
	float MaxVerticalAimOffset;

	UPROPERTY(EditDefaultsOnly, Category = "AIRangeWeapon")
	float MinHorizontalAimOffset;

	UPROPERTY(EditDefaultsOnly, Category = "AIRangeWeapon")
	float MaxHorizontalAimOffset;

public:

	class USceneComponent* GetMesh() const override;

	// 
	UFUNCTION(BlueprintCallable)
	virtual void Fire();

	void PowerFire();

	void ComboFire();

	UFUNCTION(BlueprintCallable)
	virtual void NPCFire(const AActor* _target);

protected:
	virtual void BeginPlay() override;

private:
	void PlayCrossbowFireSFXAtLocation(const UObject* WorldContextObject, FVector Location);
};
