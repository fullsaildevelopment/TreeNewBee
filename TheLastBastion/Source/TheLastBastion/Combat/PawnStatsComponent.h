// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PawnStatsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THELASTBASTION_API UPawnStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPawnStatsComponent();

protected:

	class ATheLastBastionCharacter* mCharacter;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Combat)
		TSubclassOf <class AWeapon> LeftHandWeapon_ClassBp;
	class AWeapon*    LeftHandWeapon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Combat)
		TSubclassOf <class AWeapon> RightHandWeapon_ClassBp;
	class AWeapon*    RightHandWeapon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Combat)
		TSubclassOf <class AArmor> Armor_ClassBp;
	class AArmor *    Armor;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Combat)
	   class USphereComponent*        Head;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Combat)
	   class UBoxComponent*           Body;



	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	


	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;




	void EnableWeapon(bool _bIsRightHand = true, bool _bIsAll = false);

	void DisableWeapon(bool _bIsRightHand = true, bool _bIsAll = false);

	/** Attach the current weapon to the weapon slot*/
	void OnEquipWeapon();

	void OnSheathWeapon();

protected:
	// Called When Body Is overlapped by weapon
	UFUNCTION()
		virtual void OnBodyHit(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor
			, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex,
			bool _bFromSweep, const FHitResult& _SweepResult);

	// Called When Head Is overlapped by weapon
	UFUNCTION()
		virtual void OnHeadHit(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor
			, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex,
			bool _bFromSweep, const FHitResult& _SweepResult);

	// Called When Body Is overlapped by weapon
	UFUNCTION()
		virtual void OnRightHandWeaponHit(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor
			, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex,
			bool _bFromSweep, const FHitResult& _SweepResult);


	// Called When Body Is overlapped by weapon
	UFUNCTION()
		virtual void OnLeftHandWeaponHit(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor
			, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex,
			bool _bFromSweep, const FHitResult& _SweepResult);


};
