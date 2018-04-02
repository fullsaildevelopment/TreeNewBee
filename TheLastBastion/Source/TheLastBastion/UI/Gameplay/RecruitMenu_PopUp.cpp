// Fill out your copyright notice in the Description page of Project Settings.

#include "RecruitMenu_PopUp.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "AICharacters/TheLastBastionAIBase.h"
#include "Combat/PawnStatsComponent.h"
#include "CustomType.h"

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
	ATheLastBastionAIBase* defaultAI = _unitClass.GetDefaultObject();
	if (defaultAI)
	{
		UnitName->SetText(defaultAI->GetAIName());
		Description->SetText(defaultAI->GetAIDescription());

		int Level = Cast<ATheLastBastionCharacter>(GetOwningPlayerPawn())->GetCharacerLevel();
		float damage = 0, hp = 0;
		defaultAI->GetPawnStatsComp()->CalculateRawStatsByType(Level, defaultAI->GetCharacterType(), damage, hp);

		AddPopUpEntry(TEXT("Base Health: "), hp, UnitStats);
		AddPopUpEntry(TEXT("Base Damage: "), damage, UnitStats);

		TArray<int> Cost = defaultAI->GetResourceCost();

		// Food 
		int FoodCost = Cost[0];
		if (FoodCost > 0)
			AddPopUpEntryWithColor(FoodUI_Color, TEXT("Food Require:"), FoodCost, CostList);

		// Metal 
		int MetalCost = Cost[1];
		if (MetalCost > 0)
			AddPopUpEntryWithColor(MetalUI_Color, TEXT("Metal Require:"), MetalCost, CostList);

		// Wood 
		int WoodCost = Cost[2];
		if (WoodCost > 0)
			AddPopUpEntryWithColor(WoodUI_Color, TEXT("Wood Require:"), WoodCost, CostList);

	}
}
