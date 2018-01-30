// Fill out your copyright notice in the Description page of Project Settings.

#include "AIGroupBase.h"
#include "Components/BoxComponent.h"
#include "TheLastBastionCharacter.h"


#define SIDEPADDING 200.0f
// Sets default values
AAIGroupBase::AAIGroupBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	//
	float halfHeight = 100.0f;

	GroupVolumn = CreateDefaultSubobject<UBoxComponent>(TEXT("GroupSpawnVolumn"));
	GroupVolumn->SetupAttachment(RootComponent);
	GroupVolumn->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GroupVolumn->bGenerateOverlapEvents = false;
	GroupVolumn->SetCanEverAffectNavigation(false);
	GroupVolumn->InitBoxExtent(FVector(halfHeight, halfHeight, halfHeight));
	GroupVolumn->RelativeLocation = FVector(0, 0, halfHeight);

	//
	bActivated = true;

}

// Called when the game starts or when spawned
void AAIGroupBase::BeginPlay()
{
	Super::BeginPlay();

	SpawnAGroup();
}

void AAIGroupBase::SpawnAGroup()
{

	if (bActivated)
	{

		if (AIToSpawn.Num()>0)
		{
			UWorld* const world = GetWorld();

			if (world)
			{
				FActorSpawnParameters spawnParam;
				spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				// track max width for trigger volumn bounding calculation
				float maxWidth = -1.0f;
				float maxLength = 0.0f;
				// offset on forward direction
				float xOffset = 0.0f;

				for (int iClass = 0; iClass < AIToSpawn.Num(); iClass++)
				{
					TSubclassOf<ATheLastBastionCharacter> ClassToSpawn = AIToSpawn[iClass].AIClassBP;

					int Remain = AIToSpawn[iClass].TotalNumber;
					int maxRow = AIToSpawn[iClass].NumOfRow;
					float colPadding = AIToSpawn[iClass].ColumnPadding;
					float rowPadding = AIToSpawn[iClass].RowPadding;

					float maxRowWidth = -1.0f;//(maxAIRowAICount - 1) * colPadding;
					maxLength += rowPadding * (maxRow - 1);
					for (int iRow = 0; iRow < maxRow; iRow++)
					{
						// calculate how many ai in current row
						int AICountInThisRow = Remain / (maxRow - iRow);
						float rowWidth = (AICountInThisRow - 1) * colPadding;
						if (rowWidth > maxRowWidth)
							maxRowWidth = rowWidth;

						Remain -= AICountInThisRow;
						float centerOffset = 0.5f * rowWidth;
						UE_LOG(LogTemp, Log, TEXT("AICount in this Row %d"), AICountInThisRow);
						UE_LOG(LogTemp, Log, TEXT("center offset %f"), centerOffset);

						// calculate the relative x offset base on center root
						for (int i = 0; i < AICountInThisRow; i++)
						{
							// offset on right vector direction
							float yOffset = i * colPadding - centerOffset;
							FAICharacterInfo newCharacterInfo;

							FVector myLocation = this->GetActorLocation();
							newCharacterInfo.GroupRelativeLocation = FVector(xOffset, yOffset, 0.0f);
							FVector spawnLocation = myLocation - xOffset * GetActorForwardVector() + yOffset * GetActorRightVector();
							newCharacterInfo.AICharacter = world->SpawnActor<ATheLastBastionCharacter>(ClassToSpawn, spawnLocation, this->GetActorRotation(), spawnParam);
							newCharacterInfo.AICharacter->SpawnDefaultController();
							AICharactersInfo.Add(newCharacterInfo);

							UE_LOG(LogTemp, Log, TEXT("xOffset %f"), xOffset);
						}

						xOffset += rowPadding;
					}


					UE_LOG(LogTemp, Log, TEXT("maxRowWidth in this Row %f"), maxRowWidth);

					if (maxRowWidth > maxWidth)
						maxWidth = maxRowWidth;
				}
				maxLength *= 2.0f;
				// extend take half width, add padding on the side
				maxWidth = 0.5f * maxWidth + SIDEPADDING;
				
				UE_LOG(LogTemp, Log, TEXT("maxWidth in this group %f"), maxWidth);
				GroupVolumn->SetBoxExtent(FVector(maxLength, maxWidth, GroupVolumn->GetUnscaledBoxExtent().Z), true);
			}

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s NO AI to Spawn - AAIGroupBase::SpawnAGroup"), *this->GetName());
		}
	}

	//if (AIClassBP != nullptr && bActivated)
	//{
	//	UWorld* const world = GetWorld();
	//	if (world)
	//	{
	//		SetGroupSpawingLocations();
	//		//FActorSpawnParameters spawnParam;
	//		//spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//		//for (int32 i = 0; i < NumOfAICharacters; i++)
	//		//{
	//		//	ATheLastBastionCharacter* SingleAIPawn = world->SpawnActor<ATheLastBastionCharacter>(AIClassBP, GroupSpawningLocations[i], FRotator::ZeroRotator, spawnParam);
	//		//	SingleAIPawn->SpawnDefaultController();
	//		//	AICharacters.Add(SingleAIPawn);
	//		}
	//	}
	//}
}

void AAIGroupBase::SetGroupSpawingLocations()
{
	//FVector BoxSize = BoxComp->GetScaledBoxExtent();
	//float DistanceBetweenEachPoint = BoxSize.Y / NumOfAICharacters;
	//float BoxWidth = BoxSize.X;
	//for (int32 i = 0; i < NumOfAICharacters; i++)
	//{
	//	FVector CurrentSpawnLocation = FVector(BoxWidth * 0.5f, 0.0f, 0.0f);
	//	CurrentSpawnLocation.Y += DistanceBetweenEachPoint * i;
	//	GroupSpawningLocations.Add(CurrentSpawnLocation);
	//}
}

// Called every frame
void AAIGroupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAIGroupBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

