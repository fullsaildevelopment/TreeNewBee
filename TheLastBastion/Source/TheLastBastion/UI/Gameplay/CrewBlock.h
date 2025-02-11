// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrewBlock.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UCrewBlock : public UUserWidget
{
	GENERATED_BODY()
	
	
protected:

	bool Initialize() override;

	bool NativeOnDrop(const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;

protected:

	UPROPERTY(meta = (BindWidget))
		class UCrewSlotUI* CrewSlot;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AllyIndex_Text;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CrewNum_Text;

	UPROPERTY(meta = (BindWidget))
		class UButton* Minus;

	UPROPERTY(meta = (BindWidget))
		class UButton* Plus;

	UPROPERTY(meta = (BindWidget))
		class UButton* Dismiss;

	UPROPERTY(meta = (BindWidget))
		class UButton* FastRecruit;

private:

	int AllyIndex;
	int CrewNum;

	UPROPERTY()
		class UCrewBar_RecruitMenu* mCrewBar;


private:

	UFUNCTION()
		void OnPlusClick();

	UFUNCTION()
		void OnMinusClick();

	UFUNCTION()
		void OnFastRecruitClick();

	UFUNCTION()
		void OnDismissClick();


	/** Called when during drop on event, add 5 new unit as default to the this crew block*/
	void OnAddingNewUnit(const class UUnitDrag* unitDragOp);

public:

	/** The ally block number and ally index to match the data in gamemode*/
	void SetAllyIndex(int _index); 

	FORCEINLINE void SetParent(class UCrewBar_RecruitMenu* _parent) { mCrewBar = _parent; }	
	FORCEINLINE class UCrewSlotUI* GetCrewSlot() const { return CrewSlot; }
	FORCEINLINE int GetCrewNum() const { return CrewNum; }

	TSubclassOf<class ATheLastBastionAIBase> GetUnitClass() const; 

	void SetCrewNum(int _groupSize);
	void SetOperationEnabled(int _enabled);
	void SetDismissEnabled(bool _enable);


private:

	/** Update resource after purchase preview */
	void UpdateResourcePreview(int _num);//, TSubclassOf<ATheLastBastionAIBase> _unitClass);



	
};
