// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyPlayerRow.generated.h"

/**
*
*/
UCLASS()
class THELASTBASTION_API ULobbyPlayerRow : public UUserWidget
{
	GENERATED_BODY()


protected:

	bool Initialize() override;

protected:

	/** Player profile image */
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
		class UTexture2D* mPlayerAvatar;

	/** The character image, player selected */
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
		class UTexture2D* mCharacterAvatar;

	/** Indicates whether this player is ready to play or not */
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* PlayerStatus;

	/** Player name from profile */
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* PlayerName;

public:

	void FillLobbyPlayerRow(const struct FPlayerProfile& _incomingPlayer);
};
