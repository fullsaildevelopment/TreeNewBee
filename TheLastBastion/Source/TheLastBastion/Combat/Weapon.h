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

	virtual void BeginPlay() override;

private:

		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* Mesh;


public:
	FORCEINLINE class UStaticMeshComponent* GetWeaponMeshRef() const { return Mesh; }

	void Arm(USkeletalMeshComponent * const _skeletonMeshComponent);
	
};
