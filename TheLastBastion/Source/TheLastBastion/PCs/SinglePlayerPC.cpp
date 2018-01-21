// Fill out your copyright notice in the Description page of Project Settings.

#include "SinglePlayerPC.h"
#include "GameMode/SinglePlayerGM.h"
#include "GI_TheLastBastion.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/SaveGame_TheLastBastion.h"
#include "UI/InGameHUD.h"
#include "CustomType.h"



ASinglePlayerPC::ASinglePlayerPC(const FObjectInitializer & _objInit) : Super(_objInit)
{
	if (!InGameHuD_WBPClass)
		UCustomType::FindClass<UUserWidget>(InGameHuD_WBPClass, TEXT("/Game/UI/In-Game/WBP_InGameHUD"));

	if (!SmithShop_WBPClass)
		UCustomType::FindClass<UUserWidget>(SmithShop_WBPClass, TEXT("/Game/UI/In-Game/WBP_SmithShop"));

}

void ASinglePlayerPC::OnPostLogin()
{
	FInputModeGameOnly inputMode;
	SetInputMode(inputMode);
	SaveGameCheck();


	// ask server to spawn a hero class depend on player's previous choice, and ask this server version pc to poccess
	APawn* pawn = this->GetPawn();
	if (pawn)
		pawn->Destroy();

	pawn = nullptr;

	TArray<AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStarts);

	if (playerStarts.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Please a playerStart in GamePlay map,  AGamePlayGM::SpawnPlayer"))
			return;
	}

	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	
	if (gm)
	{
		FActorSpawnParameters spawnParam;
		spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ATheLastBastionHeroCharacter* hero
			= GetWorld()->SpawnActor<ATheLastBastionHeroCharacter>(gm->DefaultPawnClass, playerStarts[0]->GetTransform(), spawnParam);
		this->Possess(hero);
	}

	CreateInGameHUD();
}

void ASinglePlayerPC::OnFinishSeamlessTravel()
{
	OnPostLogin();
}

void ASinglePlayerPC::InitUIOnBeginPlay(const UHeroStatsComponent * _heroStats)
{
	if (mInGameHUD)
	{
		mInGameHUD->InitStats(_heroStats);

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("mInGameHUD is NULL - AGamePC::CLIENT_InitUI_Implementation"));
	}
}

void ASinglePlayerPC::OnHealthChange(const UPawnStatsComponent * _heroStats)
{
	if (mInGameHUD)
		mInGameHUD->SetHpOnHealthChange(_heroStats);
}


void ASinglePlayerPC::OpenSmithShop()
{
	if (mSmithShopHUD != nullptr)
		mSmithShopHUD->AddToViewport();
	else
	{
		if (SmithShop_WBPClass)
		{
			mSmithShopHUD = CreateWidget <UUserWidget>(this, SmithShop_WBPClass);
			if (mSmithShopHUD != nullptr)
			{
				mSmithShopHUD->AddToViewport();
			}
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Widget Class not Set - ASinglePlayerPC::ShowSmithShop"));
	}
	bShowMouseCursor = true;
}

void ASinglePlayerPC::CloseSmithShop()
{
	if (mSmithShopHUD)
	{
		mSmithShopHUD->RemoveFromParent();
		bShowMouseCursor = false;
	}
}




void ASinglePlayerPC::SaveGameCheck()
{
	UGI_TheLastBastion* game_gi = Cast<UGI_TheLastBastion>(GetGameInstance());
	if (game_gi == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("game instance needs to be UGI_TheLastBastion ---  ALobbyGM::PostLogin"));
		return;
	}

	USaveGame_TheLastBastion* saveGame = game_gi->LoadSaveGame();

	// check again if we have a save game
	if (saveGame == nullptr)
	{
		// if we dont have a save game create a new one
		saveGame = Cast<USaveGame_TheLastBastion>(UGameplayStatics::CreateSaveGameObject(USaveGame_TheLastBastion::StaticClass()));
		saveGame->UseDefaultProfile();
		UGameplayStatics::SaveGameToSlot(saveGame, game_gi->GetPlayerSettingsSaveFString(), 0);
	}

	playerProfile = saveGame->mPlayerProfile;
	saveGame->LogOutProfile();
}

void ASinglePlayerPC::CreateInGameHUD()
{
	mInGameHUD = Cast<UInGameHUD>(CreateWidget<UUserWidget>(this, InGameHuD_WBPClass));

	playerProfile.mCharacterImage = USaveGame_TheLastBastion::GetRangerAvatarImage();
	playerProfile.bIsRangerClass = true;

	mInGameHUD->LoadPlayerProfile_Sp(playerProfile);
	mInGameHUD->AddToViewport();
}
