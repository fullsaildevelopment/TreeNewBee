// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionSlot.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UActionSlot : public UUserWidget
{
	GENERATED_BODY()
	


protected:

	bool Initialize() override;

	void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	FReply NativeOnMouseButtonDown
	(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;


	//void NativeOnDragEnter(const FGeometry& InGeometry, 
	//	const FDragDropEvent& InDragDropEvent, 
	//	UDragDropOperation* InOperation) override;

	//void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, 
	//	UDragDropOperation* InOperation) override;




protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (BlueprintProtected))
		// Image to tell what this action does
		class UTexture2D* mActionImage;

	UPROPERTY(meta = (BindWidget))
		class USizeBox* SlotSize;

	UPROPERTY(meta = (BindWidget))
		class UButton* ActionButton;

public:

	void SetSize(float _width, float _height);

	
	
};
