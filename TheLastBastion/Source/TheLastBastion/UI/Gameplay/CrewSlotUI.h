// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrewSlotUI.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UCrewSlotUI : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	bool Initialize() override;

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		UTexture2D* Crew_Image;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CrewSize;


public:

	void OnCrewMemberDead();

	void OnCrewSelected();

};
