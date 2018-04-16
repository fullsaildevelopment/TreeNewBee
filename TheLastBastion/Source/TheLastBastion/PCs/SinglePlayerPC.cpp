// Fill out your copyright notice in the Description page of Project Settings.

#include "SinglePlayerPC.h"
#include "GameMode/SinglePlayerGM.h"
#include "GI_TheLastBastion.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/SaveGame_TheLastBastion.h"
#include "UI/InGameHUD.h"
#include "UI/Gameplay/TradeMenu.h"
#include "UI/Gameplay/InGameMenu.h"
#include "UI/Gameplay/RecruitMenu.h"


#include "TheLastBastionHeroCharacter.h"
#include "Combat/HeroStatsComponent.h"

#include "CustomType.h"


// temp
#include "AI/AllyGroup.h"
#include "DrawDebugHelpers.h"


ASinglePlayerPC::ASinglePlayerPC(const FObjectInitializer & _objInit) : Super(_objInit)
{
	if (!InGameHuD_WBPClass)
		UCustomType::FindClass<UUserWidget>(InGameHuD_WBPClass, TEXT("/Game/UI/In-Game/WBP_InGameHUD"));

	if (!TradeMenu_WBPClass)
		UCustomType::FindClass<UUserWidget>(TradeMenu_WBPClass, TEXT("/Game/UI/In-Game/TradeMenu/WBP_TradeMenu"));

	if (!InGameMenu_WBPClass)
		UCustomType::FindClass<UUserWidget>(InGameMenu_WBPClass, TEXT("/Game/UI/In-Game/WBP_InGameMenu"));

	if (!RecruitMenu_WBPClass)
		UCustomType::FindClass<UUserWidget>(RecruitMenu_WBPClass, TEXT("/Game/UI/In-Game/Recruit/WBP_RecruitMenu"));
}

void ASinglePlayerPC::OnPostLogin()
{
	FInputModeGameOnly inputMode;
	SetInputMode(inputMode);
	SaveGameCheck();

	// Turn off loading screen

	UGI_TheLastBastion* gi = Cast<UGI_TheLastBastion>(GetGameInstance());
	if (gi)
	{
		gi->RemoveLoadingScreen();
	}

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

void ASinglePlayerPC::OnPauseButtonIsPressed()
{
	OpenInGameMenu();
}

void ASinglePlayerPC::OpenInGameMenu()
{
	if (mInGameMenu)
	{
		mInGameMenu->AddToViewport();
	}
	else
	{
		if (InGameMenu_WBPClass)
		{
			mInGameMenu = CreateWidget <UInGameMenu>(this, InGameMenu_WBPClass);
			if (mInGameMenu != nullptr)
			{
				mInGameMenu->AddToViewport();
				// set input mode
			}
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Widget Class not Set - ASinglePlayerPC::OpenInGameMenu"));

	}

	if (mInGameMenu)
	{
		SetInputModeForOpenMenu(mInGameMenu);
		mInGameMenu->OpenPauseMenu();
		this->SetPause(true);
	}

}

void ASinglePlayerPC::SetInputModeForOpenMenu(class UUserWidget* _focus)
{
	bShowMouseCursor = true;
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(_focus->TakeWidget());
	SetInputMode(InputMode);
}

void ASinglePlayerPC::OnHealthChange(const UPawnStatsComponent * _heroStats)
{
	if (mInGameHUD)
		mInGameHUD->SetHpOnHealthChange(_heroStats);
}

void ASinglePlayerPC::OpenTradeMenu()
{
	if (mTradeMenu != nullptr)
		mTradeMenu->AddToViewport();
	else
	{
		if (TradeMenu_WBPClass)
		{
			mTradeMenu = CreateWidget <UTradeMenu>(this, TradeMenu_WBPClass);
			if (mTradeMenu != nullptr)
			{
				mTradeMenu->AddToViewport();
			}
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Widget Class not Set - ASinglePlayerPC::OpenTradeMenu"));
	}


	if (mTradeMenu)
	{
		//set input mode
		//bShowMouseCursor = true;
		//FInputModeUIOnly InputMode;
		//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		//InputMode.SetWidgetToFocus(mTradeMenu->TakeWidget());
		//SetInputMode(InputMode);

		SetInputModeForOpenMenu(mTradeMenu);

		ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(GetCharacter());
		mTradeMenu->OnOpenTradeMenu(hero->GetHeroStatsComp());
	}
}

void ASinglePlayerPC::OpenRecruitMenu()
{
	if (mRecruitMenu)
	{
		mRecruitMenu->AddToViewport();
	}
	else
	{
		if (RecruitMenu_WBPClass)
		{
			mRecruitMenu = CreateWidget <URecruitMenu>(this, RecruitMenu_WBPClass);
			if (mRecruitMenu != nullptr)
			{
				mRecruitMenu->AddToViewport();
				// set input mode
			}
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Widget Class not Set - ASinglePlayerPC::OpenInGameMenu"));

	}

	if (mRecruitMenu)
	{
		SetInputModeForOpenMenu(mRecruitMenu);
		mRecruitMenu->OnOpenRecruitMenu();
	}

}

void ASinglePlayerPC::LevelUp(int _deltaLevel)
{
	ATheLastBastionHeroCharacter* heroCharacter = Cast<ATheLastBastionHeroCharacter>(GetCharacter());
	if (heroCharacter)
	{
		UHeroStatsComponent* HeroStats = heroCharacter->GetHeroStatsComp();
		
		if (HeroStats)
		{
			for (int i = 0; i < _deltaLevel; i++)
				HeroStats->LevelUp();

			mInGameHUD->SetPlayerRowOnLevelUp(HeroStats);
		}
	}

}

void ASinglePlayerPC::CloseTradeMenu()
{
	if (mTradeMenu)
	{
		mTradeMenu->RemoveFromParent();
		bShowMouseCursor = false;
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
}

void ASinglePlayerPC::ShowFrontLine()
{
	ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(GetCharacter());
	if (hero)
	{
		TArray<ATheLastBastionAIBase*> frontLine = hero->GetCommandGroup()->GetFrontLine();
		for (int iChar = 0; iChar < frontLine.Num(); iChar++)
		{
			DrawDebugSphere(GetWorld(), frontLine[iChar]->GetActorLocation(), 50.0f, 8, FColor::Red, false, 6.0f);
		}
	}
}

void ASinglePlayerPC::ShowColumnAt(int _index)
{
	ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(GetCharacter());
	if (hero)
	{
		TArray<ATheLastBastionAIBase*> frontLine = hero->GetCommandGroup()->GetColumnAt(_index);
		for (int iChar = 0; iChar < frontLine.Num(); iChar++)
		{
			DrawDebugSphere(GetWorld(), frontLine[iChar]->GetActorLocation(), 50.0f, 8, FColor::Red, false, 6.0f);
		}
	}

}

void ASinglePlayerPC::ForTheNightIsDarkAndFullOfTerrors(int _val)
{
	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm)
	{
		gm->AddFood(_val);
		gm->AddMetal(_val);
		gm->AddStone(_val);
		gm->AddWood(_val);
	}
}

void ASinglePlayerPC::OnSelectedCrewAt(int _index)
{
	mInGameHUD->OnSelectedCrewAt(_index);
}

void ASinglePlayerPC::OnTradeMenuAccept(UHeroStatsComponent * _heroStats)
{
	mInGameHUD->ResetStats(_heroStats);
}

UInventoryUI * ASinglePlayerPC::GetInventoryUI() const
{
	return mTradeMenu->GetInventoryUI();
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


