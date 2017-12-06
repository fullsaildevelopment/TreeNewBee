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
		
		//mSessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UGI_TheLastBastion::OnSessionCreateComplete);
		//mSessionInterface->OnSessionFailureDelegates.AddUObject(this, &UGI_TheLastBastion::OnSessionCreateFailed);
		//mSessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UGI_TheLastBastion::OnSessionDestroyComplete);
		//mSessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UGI_TheLastBastion::OnSessionFindComplete);
		//mSessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UGI_TheLastBastion::OnSessionJoinComplete);

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
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub == nullptr)
	{
		return;
	}
	else
	{
		IOnlineSessionPtr sessionInterface = OnlineSub->GetSessionInterface();
		if (sessionInterface.IsValid())
		{
			FNamedOnlineSession* session = sessionInterface->GetNamedSession(SESSION_NAME);
			mNumOfConnection = _numOfConnections;
			if (session)
			{
				DestroySession(true);			 
			}
			else
			{
				//ULocalPlayer* const Lp = this->GetFirstGamePlayer();
				APlayerController* const pc = this->GetFirstLocalPlayerController();
				const TSharedPtr<const FUniqueNetId>  userId =   pc->PlayerState->UniqueId.GetUniqueNetId();
				UE_LOG(LogTemp, Warning, TEXT("Userid: %s"), *userId->ToString());
				HostSession(userId, bIsLan, mNumOfConnection);
			}
		}
	}
}

bool UGI_TheLastBastion::HostSession(TSharedPtr<const FUniqueNetId> _userId, bool _bIsLan, int _numOfConnections)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSub is null during Host session"));
		return false;
	}
	else
	{
		IOnlineSessionPtr sessionInterface = OnlineSub->GetSessionInterface();
		if (sessionInterface.IsValid() && _userId.IsValid())
		{

			mOnCreateSessionCompleteDelegateHandle = sessionInterface->AddOnCreateSessionCompleteDelegate_Handle(mOnCreateSessionCompleteDelegate);

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

			
			return sessionInterface->CreateSession(*_userId, SESSION_NAME, sessionSettings);
		}
		else
		{
			if (_userId.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("sessionInterface is not valid during Host session"));
			}
			else
				UE_LOG(LogTemp, Warning, TEXT("_userId is not valid during Host session"));

			return false;
		}
	}

}

void UGI_TheLastBastion::FindLobby()
{
	// Find Session
	//if (mSessionSearch.IsValid())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("start find session, bIslan %d"), bIsLan);
	//	mSessionSearch->bIsLanQuery = bIsLan;
	//	mSessionSearch->MaxSearchResults = 5;
	//	if (!bIsLan)
	//		mSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	//	mSessionInterface->FindSessions(0, mSessionSearch.ToSharedRef());
	//}

	ULocalPlayer* const Lp = GetFirstGamePlayer();
	TSharedPtr<const FUniqueNetId> userId = Lp->GetPreferredUniqueNetId();

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSub is null during FindLobby"));
		OnSessionFindComplete(false);

		return;
	}
	else
	{
		IOnlineSessionPtr sessionInterface = OnlineSub->GetSessionInterface();
		if (sessionInterface.IsValid() && userId.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Search for a LAN? %d"), bIsLan);
			mSessionSearch->bIsLanQuery = bIsLan;
			mSessionSearch->MaxSearchResults = 50;
			//mSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

			mOnFindSessionssCompleteDelegateHandle = sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(mOnFindSessionsCompleteDelegate);
			sessionInterface->FindSessions(*userId, mSessionSearch.ToSharedRef());
		}
		else
		{
			if (userId.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("sessionInterface is not valid during FindLobby"));
			}
			else
				UE_LOG(LogTemp, Warning, TEXT("_userId is not valid during FindLobby"));

		}
	}
}

//void UGI_TheLastBastion::HostSession(bool _bIsLan, int _numOfConnections)
//{
//	if (mSessionInterface.IsValid())
//	{
//		UE_LOG(LogTemp, Warning, TEXT("create session, bIslan %d"), bIsLan);
//
//		FOnlineSessionSettings sessionSettings;
//		sessionSettings.bIsLANMatch = _bIsLan;
//		sessionSettings.NumPublicConnections = _numOfConnections;
//		sessionSettings.bShouldAdvertise = true;
//		if (!bIsLan)
//			sessionSettings.bUsesPresence = true;
//
//		mSessionInterface->CreateSession(0, SESSION_NAME, sessionSettings);
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Session Interface is invalid during create"));
//	}
//}

void UGI_TheLastBastion::DestroySession(bool _recreate)
{

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			Sessions->AddOnDestroySessionCompleteDelegate_Handle(mOnDestroySessionCompleteDelegate);
			bRecreateSession = _recreate;
			Sessions->DestroySession(SESSION_NAME);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Session Interface is invalid during destroy"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSub is null during destroy"));
	}
	//if (mSessionInterface.IsValid())
	//	mSessionInterface->DestroySession(SESSION_NAME);
	//else
	//	UE_LOG(LogTemp, Warning, TEXT("Session Interface is invalid during destroy"));

}

void UGI_TheLastBastion::JoinServer(int _index)
{
	bool conditionCheck = mSessionInterface.IsValid() && mSessionSearch.IsValid();

	if (conditionCheck)
	{
		APlayerController* const pc = this->GetFirstLocalPlayerController();
		const TSharedPtr<const FUniqueNetId>  userId = pc->PlayerState->UniqueId.GetUniqueNetId();
		UE_LOG(LogTemp, Warning, TEXT("Userid: %s"), *userId->ToString());
		mOnJoinSessionCompleteDelegateHandle = mSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(mOnJoinSessionCompleteDelegate);

		mSessionInterface->JoinSession(*userId, SESSION_NAME, mSessionSearch->SearchResults[_index]);
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

void UGI_TheLastBastion::OnSessionCreateComplete(FName _sessionName, bool _success)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete %s, %d"), *_sessionName.ToString(), _success);
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSub is null OnSessionCreateComplete"));
	}
	else
	{
		IOnlineSessionPtr sessionInterface = OnlineSub->GetSessionInterface();
		if (sessionInterface.IsValid())
		{
			sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(mOnCreateSessionCompleteDelegateHandle);
			if (_success)
			{
				mOnStartSessionCompleteDelegateHandle = sessionInterface->AddOnStartSessionCompleteDelegate_Handle(mOnStartSessionCompleteDelegate);
				sessionInterface->StartSession(SESSION_NAME);
			}
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("sessionInterface is not valid OnSessionCreateComplete"));
	}

	//if (!_success)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Create Session Failed"));
	//	return;
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Hosting"));
	//	UWorld* world = GetWorld();
	//	UGameplayStatics::OpenLevel(world, TEXT("Lobby"), true);
	//}
}

void UGI_TheLastBastion::OnStartOnlineGameComplete(FName _sessionName, bool _success)
{
	UE_LOG(LogTemp, Warning, TEXT("OnStartOnlineGameComplete %s, %d"), *_sessionName.ToString(), _success);
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSub is null OnStartOnlineGameComplete"));
	}
	else
	{
		IOnlineSessionPtr sessionInterface = OnlineSub->GetSessionInterface();
		if (sessionInterface.IsValid())
		{
			sessionInterface->ClearOnStartSessionCompleteDelegate_Handle(mOnStartSessionCompleteDelegateHandle);
		}
	}

	if (_success)
	{
		UGameplayStatics::OpenLevel(GetWorld(), "Lobby", true, "listen");
	}
}



void UGI_TheLastBastion::OnSessionDestroyComplete(FName _sessionName, bool _success)
{
	UE_LOG(LogTemp, Warning, TEXT("OnSessionDestroyComplete %s, %d"), *_sessionName.ToString(), _success);
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSub is null OnSessionDestroyComplete"));
	}
	else
	{
		IOnlineSessionPtr sessionInterface = OnlineSub->GetSessionInterface();
		if (sessionInterface.IsValid())
		{
			sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(mOnDestroySessionCompleteDelegateHandle);
		}
	}

	if (_success)
	{
		if (bRecreateSession)
		{
			APlayerController* const pc = this->GetFirstLocalPlayerController();
			const TSharedPtr<const FUniqueNetId>  userId = pc->PlayerState->UniqueId.GetUniqueNetId();
			UE_LOG(LogTemp, Warning, TEXT("Userid: %s"), *userId->ToString());
			HostSession(userId, bIsLan, mNumOfConnection);
		}
	}
	bRecreateSession = false;
}

void UGI_TheLastBastion::OnSessionCreateFailed(const FUniqueNetId & _netId, ESessionFailure::Type _failureType)
{
}

void UGI_TheLastBastion::OnSessionJoinComplete(FName _sessionName, EOnJoinSessionCompleteResult::Type _result)
{

	if (mSessionInterface.IsValid())
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

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSub is null OnSessionFindComplete"));
	}
	else
	{
		IOnlineSessionPtr sessionInterface = OnlineSub->GetSessionInterface();
		if (sessionInterface.IsValid())
		{
			sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(mOnFindSessionssCompleteDelegateHandle);
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
	}


	//if (mSessionSearch.IsValid() && _success)
	//{
	//	if (mJoinMenu_Widget != nullptr)
	//	{
	//		UJoinMenu* const joinMenu = Cast<UJoinMenu>(mJoinMenu_Widget);
	//		if (joinMenu)
	//		{
	//			joinMenu->PopLobbyList(mSessionSearch.ToSharedRef());
	//		}
	//	}
	//}
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
