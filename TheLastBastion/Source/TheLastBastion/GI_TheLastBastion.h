// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CustomType.h"
#include "Engine/GameInstance.h"

#include "OnlineSessionInterface.h"

#include "GI_TheLastBastion.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API UGI_TheLastBastion : public UGameInstance
{
	GENERATED_BODY()
	UGI_TheLastBastion(const FObjectInitializer & ObjectInitializer);

public:
	virtual void Init();


protected:

#pragma region Menus Widget Class and Reference

	UPROPERTY(EditDefaultsOnly, Category = MainMenuWidgetClass)
		TSubclassOf<class UUserWidget> StartMenu_Class;
	UPROPERTY()
		class UUserWidget* mStartMenu_Widget;

	UPROPERTY(EditDefaultsOnly, Category = MainMenuWidgetClass)
		TSubclassOf<class UUserWidget> LoadingScreen_Class;
	UPROPERTY()
		class UUserWidget* mLoadingScreen_Widget;

	UPROPERTY(EditDefaultsOnly, Category = MainMenuWidgetClass)
		TSubclassOf<class UUserWidget> CustomizeMenu_Class;
	UPROPERTY()
		class UUserWidget* mCustomizeMenu_Widget;

	UPROPERTY(EditDefaultsOnly, Category = MainMenuWidgetClass)
		TSubclassOf<class UUserWidget> HostMenu_Class;
	UPROPERTY()
		class UUserWidget* mHostMenu_Widget;

	UPROPERTY(EditDefaultsOnly, Category = MainMenuWidgetClass)
		TSubclassOf<class UUserWidget> JoinMenu_Class;
	UPROPERTY()
		class UUserWidget* mJoinMenu_Widget;

	UPROPERTY(EditDefaultsOnly, Category = MainMenuWidgetClass)
		TSubclassOf<class UUserWidget> LobbyRow_Class;

#pragma endregion

#pragma region All In-Game UI Class and Lobby Class

	UPROPERTY(EditDefaultsOnly, Category = GamePlayWidgetClass)
		TSubclassOf<class UUserWidget> InGameHUD_Class;

	UPROPERTY(EditDefaultsOnly, Category = GamePlayWidgetClass)
		TSubclassOf<class UUserWidget> InGamePlayerRow_Class;

	UPROPERTY(EditDefaultsOnly, Category = GamePlayWidgetClass)
		TSubclassOf<class UUserWidget> InGameTeamRow_Class;

	UPROPERTY(EditDefaultsOnly, Category = LobbyWidgetClass)
		TSubclassOf<class UUserWidget> LobbyMenu_Class;

	UPROPERTY(EditDefaultsOnly, Category = LobbyWidgetClass)
		TSubclassOf<class UUserWidget> LobbyPlayerRow_Class;

#pragma endregion

#pragma region All Character Class

	UPROPERTY(EditDefaultsOnly, Category = GamePlayCharacterClass)
		TSubclassOf<class ACharacter> Ranger_Class;

#pragma endregion


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName LobbyMap;

	UPROPERTY(BlueprintReadWrite)
		FPlayerProfile playerProflie;

	UPROPERTY()
		FString playerSettingsSave;

private:

#pragma region Session Settings

	class IOnlineSubsystem* mOnlineSubSystem;
	IOnlineSessionPtr mSessionInterface;
	TSharedPtr<class FOnlineSessionSearch> mSessionSearch;

	FOnCreateSessionCompleteDelegate mOnCreateSessionCompleteDelegate; 
	FDelegateHandle                  mOnCreateSessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate  mOnStartSessionCompleteDelegate;
	FDelegateHandle                  mOnStartSessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate  mOnFindSessionsCompleteDelegate;
	FDelegateHandle                  mOnFindSessionssCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate   mOnJoinSessionCompleteDelegate;
	FDelegateHandle                  mOnJoinSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate mOnDestroySessionCompleteDelegate;
	FDelegateHandle                   mOnDestroySessionCompleteDelegateHandle;

	/** The lobby name given by player during host menu*/
	FString mLobbyName;

	int  mNumOfConnection;


	bool bIsLan;
	bool bRecreateSession;

#pragma endregion

public:

	UFUNCTION(BlueprintPure)
		FORCEINLINE FText GetLobbyName() const { return FText::FromString(mLobbyName); }

	UFUNCTION(BlueprintPure)
		FORCEINLINE int GetMaxConnection() const { return mNumOfConnection; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE FString GetPlayerSettingsSaveFString() const {return playerSettingsSave;}

	UFUNCTION(BlueprintPure)
		FORCEINLINE TSubclassOf<UUserWidget> GetLobbyRow_Class() const { return LobbyRow_Class; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE TSubclassOf<UUserWidget> GetLobbyMenu_Class() const { return LobbyMenu_Class; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE TSubclassOf<UUserWidget> GetLobbyPlayerRow_Class() const { return LobbyPlayerRow_Class; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE TSubclassOf<UUserWidget> GetInGameHUD_Class() const { return InGameHUD_Class; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE TSubclassOf<UUserWidget> GetInGamePlayerRow_Class() const { return InGamePlayerRow_Class; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE TSubclassOf<UUserWidget> GetInGameTeamRow_Class() const { return InGameTeamRow_Class; }

	UFUNCTION(BlueprintPure)
		class USaveGame_TheLastBastion* GetSaveGame() const;

	void SetIsLan(bool _val);


public:

	UFUNCTION(BlueprintCallable)
		void SaveGameCheck();

#pragma region Show Menu Func

	UFUNCTION(BlueprintCallable)
		void ShowMainMenu();

	UFUNCTION(BlueprintCallable)
		void ShowCustomizeMenu();

	UFUNCTION(BlueprintCallable)
		void ShowHostMenu();

	UFUNCTION(BlueprintCallable)
		void ShowOptionMenu();

	UFUNCTION(BlueprintCallable)
		void ShowJoinMenu();

	UFUNCTION(BlueprintCallable)
		void DisplayLoadingScreen();

#pragma endregion

#pragma region Lobby && Server

	UFUNCTION(BlueprintCallable)
		void LaunchLobby(int _numOfConnections, const FString& _lobbyName);

	UFUNCTION()
		void FindLobby();

	UFUNCTION()
		void JoinServer(int _index);

	UFUNCTION(Exec)
		void BackToMainMenu();

#pragma endregion


private:

	// Helpers
	void ShowMenu(class UUserWidget* &_widget, const TSubclassOf<class UUserWidget>& _class, bool _showMouseCursor = true);

	bool OnlineSubSystemCheck();
	const TSharedPtr<const FUniqueNetId> GetUserId();

	/* return true, if there was a session with same name exist*/
	bool DestroySession(bool _recreate);
	bool HostSession(bool _bIsLan, int _numOfConnections, const FString& _lobbyName);		
	void OnStartOnlineGameComplete(FName _sessionName, bool _success);
	void OnSessionCreateComplete(FName _sessionName, bool _success);
	void OnSessionDestroyComplete(FName _sessionName, bool _success);
	void OnSessionJoinComplete(FName _sessionName, EOnJoinSessionCompleteResult::Type _result);
	void OnSessionFindComplete(bool _success);

	void LocateAllWidgetClass();
	void LocateAllCharacterClass();

};

