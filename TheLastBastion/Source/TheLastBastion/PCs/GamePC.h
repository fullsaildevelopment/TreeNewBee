// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GamePC.generated.h"

/**
*
*/
UCLASS()
class THELASTBASTION_API AGamePC : public APlayerController
{
	GENERATED_BODY()


public:

	//AGamePC(const FObjectInitializer & _objInit);

	UFUNCTION(Client, Reliable)
		/** The first function to be called after get into the game world
		a. Save Game Check to get player character class, player name, avatar
		b. Ask a server to spawn a character for this client
		c. Setup In - Game HUD */
		void CLIENT_Login(int _index);


	UFUNCTION(Client, Reliable)
		void CLIENT_AddPlayerToPlayerList(const TArray<FPlayerProfile>& _allConnectedPlayers,
			int _index);


private:

	UFUNCTION(Server, Reliable, WithValidation)
		/** Called during login, update the profile to server
		and ask server to spawn a character */
		void SERVER_UploadProfileAndRequestCharacter
		(const FPlayerProfile& _profile, int _index);
			

	UFUNCTION()
		void CreateInGameHUD();

	/** Copy the player profile from local to client version of player controller*/
	void SaveGameCheck();

private:


	struct FPlayerProfile              playerProfile;
	UPROPERTY()
	class UInGameHUD*                  mInGameHUD;
	UPROPERTY()
	class UGI_TheLastBastion*          mGameInstanceRef;

public:

	FORCEINLINE FPlayerProfile GetPlayerProfile() const { return playerProfile; }


};
