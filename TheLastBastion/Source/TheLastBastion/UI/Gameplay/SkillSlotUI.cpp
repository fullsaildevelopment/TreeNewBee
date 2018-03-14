// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillSlotUI.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

bool USkillSlotUI::Initialize()
{
	if (Super::Initialize() == false)
	{
		return false;
	}

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		CoolDownBar != nullptr && Key != nullptr;

	if (bAllWidgetAreGood == false)
		return false;


	CoolDownBar->SetPercent(0.0f);
	bIsCoolingDown = false;

	return true;

}

void USkillSlotUI::NativeTick(const FGeometry & MyGeometry, float InDeltaTime)
{
	if (bIsCoolingDown)
	{
		TimePassed += InDeltaTime;
		float barPercentage = 1 - TimePassed * DivByCoolDownTime;

		if (barPercentage <= 0)
		{
			OnCoolDownFinish();
		}
		else
		{
			CoolDownBar->SetPercent(barPercentage);
		}
	}
}

void USkillSlotUI::SetKeyText(FText _keyText)
{
	Key->SetText(_keyText);
}

void USkillSlotUI::OnCoolDownStart(float _cdDuration)
{
	bIsCoolingDown = true;
	CoolDownBar->SetPercent(1.0f);
	DivByCoolDownTime = 1.0f / _cdDuration;
	TimePassed = 0;
}

void USkillSlotUI::OnCoolDownFinish()
{
	bIsCoolingDown = false;
	CoolDownBar->SetPercent(0);
}
