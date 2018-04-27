// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Gameplay/PopUpBase.h"
#include "SkillMenu_PopUp.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API USkillMenu_PopUp : public UPopUpBase
{
	GENERATED_BODY()

protected:

	bool Initialize() override;
	


	UPROPERTY(meta = (BindWidget))
		/** vertical box that contain all the
		character stats data needs to be shown */
		class UVerticalBox* BonusList;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* SkillName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Description;


public:

	void SetUpSkillPopUp(int _index, const class UInGameMenu* const _inGameMenu);

	

private:

	void AddEntryFor_SurvivalTraining(const class UInGameMenu * const _inGameMenu);
	void AddEntryFor_StaminaTrainging(const class UInGameMenu * const _inGameMenu);
	void AddEntryFor_FarmerLevel(const class UInGameMenu * const _inGameMenu);
	void AddEntryFor_Builder(const class UInGameMenu * const _inGameMenu);
	void AddEntryFor_Miner(const class UInGameMenu * const _inGameMenu);
	void AddEntryFor_Sawyer(const class UInGameMenu * const _inGameMenu);

	void AddEntryFor_HitThemHard(const class UInGameMenu * const _inGameMenu);
	void AddEntryFor_MakeThemSuffer(const class UInGameMenu * const _inGameMenu);
	void AddEntryFor_Faith(const class UInGameMenu * const _inGameMenu);
	void AddEntryFor_Leader(const class UInGameMenu * const _inGameMenu);


	
};
