// Fill out your copyright notice in the Description page of Project Settings.

#include "GI_TheLastBastion.h"
#include "Kismet/GameplayStatics.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "UI/JoinMenu.h"
#include "CustomType.h"



static FName SESSION_NAME = TEXT("GAME");

UGI_TheLastBastion::UGI_TheLastBastion(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{

	UCustomType::FindClass<UUserWidget>(StartMenu_Class, TEXT("/Game/UI/MenuSystem/WBP_StartMenu"));
	UCustomType::FindClass<UUserWidget>(CustomizeMenu_Class, TEXT("/Game/UI/MenuSystem/WBP_CustomizeMenu"));
	UCustomType::FindClass<UUserWidget>(HostMenu_Class, TEXT("/Game/UI/MenuSystem/WBP_HostMenu"));
	UCustomType::FindClass<UUserWidget>(JoinMenu_Class, TEXT("/Game/UI/MenuSystem/WBP_JoinMenu"));
	UCustomType::FindClass<UUserWidget>(LoadingScreen_Class, TEXT("/Game/UI/MenuSystem/WBP_LoadingScreen"));

	playerSettingsSave = FString(TEXT("playerSettingsSave"));

	mOnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UGI_TheLastBastion::OnSessionDestroyComplete);
	mOnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UGI_TheLastBastion::OnSessionCreateComplete);
	mOnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UGI_TheLastBastion::OnStartOnlineGameComplete);
	mOnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UGI_TheLastBastion::OnSessionFindComplete);
	mOnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UGI_TheLastBastion::OnSessionJoinComplete);

	LobbyMap = TEXT("Lobby");
}

void UGI_TheLastBastion::Init()
{
	mOnlineSubSystem = IOnlineSubsystem::Get();
	if (mOnlineSubSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Find online system %s"), *mOnlineSubSystem->GetSubsystemName().ToString())
		mSessionInterface = mOnlineSubSystem->GetSessionInterface();
		if (!mSessionInterface.IsValid())
			UE_LOG(LogTemp, Warning, TEXT("Can not Get Session Interface"));		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can not find online subsystem during initialization"));		
	}

	if (mOnlineSubSystem && mSessionInterface.IsValid())
	{
		mSessionSearch = MakeShareable(new FOnlineSessionSearch);
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

bool UGI_TheLastBastion::OnlineSubSystemCheck()
{
	mOnlineSubSystem = IOnlineSubsystem::Get();
	if (mOnlineSubSystem)
	{
		mSessionInterface = mOnlineSubSystem->GetSessionInterface();
		if (mSessionInterface.IsValid())
			return true;
		else
			return false;
	}
	else return false;
}
const TSharedPtr<const FUniqueNetId> UGI_TheLastBastion::GetUserId()
{
	return this->GetFirstLocalPlayerController()->PlayerState->UniqueId.GetUniqueNetId();
}


void UGI_TheLastBastion::LaunchLobby(int _numOfConnections)
{
	mNumOfConnection = _numOfConnections;
	if (DestroySession(true) == false)
	{
		HostSession(bIsLan, mNumOfConnection);
	}
}

bool UGI_TheLastBastion::HostSession(bool _bIsLan, int _numOfConnections)
{
	if (OnlineSubSystemCheck())
	{
		const TSharedPtr<const FUniqueNetId>  userId = GetUserId();
		UE_LOG(LogTemp, Warning, TEXT("Host lobby Userid: %s"), *userId->ToString());
		if (userId.IsValid())
		{
			mOnCreateSessionCompleteDelegateHandle 
				= mSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(mOnCreateSessionCompleteDelegate);

			UE_LOG(LogTemp, Warning, TEXT("Host for a LAN? %d, connectionNum? %d"), _bIsLan, _numOfConnections);
			FOnlineSessionSettings sessionSettings;
			sessionSettings.bIsLANMatch = _bIsLan;
			sessionSettings.NumPublicConnections = _numOfConnections;
			sessionSettings.bShouldAdvertise = true;
			sessionSettings.bAllowJoinInProgress = true;
			sessionSettings.bUsesPresence = true;
			sessionSettings.bAllowJoinViaPresence = true;
			//sessionSettings.NumPrivateConnections = 0;
			//sessionSettings.bAllowInvites = false;
			//sessionSettings.bAllowJoinInProgress = true;
			//sessionSettings.bAllowJoinViaPresence = true;
			//sessionSettings.bAllowJoinViaPresenceFriendsOnly = false
			//sessionSettings.Set(SETTING_MAPNAME, FString("Lobby"), EOnlineDataAdvertisementType::ViaOnlineService);			
			UE_LOG(LogTemp, Warning, TEXT("Creating Session, %s"), *SESSION_NAME.ToString());
			return mSessionInterface->CreateSession(*userId, SESSION_NAME, sessionSettings);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("userId is not valid during Host session"));
			return false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSubSystemCheck is failed during Host session"));
		return false;
	}
}

void UGI_TheLastBastion::FindLobby()
{
	if (OnlineSubSystemCheck())
	{
		const TSharedPtr<const FUniqueNetId>  userId = GetUserId();
		UE_LOG(LogTemp, Warning, TEXT("Find Lobby Userid: %s"), *userId->ToString());
		if (userId.IsValid())
		{

			mOnFindSessionssCompleteDelegateHandle 
				= mSessionInterface ->AddOnFindSessionsCompleteDelegate_Handle(mOnFindSessionsCompleteDelegate);

			UE_LOG(LogTemp, Warning, TEXT("Search for a LAN? %d"), bIsLan);
			mSessionSearch->bIsLanQuery = bIsLan;
			mSessionSearch->MaxSearchResults = 50;
			mSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

			mSessionInterface ->FindSessions(*userId, mSessionSearch.ToSharedRef());
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("userId is not valid during Find Lobby"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSubSystemCheck is failed during Host session"));
		OnSessionFindComplete(false);
	}

	//ULocalPlayer* const Lp = GetFirstGamePlayer();
	//TSharedPtr<const FUniqueNetId> userId = Lp->GetPreferredUniqueNetId();
	//IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	//if (OnlineSub == nullptr)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("OnlineSub is null during FindLobby"));
	//	OnSessionFindComplete(false);
	//	return;
	//}
	//else
	//{
	//	IOnlineSessionPtr sessionInterface = OnlineSub->GetSessionInterface();
	//	if (sessionInterface.IsValid() && userId.IsValid())
	//	{
	//		mOnFindSessionssCompleteDelegateHandle = sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(mOnFindSessionsCompleteDelegate);
	//		UE_LOG(LogTemp, Warning, TEXT("Search for a LAN? %d"), bIsLan);
	//		mSessionSearch->bIsLanQuery = bIsLan;
	//		mSessionSearch->MaxSearchResults = 50;
	//		mSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	//		sessionInterface->FindSessions(*userId, mSessionSearch.ToSharedRef());
	//	}
	//	else
	//	{
	//		if (userId.IsValid())
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("sessionInterface is not valid during FindLobby"));
	//		}
	//		else
	//			UE_LOG(LogTemp, Warning, TEXT("_userId is not valid during FindLobby"));
	//	}
	//}
}

bool UGI_TheLastBastion::DestroySession(bool _recreate)
{
	if (OnlineSubSystemCheck())
	{
		FNamedOnlineSession* session = mSessionInterface->GetNamedSession(SESSION_NAME);
		bRecreateSession = _recreate;
		if (session != nullptr)
		{
			mSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(mOnDestroySessionCompleteDelegate);
			mSessionInterface->DestroySession(SESSION_NAME);
			UE_LOG(LogTemp, Warning, TEXT("Destroying Session, %s"), *SESSION_NAME.ToString());
			return true;
		}
		else
		{
			bRecreateSession = false;
			return false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSubSystemCheck failed during destroy"));
		return false;
	}
}

void UGI_TheLastBastion::JoinServer(int _index)
{
	if (OnlineSubSystemCheck())
	{
		const TSharedPtr<const FUniqueNetId>  userId = GetUserId();
		UE_LOG(LogTemp, Warning, TEXT("Join, Userid: %s"), *userId->ToString());
		mOnJoinSessionCompleteDelegateHandle = mSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(mOnJoinSessionCompleteDelegate);
		mSessionInterface->JoinSession(*userId, SESSION_NAME, mSessionSearch->SearchResults[_index]);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSubSystemCheck Failed during Join Server"));
	}
}

void UGI_TheLastBastion::OnSessionCreateComplete(FName _sessionName, bool _success)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete %s, %d"), *_sessionName.ToString(), _success);

	if (OnlineSubSystemCheck())
	{
		mSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(mOnCreateSessionCompleteDelegateHandle);
		if (_success)
		{
			mOnStartSessionCompleteDelegateHandle = mSessionInterface->AddOnStartSessionCompleteDelegate_Handle(mOnStartSessionCompleteDelegate);
			mSessionInterface->StartSession(SESSION_NAME);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSubSystemCheck failed during OnSessionCreateComplete"));
	}
}

void UGI_TheLastBastion::OnStartOnlineGameComplete(FName _sessionName, bool _success)
{
	UE_LOG(LogTemp, Warning, TEXT("OnStartOnlineGameComplete %s, %d"), *_sessionName.ToString(), _success);

	if (OnlineSubSystemCheck())
		mSessionInterface->ClearOnStartSessionCompleteDelegate_Handle(mOnStartSessionCompleteDelegateHandle);
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSubSystemCheck failed during OnStartOnlineGameComplete"));
	}

	if (_success)
		UGameplayStatics::OpenLevel(GetWorld(), LobbyMap, true, "listen");
}

void UGI_TheLastBastion::OnSessionDestroyComplete(FName _sessionName, bool _success)
{
	UE_LOG(LogTemp, Warning, TEXT("OnSessionDestroyComplete %s, %d"), *_sessionName.ToString(), _success);

	if (OnlineSubSystemCheck())
	{
		mSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(mOnDestroySessionCompleteDelegateHandle);
	}
	if (_success)
	{
		if (bRecreateSession)
		{
			HostSession(bIsLan, mNumOfConnection);
		}
	}
	bRecreateSession = false;
}

void UGI_TheLastBastion::OnSessionJoinComplete(FName _sessionName, EOnJoinSessionCompleteResult::Type _result)
{
	if (OnlineSubSystemCheck())
	{
		mSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(mOnJoinSessionCompleteDelegateHandle);
		FString address;
		UE_LOG(LogTemp, Warning, TEXT("Connect to %s"), *address);

		APlayerController* pc = GetFirstLocalPlayerController();
		if (pc && mSessionInterface->GetResolvedConnectString(_sessionName, address))
		{
			pc->ClientTravel(address, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UGI_TheLastBastion::OnSessionFindComplete(bool _success)
{

	UE_LOG(LogTemp, Warning, TEXT("OnSessionFindComplete %d"), _success);

	if (OnlineSubSystemCheck())
	{
		mSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(mOnFindSessionssCompleteDelegateHandle);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSubSystemCheck Failed during OnSessionFindComplete %d"));
	}

	if (_success)
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

void UGI_TheLastBastion::BackToMainMenu()
{
	FNamedOnlineSession* session = mSessionInterface->GetNamedSession(SESSION_NAME);
	if (session != nullptr)
		DestroySession(false);


	ShowMainMenu();
	UWorld* world = GetWorld();
	UGameplayStatics::OpenLevel(world, TEXT("StartMenu_Map"), true);
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
