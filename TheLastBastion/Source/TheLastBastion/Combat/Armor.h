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

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	    class USkeletalMesh* BodyMeshRef;

public:
	
	void Equip(class USkeletalMeshComponent* const _skeletonMeshComponent) override;

};
