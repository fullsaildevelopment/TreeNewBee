// Fill out your copyright notice in the Description page of Project Settings.

#include "UnitDrag.h"



UUnitDrag::UUnitDrag(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	Pivot = EDragPivot::MouseDown;
}
