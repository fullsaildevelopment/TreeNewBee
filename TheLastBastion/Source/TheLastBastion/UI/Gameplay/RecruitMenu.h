// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Gameplay/CrewSlotUI.h"
#include "Blueprint/UserWidget.h"
#include "RecruitMenu.generated.h"

/**
 * 
 */


UCLASS()
class THELASTBASTION_API URecruitMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	bool Initialize() override;

protected:

	UPROPERTY(meta = (BindWidget))
		/** the gear list that allow player to upgrade */
		class UScrollBox* UnitList;

	UPROPERTY(meta = (BindWidget))
		/** the gear list that allow player to upgrade */
		class UCrewBar_RecruitMenu* AllMyCrew;

	UPROPERTY(meta = (BindWidget))
		class UUnitRow* HillTribe;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UnitLists")
		TArray<struct FUnitData> HillTribe_Data;


	UPROPERTY(meta = (BindWidget))
		class UButton* Cancel;

	UPROPERTY(meta = (BindWidget))
		class UButton* Accept;

	bool bIsOpened;



public:

	void OnOpenRecruitMenu();

	FORCEINLINE bool IsOpened() const { return bIsOpened; }


	void AddWood(int _val);

	void AddFood(int _val);

	void AddMetal(int _val);

	void AddStone(int _val);



private:

	UFUNCTION()
		void OnAcceptClicked();

	UFUNCTION()
		void OnCancelClicked();
	
};
