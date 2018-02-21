// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrewBar_RecruitMenu.generated.h"


#define MaxAlliesNum 50

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
	
};
