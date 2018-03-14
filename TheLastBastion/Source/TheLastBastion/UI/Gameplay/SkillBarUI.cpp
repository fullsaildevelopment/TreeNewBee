// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillBarUI.h"
#include "Components/HorizontalBox.h"
#include "UI/Gameplay/SkillSlotUI.h"

bool USkillBarUI::Initialize()
{
	if (Super::Initialize() == false)
	{
		return false;
	}

	// Bind Delegetes to Widget components
	bool bAllWidgetAreGood =
		SkillRow != nullptr;

	if (bAllWidgetAreGood == false)
		return false;



	// Set Key Text

	USkillSlotUI* skillSlot =  nullptr;
	if (SkillRow->GetChildrenCount() == 7 && SkillThumbNails.Num() == 7)
	{
		skillSlot = Cast<USkillSlotUI>(SkillRow->GetChildAt(0));
		skillSlot->SetKeyText(FText::FromString(TEXT("q")));
		skillSlot->SetSkillTN(SkillThumbNails[0]);
		skillSlot = Cast<USkillSlotUI>(SkillRow->GetChildAt(1));
		skillSlot->SetKeyText(FText::FromString(TEXT("e")));
		skillSlot->SetSkillTN(SkillThumbNails[1]);
		skillSlot = Cast<USkillSlotUI>(SkillRow->GetChildAt(2));
		skillSlot->SetKeyText(FText::FromString(TEXT("r")));
		skillSlot->SetSkillTN(SkillThumbNails[2]);
		skillSlot = Cast<USkillSlotUI>(SkillRow->GetChildAt(3));
		skillSlot->SetKeyText(FText::FromString(TEXT("f")));
		skillSlot->SetSkillTN(SkillThumbNails[3]);
		skillSlot = Cast<USkillSlotUI>(SkillRow->GetChildAt(4));
		skillSlot->SetKeyText(FText::FromString(TEXT("1")));
		skillSlot->SetSkillTN(SkillThumbNails[4]);
		skillSlot = Cast<USkillSlotUI>(SkillRow->GetChildAt(5));
		skillSlot->SetKeyText(FText::FromString(TEXT("2")));
		skillSlot->SetSkillTN(SkillThumbNails[5]);
		skillSlot = Cast<USkillSlotUI>(SkillRow->GetChildAt(6));
		skillSlot->SetKeyText(FText::FromString(TEXT("3")));
		skillSlot->SetSkillTN(SkillThumbNails[6]);
	}


	return true;
}

void USkillBarUI::OnLaunchSkillAt(int _skillIndex, float _cdDuration)
{
	USkillSlotUI* skillSlot = Cast<USkillSlotUI>(SkillRow->GetChildAt(_skillIndex));
	if (skillSlot)
	{
		skillSlot->OnCoolDownStart(_cdDuration);
	}
}

bool USkillBarUI::IsCooledDownAt(int _skillIndex) const
{
	USkillSlotUI* skillSlot = Cast<USkillSlotUI>(SkillRow->GetChildAt(_skillIndex));
	if (skillSlot)
	{
		return skillSlot->IsCooledDown();
	}
	else
	{
		return false;
	}

}
