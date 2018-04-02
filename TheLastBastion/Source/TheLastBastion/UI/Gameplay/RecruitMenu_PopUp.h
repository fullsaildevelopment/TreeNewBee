// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Gameplay/PopUpBase.h"
#include "RecruitMenu_PopUp.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API URecruitMenu_PopUp : public UPopUpBase
{
	GENERATED_BODY()
	
protected:

	bool Initialize() override;

protected:

	UPROPERTY(meta = (BindWidget))
		/** vertical box that contain all the
		character stats data needs to be shown */
		class UVerticalBox* UnitStats;

	UPROPERTY(meta = (BindWidget))
		/** vertical box that contain all the
		character cost data needs to be shown */
		class UVerticalBox* CostList;


	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UnitName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Description;

public:

	/** On when pop up widget creation is needed, 
	put the default class information on viewport*/
	void OnPopUp(TSubclassOf<class ATheLastBastionAIBase> _unitClass);

};
