// Fill out your copyright notice in the Description page of Project Settings.

#include "RecruitMenu_PopUp.h"


bool URecruitMenu_PopUp::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	bool bAllWidgetAreGood = UnitStats && CostList &&
		Description && UnitName;

	if (bAllWidgetAreGood == false)
		return false;

	return true;
}

void URecruitMenu_PopUp::OnPopUp(TSubclassOf<class ATheLastBastionAIBase> _unitClass)
{
	
}
