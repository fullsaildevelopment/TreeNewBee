// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePlayGM.h"
#include "CustomType.h"
#include "PCs/GamePC.h"
#include "GI_TheLastBastion.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h "


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

		AllPlayerControllers.Add(newPC);


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


		newPC->CLIENT_Login();
	}
}

void AGamePlayGM::SpawnPlayer(APlayerController * _pc)
{
	APawn* pawn = _pc->GetPawn();
	if (pawn)
		pawn->Destroy();

	pawn = nullptr;


	// Get Class from client
	if (DefaultPawnClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultPawnClass is NULL,  ALobbyGM::SpawnPlayer"))
	}

	TArray<AActor*> playerStarts;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStarts);


	if (playerStarts.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Please put a playerStart in Lobby map,  ALobbyGM::SpawnPlayer"))
			return;
	}
	pawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, playerStarts[0]->GetTransform());
	_pc->Possess(pawn);

}
