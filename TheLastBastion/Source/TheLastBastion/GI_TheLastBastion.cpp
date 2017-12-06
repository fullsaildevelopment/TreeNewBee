// Fill out your copyright notice in the Description page of Project Settings.

#include "GI_TheLastBastion.h"
#include "Kismet/GameplayStatics.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "UI/JoinMenu.h"
#include "CustomType.h"



static FName SESSION_NAME = TEXT("TheLastBastion");

UGI_TheLastBastion::UGI_TheLastBastion(const FObjectInitializer & ObjectInitializer)
{

	UCustomType::FindClass<UUserWidget>(StartMenu_Class, TEXT("/Game/UI/MenuSystem/WBP_StartMenu"));
	UCustomType::FindClass<UUserWidget>(CustomizeMenu_Class, TEXT("/Game/UI/MenuSystem/WBP_CustomizeMenu"));
	UCustomType::FindClass<UUserWidget>(HostMenu_Class, TEXT("/Game/UI/MenuSystem/WBP_HostMenu"));
	UCustomType::FindClass<UUserWidget>(JoinMenu_Class, TEXT("/Game/UI/MenuSystem/WBP_JoinMenu"));
	UCustomType::FindClass<UUserWidget>(LoadingScreen_Class, TEXT("/Game/UI/MenuSystem/WBP_LoadingScreen"));

	playerSettingsSave = FString(TEXT("playerSettingsSave"));

}

void UGI_TheLastBastion::Init()
{

	UE_LOG(LogTemp, Warning, TEXT("GI Init"));

	IOnlineSubsystem* OnlineSubSystem = IOnlineSubsystem::Get();
	if (OnlineSubSystem)
	{

		UE_LOG(LogTemp, Warning, TEXT("Find online system %s"), *OnlineSubSystem->GetSubsystemName().ToString())
		mSessionInterface = OnlineSubSystem->GetSessionInterface();
		if (!mSessionInterface.IsValid())
			UE_LOG(LogTemp, Warning, TEXT("Can not Get Session Interface"));
		

		mSessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UGI_TheLastBastion::OnSessionCreateComplete);
		mSessionInterface->OnSessionFailureDelegates.AddUObject(this, &UGI_TheLastBastion::OnSessionCreateFailed);
		mSessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UGI_TheLastBastion::OnSessionDestroyComplete);
		mSessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UGI_TheLastBastion::OnSessionFindComplete);
		mSessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UGI_TheLastBastion::OnSessionJoinComplete);

		mSessionSearch = MakeShareable(new FOnlineSessionSearch);

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("can not find online subsystem"));
	}
}

FString UGI_TheLastBastion::GetPlayerSettingsSave() const
{
	return playerSettingsSave;
}

#pragma region Menus
void UGI_TheLastBastion::ShowMainMenu()
{
	ShowMenu(mStartMenu_Widget, StartMenu_Class);
}

void UGI_TheLastBastion::ShowCustomizeMenu()
{
	ShowMenu(mCustomizeMenu_Widget, CustomizeMenu_Class);
}

void UGI_TheLastBastion::ShowHostMenu()
{
	ShowMenu(mHostMenu_Widget, HostMenu_Class);
}

void UGI_TheLastBastion::ShowOptionMenu()
{
}

void UGI_TheLastBastion::ShowJoinMenu()
{
	ShowMenu(mJoinMenu_Widget, JoinMenu_Class);
}

void UGI_TheLastBastion::DisplayLoadingScreen()
{
	ShowMenu(mLoadingScreen_Widget, LoadingScreen_Class);
}

void UGI_TheLastBastion::ShowMenu(UUserWidget* & _widget, const TSubclassOf<class UUserWidget>& _class)
{

	APlayerController* const controller = GetWorld()->GetFirstPlayerController();
	if (_widget != nullptr)
	{
		_widget->AddToViewport();
	}
	else
	{
		if (_class)
		{
			_widget =  CreateWidget <UUserWidget>(this, _class);
			if (_widget != nullptr)
			{
				_widget->AddToViewport();
			}
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Widget Class not Set - Game Instance"));
	}

	if (controller->bShowMouseCursor == false)
		controller->bShowMouseCursor = true;
}


#pragma endregion


#pragma region Create && Join && Find Lobby

void UGI_TheLastBastion::LaunchLobby(int _numOfConnections)
{

	// check if the session exist?
	FNamedOnlineSession* session = mSessionInterface->GetNamedSession(SESSION_NAME);
	if (session != nullptr)
		DestroySession();

	CreateSession(bIsLan, _numOfConnections);

}

void UGI_TheLastBastion::FindLobby()
{
	// Find Session
	if (mSessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("start find session, bIslan %d"), bIsLan);
		mSessionSearch->bIsLanQuery = bIsLan;
		mSessionSearch->MaxSearchResults = 5;
		if (!bIsLan)
			mSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

		mSessionInterface->FindSessions(0, mSessionSearch.ToSharedRef());
	}
}

void UGI_TheLastBastion::CreateSession(bool _bIsLan, int _numOfConnections)
{
	if (mSessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("create session, bIslan %d"), bIsLan);

		FOnlineSessionSettings sessionSettings;
		sessionSettings.bIsLANMatch = _bIsLan;
		sessionSettings.NumPublicConnections = _numOfConnections;
		sessionSettings.bShouldAdvertise = true;
		if (!bIsLan)
			sessionSettings.bUsesPresence = true;

		mSessionInterface->CreateSession(0, SESSION_NAME, sessionSettings);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Interface is invalid during create"));
	}
}

void UGI_TheLastBastion::DestroySession()
{
	if (mSessionInterface.IsValid())
		mSessionInterface->DestroySession(SESSION_NAME);
	else
		UE_LOG(LogTemp, Warning, TEXT("Session Interface is invalid during destroy"));

}

void UGI_TheLastBastion::JoinServer(int _index)
{
	bool conditionCheck = mSessionInterface.IsValid() && mSessionSearch.IsValid();

	if (conditionCheck)
	{
		mSessionInterface->JoinSession(0, SESSION_NAME, mSessionSearch->SearchResults[_index]);
	}
}

void UGI_TheLastBastion::Host()
{
	LaunchLobby(5);
}

void UGI_TheLastBastion::OnSessionCreateComplete(FName _sessionName, bool _success)
{
	if (!_success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Create Session Failed"));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Hosting"));

		UWorld* world = GetWorld();
		if (world)
		{
			world->ServerTravel("/Game/Maps/Lobby?listen", true);
		}
	}
}

void UGI_TheLastBastion::OnSessionDestroyComplete(FName _sessionName, bool _success)
{
}

void UGI_TheLastBastion::OnSessionCreateFailed(const FUniqueNetId & _netId, ESessionFailure::Type _failureType)
{
}

void UGI_TheLastBastion::OnSessionJoinComplete(FName _sessionName, EOnJoinSessionCompleteResult::Type _result)
{
	if (mSessionInterface.IsValid())
	{
		FString address;
		mSessionInterface->GetResolvedConnectString(_sessionName, address);
		UE_LOG(LogTemp, Warning, TEXT("Connect to %s"), *address);

		APlayerController* pc = GetFirstLocalPlayerController();
		if (pc)
		{
			pc->ClientTravel(address, ETravelType::TRAVEL_Absolute);
		}
	}

}

void UGI_TheLastBastion::OnSessionFindComplete(bool _success)
{
	if (mSessionSearch.IsValid() && _success)
	{
		if (mJoinMenu_Widget != nullptr)
		{
			UJoinMenu* const joinMenu = Cast<UJoinMenu>(mJoinMenu_Widget);
			if (joinMenu)
			{
				joinMenu->PopLobbyList(mSessionSearch.ToSharedRef());
			}
		}
	}
}


#pragma endregion


// Check If there exsits a player profile save
void UGI_TheLastBastion::SaveGameCheck()
{
	APlayerController* const pc = GetWorld()->GetFirstPlayerController();
	if (pc)
		pc->bShowMouseCursor = true;
	else
		UE_LOG(LogTemp, Warning, TEXT("No pc exist - Game Instance Member:: Save Game Check"));

	bool bThereIsASavedProfile = UGameplayStatics::DoesSaveGameExist(playerSettingsSave, 0);

	if (bThereIsASavedProfile)
	{
		ShowMainMenu();
	}
	else
	{
		ShowCustomizeMenu();
	}

}

void UGI_TheLastBastion::SetIsLan(bool _val)
{
	bIsLan = _val;
}
