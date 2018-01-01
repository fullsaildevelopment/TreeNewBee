// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPC.generated.h"

/**
*
*/
UCLASS()
class THELASTBASTION_API ALobbyPC : public APlayerController
{
	GENERATED_BODY()

public:

	ALobbyPC(const FObjectInitializer& _objectInitializer);

public:

	UFUNCTION(Client, Reliable)
		void CLIENT_OnQuitLobby();

	UFUNCTION(Client, Reliable)
		/** Call after a client has joined, the server ask
		this client to make a save game check, and create LobbyUI*/
		void CLIENT_OnGameModeLogIn();

	UFUNCTION(Client, Reliable)
		/** Server ask client to Init those unchanged variables of the UI after it is creation */
		void CLIENT_InitLobbyUI(const FText& _lobbyName);


	UFUNCTION(Client, Reliable)
		/** Server ask each clients to update their player number display on their lobby UI */
		void CLIENT_UpdateNumOfPlayer(int _currentNumOfPlayers, int _maxNumOfPlayers);

	UFUNCTION(Client, Reliable)
		/** Remove lobby UI and Save profile*/
		void CLIENT_CleanAndSave();

	UFUNCTION(Client, Reliable)
		/** Tell client UI to populate all player rows for all connected players*/
		void CLIENT_AddPlayerToPlayerList(const TArray<FPlayerProfile>& _connectedPlayers);

	UFUNCTION(Server, Reliable, WithValidation)
		/** Tell server to add this clients playerProfile to server connected player list,
		and call server UpdateAllConnectedPlayers function*/
		void SERVER_UpdateAllConnectedPlayer(const FPlayerProfile & _playerProfile);

	UFUNCTION(Server, Reliable, WithValidation)
		/** Called when client click on the ready button, and server will update this clients
		player status to other clients */
		void SERVER_OnClientReadyButtonClicked();

	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_LeaveLobbyAndUpdateConnectedPlayer();

private:

	bool mAmReady;
	struct FPlayerProfile playerProfile;

private:

	/** Called during onGameModeLogin, check if this client has a vaild save game,
	if there is not create a temp holder to make sure we have something in player profile*/
	void SaveGameCheck();

	/** Called after save game Check, Create UI with lobby name specified by GameMode*/
	void CreateLobbyUI();

private:

	class UGI_TheLastBastion* mGameInstanceRef;
	class ULobbyMenu* mLobbyMenuRef;

public:

	FORCEINLINE bool GetAmReady() const { return mAmReady; }
	FORCEINLINE struct FPlayerProfile GetPlayerProfile() const { return playerProfile; }
};
