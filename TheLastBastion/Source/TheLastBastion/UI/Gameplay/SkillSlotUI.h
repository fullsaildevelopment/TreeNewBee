// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillSlotUI.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API USkillSlotUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	bool Initialize() override;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* CoolDownBar;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Key;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		UTexture2D* TN_Image;

	float DivByCoolDownTime;

	/** Passed time from cool down start*/
	float TimePassed;

	bool bIsCoolingDown;

public:

	void SetKeyText(FText _keyText);

	/** Called when the skill is launched, start the launch bar progress*/
	void OnCoolDownStart(float _cdDuration);

	void OnCoolDownFinish();

	FORCEINLINE void SetSkillTN(UTexture2D* _tnImage) { TN_Image = _tnImage; }

	FORCEINLINE bool IsCooledDown() const {return !bIsCoolingDown;}


};
