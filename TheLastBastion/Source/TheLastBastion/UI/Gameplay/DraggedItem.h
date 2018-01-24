// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DraggedItem.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UDraggedItem : public UUserWidget
{
	GENERATED_BODY()
	
protected:
		bool Initialize() override;


protected:

		UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (BlueprintProtected))
		// Image to tell what this action does
		UTexture2D* DraggedImage;

		UPROPERTY(meta = (BindWidget))
		class USizeBox* ItemSize;

public:

	void SetItemSize(float _width, float _height);

	void SetImage(UTexture2D* _image);

};
