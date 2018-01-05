// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGamePlayerRow.generated.h"

/**
*
*/
UCLASS()
class THELASTBASTION_API UInGamePlayerRow : public UUserWidget
{
	GENERATED_BODY()

protected:

	bool Initialize() override;

protected:

	UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
		class UTexture2D* HeroAvatarImage;	

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* Name;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* Level;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Class;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Hp;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HpBar;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Sp;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* SpBar;


public:
	
	void SetPlayerName(const struct FPlayerProfile& _profile);

	void SetHpValue(float _currentHp, float _maxHp);

	void SetHpPercentage(float _currentHp, float _divByMax);

	void SetSpValue(float _currentSp, float _maxSp);

	void SetSpPercentage(float _currentSp, float _divByMax);

	void SetLevel(int _Level);
}; 
