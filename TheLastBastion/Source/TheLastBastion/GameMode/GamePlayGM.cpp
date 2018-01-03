// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePlayGM.h"
#include "CustomType.h"
#include "PCs/LobbyPC.h"
#include "PCs/GamePC.h"
#include "GI_TheLastBastion.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h "
#include "UI/InGameHUD.h"
#include "TheLastBastionHeroCharacter.h"


AGamePlayGM::AGamePlayGM(const FObjectInitializer & _objectInitilizer) : Super(_objectInitilizer)
{
	PlayerControllerClass = AGamePC::StaticClass();
}

void AGamePlayGM::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (HasAuthority())
	{
		AGamePC* newPC = Cast<AGamePC>(NewPlayer);
		if (newPC == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("pc is not a AGamePC -- AGamePlayGM::PostLogin"));
			return;
		}

		FMatchPlayer newMatchPlayer;
		newMatchPlayer.controller = newPC;
		AllPlayers.Add(newMatchPlayer);

		//AllPlayerControllers.Add(newPC);
		UGameInstance* gi = GetGameInstance();
		if (gi == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("can get game instance  ---  ALobbyGM::PostLogin"));
			return;
		}
		UGI_TheLastBastion* game_gi = Cast<UGI_TheLastBastion>(gi);
		if (game_gi == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("game instance needs to be UGI_TheLastBastion ---  ALobbyGM::PostLogin"));
			return;
		}

		NumOfPlayers = game_gi->GetMaxConnection();

		// pass the which controller this is in the array
		newPC->CLIENT_Login(AllPlayers.Num() - 1);	
	}
}

void AGamePlayGM::HandleSeamlessTravelPlayer(AController *& C)
{
	// Default behavior is to spawn new controllers and copy data
	APlayerController* PC = Cast<APlayerController>(C);
	if (PC && PC->Player)
	{
		// We need to spawn a new PlayerController to replace the old one
		APlayerController* NewPC = SpawnPlayerController(PC->IsLocalPlayerController() ? ROLE_SimulatedProxy : ROLE_AutonomousProxy, PC->GetFocalLocation(), PC->GetControlRotation());

		if (NewPC)
		{
			PC->SeamlessTravelTo(NewPC);
			NewPC->SeamlessTravelFrom(PC);
			SwapPlayerControllers(PC, NewPC);
			PC = NewPC;
			C = NewPC;
		}
		else
		{
			UE_LOG(LogGameMode, Warning, TEXT("HandleSeamlessTravelPlayer: Failed to spawn new PlayerController for %s (old class %s)"), *PC->GetHumanReadableName(), *PC->GetClass()->GetName());
			PC->Destroy();
			return;
		}
	}

	InitSeamlessTravelPlayer(C);

	// Initialize hud and other player details, shared with PostLogin
	GenericPlayerInitialization(C);

	if (PC)
	{
		// This may spawn the player pawn if the game is in progress
		AGamePC* gamePC = Cast<AGamePC>(PC);
		if (gamePC)
		{
			FMatchPlayer newMatchPlayer;
			newMatchPlayer.controller = gamePC;
			AllPlayers.Add(newMatchPlayer);
			gamePC->CLIENT_Login(AllPlayers.Num() - 1);
		}
	}

}

void AGamePlayGM::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
	UE_LOG(LogTemp, Warning,
		TEXT("PostSeamlessTravel"));


	UGameInstance* gi = GetGameInstance();
	if (gi == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("can get game instance  ---  ALobbyGM::PostLogin"));
		return;
	}
	UGI_TheLastBastion* game_gi = Cast<UGI_TheLastBastion>(gi);
	if (game_gi == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("game instance needs to be UGI_TheLastBastion ---  ALobbyGM::PostLogin"));
		return;
	}

	NumOfPlayers = game_gi->GetMaxConnection();

	//for (TActorIterator<APlayerController> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	//{
	//	APlayerController* pc = Cast<APlayerController>(*ActorItr);
	//	ALobbyPC* lobbyPC = Cast<ALobbyPC>(pc);
	//	if (lobbyPC)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Not a GamePC, but a Lobby PC"));
	//	}
	//	AGamePC* gamePC = Cast<AGamePC>(pc);
	//	if (gamePC)
	//	{
	//		FMatchPlayer newMatchPlayer;
	//		newMatchPlayer.controller = gamePC;
	//		AllPlayers.Add(newMatchPlayer);
	//		gamePC->CLIENT_Login(AllPlayers.Num() - 1);
	//	}
	//}
}

void AGamePlayGM::BeginPlay()
{

}

void AGamePlayGM::GrabProfileAndSpawnPlayer(const FPlayerProfile & _profile, int _index)
{
	// Upload the profile
	AllPlayers[_index].profile = _profile;

	// ask server to spawn a hero class depend on player's previous choice, and ask this server version pc to poccess
	APawn* pawn = AllPlayers[_index].controller->GetPawn();
	if (pawn)
		pawn->Destroy();

	pawn = nullptr;

	TArray<AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStarts);

	if (playerStarts.Num() != 4)
	{
		UE_LOG(LogTemp, Error, TEXT("Please put 4 playerStart in GamePlay map,  AGamePlayGM::SpawnPlayer"))
			return;
	}

	//pawn = GetWorld()->SpawnActor<APawn>(_profile.mCharacterClass, playerStarts[_index]->GetTransform());
	ATheLastBastionHeroCharacter* hero
	 = GetWorld()->SpawnActor<ATheLastBastionHeroCharacter>(_profile.mCharacterClass, playerStarts[_index]->GetTransform());
	
	AllPlayers[_index].controller->Possess(hero);

	AllPlayers[_index].character = hero;
}

void AGamePlayGM::UpdateAllConnectedPlayers(int _index)
{

	//// clear out all Profile
	//AllConnectedPlayers.Empty();

	//AGamePC* gamePC = nullptr;
	//for (int iCtrl = 0; iCtrl < AllPlayerControllers.Num(); iCtrl++)
	//{
	//	// gather player profiles from controller
	//	gamePC = AllPlayerControllers[iCtrl];

	//	AllConnectedPlayers.Add(gamePC->GetPlayerProfile());
	//}

	//// then put all connect clients' info on each clients UI
	//for (int iCtrl = 0; iCtrl < AllPlayerControllers.Num(); iCtrl++)
	//{
	//	gamePC = AllPlayerControllers[iCtrl];
	//	gamePC ->CLIENT_AddPlayerToPlayerList(AllConnectedPlayers, _index);
	//}
}
