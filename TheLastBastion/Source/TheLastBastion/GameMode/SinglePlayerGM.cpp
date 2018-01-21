// Fill out your copyright notice in the Description page of Project Settings.

#include "SinglePlayerGM.h"
#include "PCs/SinglePlayerPC.h"
#include "CustomType.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/SpawnLocation.h"


ASinglePlayerGM::ASinglePlayerGM(const FObjectInitializer & _objectInitilizer) : Super(_objectInitilizer)
{
	PlayerControllerClass = ASinglePlayerPC::StaticClass();
	UCustomType::FindClass<APawn>(DefaultPawnClass, TEXT("/Game/Blueprints/Heros/SinglePlayer_Bp"));
	
	GetAllCharacterClass();
	EnemyAmount = 0;
}

void ASinglePlayerGM::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(NewPlayer);

	pc->OnPostLogin();

}

void ASinglePlayerGM::HandleSeamlessTravelPlayer(AController *& C)
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
		ASinglePlayerPC* spPC = Cast<ASinglePlayerPC>(PC);
		if (spPC)
		{
			spPC->OnFinishSeamlessTravel();
		}
	}

}

void ASinglePlayerGM::BeginPlay()
{
	TArray<AActor*> AllSpawnLocations;
	ASpawnLocation* spawner = nullptr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnLocation::StaticClass(), AllSpawnLocations);
	for (size_t i = 0; i < AllSpawnLocations.Num(); i++)
	{
		spawner = Cast<ASpawnLocation>(AllSpawnLocations[i]);
		if (spawner)
		{
			switch (spawner->GetSpawnLocationType())
			{
			case ESpawnWorldLocation::Lannester_S1:
			{   
				LannesterSpawnLocations_One.Add(spawner);
				break;
			}

			case ESpawnWorldLocation::Lannester_S2:
			{
				//LannesterSpawnLocations_One.Add(spawner);
				break;
			}

			case ESpawnWorldLocation::Lannester_S3:
			{
				//LannesterSpawnLocations_One.Add(spawner);
				break;
			}

			case ESpawnWorldLocation::WhiteWalker_S1:
			{
				//LannesterSpawnLocations_One.Add(spawner);
				break;
			}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("%s"), *spawner->GetName());

	}

	UE_LOG(LogTemp, Log, TEXT("%d"), LannesterSpawnLocations_One.Num());

}

void ASinglePlayerGM::GetAllCharacterClass()
{

	if (LannesterTrooper_T0_BP == nullptr)
		UCustomType::FindClass<ATheLastBastionCharacter>(LannesterTrooper_T0_BP, TEXT("/Game/Blueprints/AI/Lannester/Lan_Trooper_T0"));

	if (LannesterShooter_T0_BP == nullptr)
		UCustomType::FindClass<ATheLastBastionCharacter>(LannesterShooter_T0_BP, TEXT("/Game/Blueprints/AI/Lannester/Lan_Crossbow_T0"));

}

void ASinglePlayerGM::UpdateEnemyAmount(int _val)
{
	EnemyAmount += _val;
}
