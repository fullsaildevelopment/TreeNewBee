// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionSlot.generated.h"

/**
 * 
 */

#define DefaultActionSlot_Width 64
#define DefaultActionSlot_Height 64


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


protected:

	static TSubclassOf<class UUserWidget> WBP_DraggedItem;

	UPROPERTY(meta = (BindWidget))
		class USizeBox* SlotSize;

	UPROPERTY(meta = (BindWidget))
		class UButton* ActionButton;

public:

	void SetSize(float _width, float _height);

	/** Set the Image of action slot*/
	virtual void SetImage(class UTexture2D* _image) {}


};
