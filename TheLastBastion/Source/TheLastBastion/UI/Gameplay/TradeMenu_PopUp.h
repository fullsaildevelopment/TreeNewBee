// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Blueprint/UserWidget.h"
#include "UI/Gameplay/PopUpBase.h"
#include "TradeMenu_PopUp.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UTradeMenu_PopUp : public UPopUpBase
{
	GENERATED_BODY()
	
//public:
//
//	UTradeMenu_PopUp(const FObjectInitializer & ObjectInit);

protected:

	bool Initialize() override;

//private:
//	static TSubclassOf<UUserWidget> WBP_TradePopUpEntry;

protected:

	UPROPERTY(meta = (BindWidget))
		/** vertical box that contain all the 
		gear stats data needs to be shown */
		class UVerticalBox* GearStats;

	UPROPERTY(meta = (BindWidget))
		/** vertical box that contain all the
		gear cost data needs to be shown */
		class UVerticalBox* CostList;


	UPROPERTY(meta = (BindWidget))
		class UTextBlock* GearName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Description;


public:

	/** On when pop up widget creation is needed, put the default class information on viewport*/
	void OnPopUp(TSubclassOf<class AGear> _GearClass);


private:

	void PopUpArmor(const class AGear* const _armor);
	void PopUpSharpWeapon(const class AGear* const _sharpWeapon);
	void PopUpStunWeapon(const class AGear* const _stunWeapon);

	void PopUpCost(const class AGear* const _gear);

	//void AddPopUpEntry(FString _text, int _val);
	//void AddPopUpEntryWithColor(FLinearColor _color, FString _text, int _val);

};
