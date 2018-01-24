// Fill out your copyright notice in the Description page of Project Settings.

#include "DraggedItem.h"
#include "Components/SizeBox.h"

bool UDraggedItem::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	bool bAllWidgetAreGood =
		ItemSize != nullptr;

	if (bAllWidgetAreGood)
	{

	}
	else
		return false;

	ItemSize->SetWidthOverride(true);
	ItemSize->SetHeightOverride(true);

	ItemSize->WidthOverride = 120;
	ItemSize->HeightOverride = 120;

	return true;

}

void UDraggedItem::SetItemSize(float _width, float _height)
{
	ItemSize->WidthOverride = _width;
	ItemSize->HeightOverride = _height;
}

void UDraggedItem::SetImage(UTexture2D * _image)
{
	DraggedImage = _image;
}
