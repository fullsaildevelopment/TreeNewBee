// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/Gear.h"
#include "Projectile.generated.h"

/**
 * 
 */

#define Bullets_DamageBoxExtend FVector(12, 1.5f, 1.5f)
#define Bullets_DamageBoxOffset FVector(-3, 0, 0)
#define Bullets_HeadOffset 10
#define Bullets_EndOffset -18

#define StabInDistance 3.0f;

UCLASS(BlueprintType)
class THELASTBASTION_API AProjectile : public AGear
{
	GENERATED_BODY()
	
protected:
	// Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* ProjectileMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComp;

	UPROPERTY(BlueprintReadOnly, Category = DamageCollsion)
		/** The actor to ignore in one shot*/
		TArray<AActor*> IgnoredActors;

	FCollisionObjectQueryParams ObjectParams;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = DamageCollsion)
		// how many enemy can this arrow penetrate
		int PenetrateLevel;

	UPROPERTY(BlueprintReadOnly, Category = DamageCollsion)
		// how many enemies this bullet hit
		int CurrentHitCount;

	UPROPERTY(BlueprintReadOnly, Category = DamageCollsion)
		bool bDamageIsEnable;

	UPROPERTY(BlueprintReadOnly, Category = DamageCollsion)
		// Does this projectile should fly or stick onto something
		bool bIsFlying;

public:
	// Sets default values for this actor's properties
	AProjectile();

	void Tick(float _deltaTime) override;

	void SetDamageIsEnabled(bool _val);

	void BeginPlay() override;

	bool ShouldDestroy() const;

	void SetInitFireVelocity(const FVector& _hor, float flyTime);

public:
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovementComp() const { return ProjectileMovementComp; }

	void ProjectileOnFire(class AGear* _rangeWeapon, bool _isComboProjectile = false);

private:

	void MakeStatic();

	void SetMesh(UStaticMesh * _mesh);

protected:

	UFUNCTION(BlueprintNativeEvent)
		class UStaticMesh* GetFlyingMesh() const;
	UStaticMesh* GetFlyingMesh_Implementation() const { return nullptr; }


	UFUNCTION(BlueprintNativeEvent)
		class UStaticMesh* GetNonFlyingMesh() const;
	UStaticMesh* GetNonFlyingMesh_Implementation() const { return nullptr; }

};
