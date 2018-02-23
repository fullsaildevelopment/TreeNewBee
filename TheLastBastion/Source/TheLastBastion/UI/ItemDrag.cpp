// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemDrag.h"


UItemDrag::UItemDrag(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Pivot = EDragPivot::MouseDown;
}



