// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomizeMenu.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UCustomizeMenu : public UUserWidget
{
	GENERATED_BODY()
	
	
	UCustomizeMenu(const FObjectInitializer& _objectInitailizer);
	

protected:
	virtual bool Initialize();

private:

#pragma region Widget Components


	UPROPERTY(meta = (BindWidget))
		class UButton* Accept;
	UPROPERTY(meta = (BindWidget))
		class UButton* Back;
	UPROPERTY(meta = (BindWidget))
		class UButton* ToggleLeft;
	UPROPERTY(meta = (BindWidget))
		class UButton* ToggleRight;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WelcomeMessage;
	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* PlayerName;


#pragma endregion


	UPROPERTY()
		class UGI_TheLastBastion* mGameInstanceRef;

	UPROPERTY()
		class USaveGame_TheLastBastion* mSaveGame;

	unsigned mCurrentAvatarSelectionIndex;


protected:

	// Binding to CurrentAvatar_Image
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
		class UTexture2D* mCurrentAvatar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<class UTexture2D*> allAvatars;


private:

	UFUNCTION()
		void OnBackClick();

	UFUNCTION()
		void OnAcceptClick();

	UFUNCTION()
		void OnToggleLeftClick();

	UFUNCTION()
		void OnToggleRightClick();

	UFUNCTION()
		void OnPlayerNameChange(const FText& _text);


	// Check if there is a locally saved player profile during init
	void PlayerProfileSaveCheck();

};

	