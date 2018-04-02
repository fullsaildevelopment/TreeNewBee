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


#define MetalUI_Color FLinearColor(0.448561f,0.306668f,0.494792f)
#define WoodUI_Color  FLinearColor(1.000000f,0.542730f,0.235314f)
#define StoneUI_Color FLinearColor(0.213542f,0.213542f,0.213542f)
#define FoodUI_Color  FLinearColor(1.0f,0.518488f,0.488628f)

// Floating text color
#define EnemyHit_FTColor       FLinearColor(0.604167f,0.027984f,0.0f)
#define CriticalHit_FTColor	   FLinearColor(0.309647f,0.0f,1.0f)
#define FireHit_FTColor		   FLinearColor(1.0f,0.159238f,0.0f)
#define IceHit_FTColor		   FLinearColor(0.251666f,0.798392f,1.0f)
#define StunHit_FTColor		   FLinearColor(0.709889f,1.0f,0.0f)

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

