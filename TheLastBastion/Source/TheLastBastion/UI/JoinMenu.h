// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JoinMenu.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UJoinMenu : public UUserWidget
{
	GENERATED_BODY()

		enum ListToSwitch {ShowLobby = 0, NoLobby = 1, Searching = 2};

public:

    virtual bool Initialize();

private:

	UPROPERTY(meta = (BindWidget))
		class UButton* Back;
	UPROPERTY(meta = (BindWidget))
		class UButton* Refresh;
	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* ListSwitcher;
	UPROPERTY(meta = (BindWidget))
		class UScrollBox* LobbyList;

	UPROPERTY()
		class UGI_TheLastBastion* mGameInstanceRef;

	UPROPERTY()
		TArray<class UUserWidget*> Lobbies;


private:

	UFUNCTION()
		void OnBackClick();

	UFUNCTION()
		void OnRefreshClick();

public:
	
	void PopLobbyList(const TSharedRef<class FOnlineSessionSearch>& _searchSettings);

}; 
