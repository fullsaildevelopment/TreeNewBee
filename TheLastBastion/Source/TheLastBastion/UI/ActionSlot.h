// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "ActionSlot.generated.h"

/**
 * 
 */

#define DefaultActionSlot_Width 64
#define DefaultActionSlot_Height 64


UENUM()
enum class EDragDropMode :uint8
{
	EDragAndDrop,
	EDragOnly,
	EDropOnly,
	EDisable
};

UCLASS()
class THELASTBASTION_API UActionSlot : public UUserWidget
{
	GENERATED_BODY()

public:

	UActionSlot(const FObjectInitializer& _objInit);

protected:

	bool Initialize() override;

	FReply NativeOnMouseButtonDown
	(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	//void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;


	/** Remove the pop up widget from viewport*/
	void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

protected:

	static TSubclassOf<class UUserWidget> WBP_DraggedItem;

	/** Widget class to pop up and show information*/
	static TSubclassOf<class UUserWidget> WBP_PopUpWidget;

	UPROPERTY()
		/** The pop up widget to show information*/
		class UUserWidget* PopUpWidget;

	UPROPERTY(meta = (BindWidget))
		class USizeBox* SlotSize;

	EDragDropMode DragDropMode;

public:

	///** if true, will enable the button click, and set drag drop operation to disable
	//	if false, will only disable the button click*/
	//void SetIsButton(bool _val);

	void SetDragDropMode(EDragDropMode _mode);

	FORCEINLINE bool IsDragEnabled() const { return DragDropMode == EDragDropMode::EDragOnly || DragDropMode == EDragDropMode::EDragAndDrop; }
	FORCEINLINE bool IsDropEnabled() const { return DragDropMode == EDragDropMode::EDropOnly || DragDropMode == EDragDropMode::EDragAndDrop; }
	
	void SetSize(float _width, float _height);

	/** Set the Image of action slot*/
	virtual void SetImage(class UTexture2D* _image) {}

protected:

	UFUNCTION()
		virtual void OnButtonClick() {}



};
