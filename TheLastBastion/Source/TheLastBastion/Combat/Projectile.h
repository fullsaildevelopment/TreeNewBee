// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/Gear.h"
#include "Projectile.generated.h"

/**
 * 
 */
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DamageCollsion)
		class USceneComponent* DamageSphereHolder;

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

public:
	// Sets default values for this actor's properties
	AProjectile();

	void Tick(float _deltaTime) override;

	void SetDamageIsEnabled(bool _val);

	void BeginPlay() override;

	bool ShouldDestroy() const;


public:
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovementComp() const { return ProjectileMovementComp; }


	
};
