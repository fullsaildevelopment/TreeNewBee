// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/Gear.h"
#include "Armor.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API AArmor : public AGear
{
	GENERATED_BODY()

public:
	AArmor();
	
private:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Mesh;	

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	    class USkeletalMesh* BodyMeshRef;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		class UMaterialInterface* BodyMaterialRef;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		class UMaterialInterface* BodyLogoMaterialRef;


public:
	
	void Equip(class USkeletalMeshComponent* const _skeletonMeshComponent) override;

};
