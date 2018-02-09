// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrewBarUI.generated.h"

/**
 * 
 */
USTRUCT()
struct FCrew
{
	GENERATED_BODY()

	UPROPERTY()
		class UCrewSlotUI* Crew;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Crew_TEXT;
};

UCLASS()
class THELASTBASTION_API UCrewBarUI : public UUserWidget
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
		class UTextBlock* Crew_1_TEXT;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Crew_2_TEXT;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Crew_3_TEXT;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Crew_4_TEXT;

	UPROPERTY()
		TArray<FCrew> mCrewBar;

private:

	int mCurrentIndex;

public:

	void OnSelectedCrewAt(int _index);
	
	
};
