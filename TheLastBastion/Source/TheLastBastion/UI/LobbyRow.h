// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyRow.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API ULobbyRow : public UUserWidget
{
	GENERATED_BODY()
	
    virtual bool Initialize();

private:

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* LobbyName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* HostName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Players;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Ping;

	UPROPERTY(meta = (BindWidget))
		class UButton* Lobby;

	int index;

	UPROPERTY()
		class UGI_TheLastBastion* mGameInstanceRef;

public:

	// Disable the button, and make it a simple heading for the entire list
	void MakeThisAHeader();

	UFUNCTION()
		void SetRowProperty(const FText & _ServerName, const FText& _HostName, const FText & _Players, const FText & _ping, int _index);

	UFUNCTION()
		void OnLobbyClick();
	
};
