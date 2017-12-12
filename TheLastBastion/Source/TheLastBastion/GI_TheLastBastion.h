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

#pragma region Menus

	// Main Menu
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> StartMenu_Class;
	UPROPERTY()
		class UUserWidget* mStartMenu_Widget;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> LoadingScreen_Class;
	UPROPERTY()
		class UUserWidget* mLoadingScreen_Widget;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> CustomizeMenu_Class;
	UPROPERTY()
		class UUserWidget* mCustomizeMenu_Widget;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> HostMenu_Class;
	UPROPERTY()
		class UUserWidget* mHostMenu_Widget;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> JoinMenu_Class;
	UPROPERTY()
		class UUserWidget* mJoinMenu_Widget;

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


	int  mNumOfConnection;
	bool bIsLan;
	bool bRecreateSession;

#pragma endregion




public:

	UFUNCTION(BlueprintPure)
		FString GetPlayerSettingsSave() const;

	void SetIsLan(bool _val);

public:

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
		void LaunchLobby(int _numOfConnections);

	UFUNCTION()
		void FindLobby();

	UFUNCTION()
		void JoinServer(int _index);

	UFUNCTION(Exec)
		void BackToMainMenu();

#pragma endregion

	UFUNCTION(BlueprintCallable)
		void SaveGameCheck();


private:

	// Helpers
	void ShowMenu(class UUserWidget* &_widget, const TSubclassOf<class UUserWidget>& _class);
	bool OnlineSubSystemCheck();
	const TSharedPtr<const FUniqueNetId> GetUserId();

	bool HostSession(bool _bIsLan, int _numOfConnections);
	
	
	
	/* return true, if there was a session with same name exist*/
	bool DestroySession(bool _recreate); 



	void OnStartOnlineGameComplete(FName _sessionName, bool _success);
	void OnSessionCreateComplete(FName _sessionName, bool _success);
	void OnSessionDestroyComplete(FName _sessionName, bool _success);
	void OnSessionJoinComplete(FName _sessionName, EOnJoinSessionCompleteResult::Type _result);
	void OnSessionFindComplete(bool _success);
};

