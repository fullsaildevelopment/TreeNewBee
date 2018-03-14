// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillBarUI.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API USkillBarUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	bool Initialize() override;

protected:

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* SkillRow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<UTexture2D*> SkillThumbNails;

public:

	void OnLaunchSkillAt(int _skillIndex, float _cdDuration);

	bool IsCooledDownAt(int _skillIndex) const;
	
};
