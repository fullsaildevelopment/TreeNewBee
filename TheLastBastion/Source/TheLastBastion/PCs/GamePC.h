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

	UFUNCTION(Client, Reliable)
		/** The first function to be called after get into the game world
		a. Save Game Check to get player character class, player name, avatar
		b. Ask a server to spawn a character for this client
		c. Setup In - Game HUD */
		void CLIENT_Login();


private:

	UFUNCTION(Server, Reliable, WithValidation)
		/** Called during login, update the profile to server
		and ask server to spawn a character */
		void SERVER_UploadProfileAndRequestCharacter();

	UFUNCTION(Client, Reliable)
		void CLIENT_CreateInGameHUD();

private:


	struct FPlayerProfile playerProfile;

	class UInGameHUD* mInGameHUD;

	class UGI_TheLastBastion* mGameInstanceRef;

private:

	void SaveGameCheck();

};
