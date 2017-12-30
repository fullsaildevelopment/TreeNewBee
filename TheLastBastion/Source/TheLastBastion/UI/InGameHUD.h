// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameHUD.generated.h"

/**
*
*/
UCLASS()
class THELASTBASTION_API UInGameHUD : public UUserWidget
{
	GENERATED_BODY()


protected:

	virtual bool Initialize();


	UPROPERTY(meta = (BindWidget))
		class UInGamePlayerRow* PlayerRow;

	UPROPERTY(meta = (BindWidget))
		class UVerticalBox* TeamWindow;



	UPROPERTY(meta = (BindWidget))
		class UTextBlock* GoldValue;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WoodValue;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MetalValue;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* RockValue;

private:

	UPROPERTY()
		class UGI_TheLastBastion* mGameInstanceRef;

	UPROPERTY()
		class ATheLastBastionHeroCharacter* mHeroCharacter;


public:


};
