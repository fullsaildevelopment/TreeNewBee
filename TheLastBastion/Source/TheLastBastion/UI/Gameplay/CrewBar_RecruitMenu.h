// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrewBar_RecruitMenu.generated.h"


#define MaxAlliesNum 50
#define MinUnitsNum_EachCrew 3 /** The Minimum number of unit number for decreasing the number of each unit group*/
#define StartingNum_EachCrew 5 /** The Group size we use when adding new unit */

UCLASS()
class THELASTBASTION_API UCrewBar_RecruitMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	bool Initialize() override;

protected:
	
	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* AllAllyGroups;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TotalNum;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WoodValue;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* FoodValue;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* StoneValue;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MetalValue;

	int Wood, Food, Stone, Metal;

private:

	UPROPERTY()
		TArray<class UCrewBlock*> AllCrewBlock;

	int totalAmount;

public:

	void OnOpenRecruitMenu();

	/** Called When Accept button is pressed, 
	adjust allies if the crewBlock is changed while Recruit menu is opened*/
	void OnAccept();

	void SetTotalAmount(int _val);

	FORCEINLINE int GetTotalAmount() const { return totalAmount; }
	
	/** Load the resource from Game Mode*/
	void LoadResource();
	void AddWoodValue(int _val);
	void AddFoodValue(int _val);
	void AddMetalValue(int _val);
	void AddStoneValue(int _val);




};
