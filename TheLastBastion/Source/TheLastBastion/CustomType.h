// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/ConstructorHelpers.h"
#include "CustomType.generated.h"





#define SURFACE_FLESH EPhysicalSurface::SurfaceType1
#define SURFACE_METAL EPhysicalSurface::SurfaceType2
#define SURFACE_LightShield EPhysicalSurface::SurfaceType3
#define SURFACE_HeavyShield EPhysicalSurface::SurfaceType4
#define SURFACE_ICE EPhysicalSurface::SurfaceType5

#define CollisionObjectType_Hero ECollisionChannel::ECC_GameTraceChannel8


USTRUCT(Blueprintable)
struct FPlayerProfile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText mPlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText mPlayerStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTexture2D* mAvatarImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTexture2D* mCharacterImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class ACharacter> mCharacterClass;

	UPROPERTY()
		bool bIsRangerClass = true;
};
/**
 * 
 */
UCLASS()
class THELASTBASTION_API UCustomType : public UObject
{
	GENERATED_BODY()
	
public:

	template <typename Type>
	static bool FindClass(TSubclassOf<Type>& _class, const TCHAR* _classToFind);
	
};

template<typename Type>
inline bool UCustomType::FindClass(TSubclassOf<Type>& _class, const TCHAR * _classToFind)
{
	ConstructorHelpers::FClassFinder<Type> classFinder(_classToFind);
	if (classFinder.Class)
		_class = classFinder.Class;

	return classFinder.Succeeded();
}

