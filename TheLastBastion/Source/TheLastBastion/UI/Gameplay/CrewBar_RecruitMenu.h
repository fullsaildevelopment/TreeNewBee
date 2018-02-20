// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrewBar_RecruitMenu.generated.h"

/**
 * 
 */
USTRUCT()
struct FCrewInfo
{
	GENERATED_BODY()

	UPROPERTY()
		class UCrewSlotUI* Crew;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Crew_Num;
};

UCLASS()
class THELASTBASTION_API UCrewBar_RecruitMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	bool Initialize() override;

protected:

	UPROPERTY(meta = (BindWidget))
		class UCrewSlotUI* Crew_1;

	UPROPERTY(meta = (BindWidget))
		class UCrewSlotUI* Crew_2;

	UPROPERTY(meta = (BindWidget))
		class UCrewSlotUI* Crew_3;

	UPROPERTY(meta = (BindWidget))
		class UCrewSlotUI* Crew_4;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Crew_1_Num;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Crew_2_Num;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Crew_3_Num;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Crew_4_Num;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TotalNum;

	UPROPERTY()
		TArray<FCrewInfo> mCrewRow;

private:

	int totalAmount;

public:

	void OnOpenRecruitMenu();

	
	
};
