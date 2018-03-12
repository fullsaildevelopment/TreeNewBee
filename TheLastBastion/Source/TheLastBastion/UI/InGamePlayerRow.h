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
		class UProgressBar* DpBar;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Sp;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* SpBar;


public:
	
	/** Called during the create of HUD, init by player profile*/
	void InitByPlayerProfile(const struct FPlayerProfile& _profile);

	/** Set the Hp value to the Text*/
	void SetHpValue(float _currentHp, float _maxHp);
	/** Set the Hp percentage to the HpBar*/
	void SetHpPercentage(float _currentHp, float _divByMax);
	/** Set the Sp value to the Text*/
	void SetSpValue(float _currentSp, float _maxSp);
	/** Set the Sp percentage to the SpBar*/
	void SetSpPercentage(float _currentSp, float _divByMax);
	/** Set the Dp percentage to the DpBar*/
	void SetDpPercentage(float _currentDp, float _divByMax);
	/** Set the Level to the text*/
	void SetLevel(int _Level);
	/** Set the Hp value to the Text and percentage to Hp Bar*/
	void SetHp(float _currentHp, float _maxHp, float _divByMax);
	/** Set the Sp value to the Text and percentage to Sp Bar*/
	void SetSp(float _currentSp, float _maxSp, float _divByMax);
}; 
