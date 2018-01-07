// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/Gear.h"
#include "Weapon.generated.h"




/**
 * 
 */
UCLASS()
class THELASTBASTION_API AWeapon : public AGear
{
	GENERATED_BODY()
	
public:

	AWeapon();

protected:

	void BeginPlay() override;

private:

		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* Mesh;

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = DamageCollision)
		bool bDisableCutOpenDamage;
	UPROPERTY(BlueprintReadOnly, Category = DamageCollsion)
		bool bDamageIsEnable;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = DamageCollsion)
		float DamageEdgeOffset_start;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = DamageCollsion)
		float DamageEdgeOffset_end;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = DamageCollsion)
		FVector DamageVolumnExtend;

	UPROPERTY(BlueprintReadOnly, Category = DamageCollsion)
		/** The actor to ignore in one slash*/
		TArray<AActor*> IgnoredActors;

	UPROPERTY(EditDefaultsOnly, Category = DamageCalculation)
		TSubclassOf <class UDamageType> DamageType;


public:

	FORCEINLINE class UStaticMeshComponent* GetWeaponMeshRef() const { return Mesh; }

	/** Called when character try to attack without equip animation, or during equip animation*/
	void Arm(USkeletalMeshComponent * const _skeletonMeshComponent);

	void SetDamageIsEnabled(bool _val);

	void Tick(float _deltaTime) override;	
};
