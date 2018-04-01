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

		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* ParticleSystemComp;

		float ParticleEffectCylinderHeight;

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = DamageCollision)
		bool bEnableCutOpenDamage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = DamageCollision)
		bool bShowBounding;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DamageCollision)
		bool bDamageIsEnable;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = DamageCollision)
		FVector DamageEdgeOffset_start;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = DamageCollision)
		FVector DamageEdgeOffset_end;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = DamageCollision)
		FVector DamageVolumnExtend;

	UPROPERTY(BlueprintReadOnly, Category = DamageCollision)
		/** The actor to ignore in one slash*/
		TArray<AActor*> IgnoredActors;


public:

	void SetDamageIsEnabled(bool _val);

	virtual void Tick(float _deltaTime) override;	

	FORCEINLINE class USceneComponent* GetMesh() const override {return Mesh;}

	FORCEINLINE class UParticleSystemComponent* GetParticleSystemComp() const override { return ParticleSystemComp; }

	void GetRayCastPosition(FVector& _start, FVector& _end);

	////
	//UFUNCTION(BlueprintCallable)
	//void StartWeaponFireEnchantment() override;

	class UParticleSystemComponent* OnWeaponEnchant() override;

	//UFUNCTION(BlueprintCallable)
	//void EndWeaponFireEnchantment() override;
};
