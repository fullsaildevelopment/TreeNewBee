// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/ConstructorHelpers.h"
#include "CustomType.generated.h"

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

