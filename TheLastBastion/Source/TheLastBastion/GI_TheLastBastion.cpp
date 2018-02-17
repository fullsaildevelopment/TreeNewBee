// Fill out your copyright notice in the Description page of Project Settings.

#include "GI_TheLastBastion.h"

#include "Kismet/GameplayStatics.h"
#include "SaveGame/SaveGame_TheLastBastion.h"
#include "OnlineSubsystem.h"
#include "TheLastBastionCharacter.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "UI/JoinMenu.h"
#include "UI/StartMenu.h"
#include "CustomType.h"
#include "UI/InGameHUD.h"



static FName SESSION_NAME = TEXT("GAME");

UGI_TheLastBastion::UGI_TheLastBastion(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
	//LocateAllVFX();
	LocateAllWidgetClass();
	//LocateAllCharacterClass();

	playerSettingsSave = FString(TEXT("playerSettingsSave"));

	mOnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UGI_TheLastBastion::OnSessionDestroyComplete);
	mOnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UGI_TheLastBastion::OnSessionCreateComplete);
	mOnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UGI_TheLastBastion::OnStartOnlineGameComplete);
	mOnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UGI_TheLastBastion::OnSessionFindComplete);
	mOnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UGI_TheLastBastion::OnSessionJoinComplete);

	mLobbyName = TEXT("NewLobby");
	LobbyMap = TEXT("Lobby");
	mNumOfConnection = 4;

}

void UGI_TheLastBastion::LocateAllWidgetClass()
{
	// Find Class for MainMenus
	UCustomType::FindClass<UUserWidget>(StartMenu_Class, TEXT("/Game/UI/MenuSystem/WBP_StartMenu"));
	UCustomType::FindClass<UUserWidget>(CustomizeMenu_Class, TEXT("/Game/UI/MenuSystem/WBP_CustomizeMenu"));
	UCustomType::FindClass<UUserWidget>(HostMenu_Class, TEXT("/Game/UI/MenuSystem/WBP_HostMenu"));
	UCustomType::FindClass<UUserWidget>(JoinMenu_Class, TEXT("/Game/UI/MenuSystem/WBP_JoinMenu"));
	UCustomType::FindClass<UUserWidget>(LoadingScreen_Class, TEXT("/Game/UI/MenuSystem/WBP_LoadingScreen"));
	UCustomType::FindClass<UUserWidget>(LobbyRow_Class, TEXT("/Game/UI/MenuSystem/WBP_LobbyRow"));

	// Find Class for Lobby Map
	UCustomType::FindClass<UUserWidget>(LobbyMenu_Class, TEXT("/Game/UI/Lobby/WBP_LobbyMenu"));
	UCustomType::FindClass<UUserWidget>(LobbyPlayerRow_Class, TEXT("/Game/UI/Lobby/WBP_LobbyPlayerRow"));

	// Find Class for Game Map
	UCustomType::FindClass<UUserWidget>(InGameHUD_Class, TEXT("/Game/UI/In-Game/WBP_InGameHUD"));
	UCustomType::FindClass<UUserWidget>(InGamePlayerRow_Class, TEXT("/Game/UI/In-Game/WBP_InGamePlayerRow"));
	UCustomType::FindClass<UUserWidget>(InGameTeamRow_Class, TEXT("/Game/UI/In-Game/WBP_InGameTeamRow"));

}

//void UGI_TheLastBastion::LocateAllCharacterClass()
//{
//	UCustomType::FindClass<ACharacter>(Ranger_Class, TEXT("/Game/Blueprints/Heros/Ranger_Bp"));
//}

void UGI_TheLastBastion::LocateAllVFX()
{
	//ConstructorHelpers::FObjectFinder<UParticleSystem> bloodImpactFind (TEXT("/Game/Assets/Partical/WeaponEffects/BloodImpact/P_blood_splash_02"));
	//if (bloodImpactFind.Succeeded())
	//	BloodImpact_vfx = bloodImpactFind.Object;
	//else
	//	UE_LOG(LogTemp, Error, TEXT("Can not find BloodImpact_vfx"));
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

#pragma region Menus

void UGI_TheLastBastion::ShowMainMenu()
{
	ShowMenu(mStartMenu_Widget, StartMenu_Class);
	UStartMenu* startMenu = Cast<UStartMenu>(mStartMenu_Widget);

	if (startMenu)
	{
		startMenu->UseFirstSlide();
	}

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

void UGI_TheLastBastion::RemoveLoadingScreen()
{
	if (mLoadingScreen_Widget)
	{
		mLoadingScreen_Widget->RemoveFromParent();
	}
}

void UGI_TheLastBastion::ShowMenu(UUserWidget* & _widget, const TSubclassOf<class UUserWidget>& _class, bool _showMouseCursor)
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
			_widget = CreateWidget <UUserWidget>(this, _class);
			if (_widget != nullptr)
			{
				_widget->AddToViewport();
			}
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Widget Class not Set - Game Instance"));
	}

	controller->bShowMouseCursor = _showMouseCursor;
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

void UGI_TheLastBastion::LaunchLobby(int _numOfConnections, const FString& _lobbyName)
{
	mNumOfConnection = _numOfConnections;
	mLobbyName = _lobbyName;
	if (DestroySession(true) == false)
	{
		HostSession(bIsLan, mNumOfConnection, mLobbyName);
	}
}

bool UGI_TheLastBastion::HostSession(bool _bIsLan, int _numOfConnections, const FString& _lobbyName)
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


			USaveGame_TheLastBastion* sg = this->LoadSaveGame();
			FString UserName;

			if (sg)
				UserName = sg->mPlayerProfile.mPlayerName.ToString();
			else
				UserName = TEXT("NewPlayer");

			sessionSettings.Set(TEXT("UserName"), UserName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
			sessionSettings.Set(TEXT("LobbyName"), _lobbyName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

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
				= mSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(mOnFindSessionsCompleteDelegate);

			UE_LOG(LogTemp, Warning, TEXT("Search for a LAN? %d"), bIsLan);
			mSessionSearch->bIsLanQuery = bIsLan;
			mSessionSearch->MaxSearchResults = 50;
			mSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
			mSessionInterface->FindSessions(*userId, mSessionSearch.ToSharedRef());
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("userId is not valid during Find Lobby"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineSubSystemCheck is failed during Host session"));
		OnSessionFindComplete(false);
	}
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
			HostSession(bIsLan, mNumOfConnection, mLobbyName);
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

USaveGame_TheLastBastion* UGI_TheLastBastion::LoadSaveGame() const
{
	bool bThereIsASavedProfile = UGameplayStatics::DoesSaveGameExist(playerSettingsSave, 0);

	if (bThereIsASavedProfile)
	{
		// Load profile
		USaveGame_TheLastBastion* saveGame = Cast<USaveGame_TheLastBastion>(UGameplayStatics::LoadGameFromSlot(playerSettingsSave, 0));
		if (saveGame == nullptr)
			return nullptr;
		else
			return saveGame;
	}
	else
		return nullptr;
}



