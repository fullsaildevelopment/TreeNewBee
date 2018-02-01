// Fill out your copyright notice in the Description page of Project Settings.

#include "AIGroupBase.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

#include "TheLastBastionCharacter.h"
#include "AI/TheLastBastionGroupAIController.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"


#include "Kismet/GameplayStatics.h"
#include "TheLastBastionHeroCharacter.h"


#define SIDEPADDING 200.0f
// Sets default values
AAIGroupBase::AAIGroupBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	//RootComponent = RootComp;

	//
	float halfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	GroupVolumn = CreateDefaultSubobject<UBoxComponent>(TEXT("GroupSpawnVolumn"));
	GroupVolumn->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GroupVolumn->bGenerateOverlapEvents = false;
	GroupVolumn->SetCanEverAffectNavigation(false);
	GroupVolumn->InitBoxExtent(FVector(halfHeight, halfHeight, halfHeight));

	GroupVolumn->SetupAttachment(GetCapsuleComponent());

	//moveComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("MovementComp"));

	AIControllerClass = ATheLastBastionGroupAIController::StaticClass();

	//
	bActivated = true;

}

// Called when the game starts or when spawned
void AAIGroupBase::BeginPlay()
{
	Super::BeginPlay();

	SpawnAGroup();

	ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (hero)
		hero->CommandedGroup = this;
	else
		UE_LOG(LogTemp, Error, TEXT("Can find hero - AAIGroupBase::BeginPlay"));

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
						//UE_LOG(LogTemp, Log, TEXT("AICount in this Row %d"), AICountInThisRow);
						//UE_LOG(LogTemp, Log, TEXT("center offset %f"), centerOffset);

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

							//UE_LOG(LogTemp, Log, TEXT("xOffset %f"), xOffset);
						}

						xOffset += rowPadding;
					}


					//UE_LOG(LogTemp, Log, TEXT("maxRowWidth in this Row %f"), maxRowWidth);

					if (maxRowWidth > maxWidth)
						maxWidth = maxRowWidth;
				}
				maxLength *= 2.0f;
				// extend take half width, add padding on the side
				maxWidth = 0.5f * maxWidth + SIDEPADDING;
				
				//UE_LOG(LogTemp, Log, TEXT("maxWidth in this group %f"), maxWidth);
				GroupVolumn->SetBoxExtent(FVector(maxLength, maxWidth, GroupVolumn->GetUnscaledBoxExtent().Z), true);
			}

		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("%s NO AI to Spawn - AAIGroupBase::SpawnAGroup"), *this->GetName());
		}
	}
}


// Called every frame
void AAIGroupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void AAIGroupBase::SetChildPathLocation()
{
	FVector forward = GetActorForwardVector();
	FVector right = GetActorRightVector();
	FVector childtargetLocation;
	FVector locationOffset;

	UBlackboardComponent* bbcGroup = nullptr;
	UBlackboardComponent* bbcChild = nullptr;

	ATheLastBastionBaseAIController* enemyC = nullptr;

	ATheLastBastionGroupAIController* groupC = Cast<ATheLastBastionGroupAIController>(GetController());
	bbcGroup = groupC->GetBlackboardComponent();
	if (bbcGroup == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT(" groupC bbc == nullptr - AAIGroupBase::SetChildPathLocation"));
		return;
	}

	FVector targetLocation = bbcGroup->GetValue<UBlackboardKeyType_Vector>(groupC->GetKeyID_TargetLocation());

	//FVector targetFwd = (targetLocation - GetActorLocation()).GetSafeNormal();
	//FVector targetRight = 



	for (int i = 0; i < AICharactersInfo.Num(); i++)
	{
		enemyC = Cast<ATheLastBastionBaseAIController>(AICharactersInfo[i].AICharacter->GetController());
		if (enemyC == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("enemyC == nullptr - AAIGroupBase::SetChildPathLocation"));
			return;
		}

		bbcChild = enemyC->GetBlackboardComponent();
		if (bbcChild == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("bbc == nullptr - AAIGroupBase::SetChildPathLocation"));
			return;
		}

		locationOffset = AICharactersInfo[i].GroupRelativeLocation;
		childtargetLocation = targetLocation - forward * locationOffset.X + right * locationOffset.Y;
		bbcChild->SetValue<UBlackboardKeyType_Vector>(enemyC->GetKeyID_TargetLocation(), childtargetLocation);
		bbcChild->SetValue<UBlackboardKeyType_Int>(enemyC->GetKeyID_NewCommandIndex(), 0);

	}

	// Clear the command
	bbcGroup->SetValue<UBlackboardKeyType_Int>(groupC->GetKeyID_NewCommandIndex(), 0);
}

void AAIGroupBase::SetMarchLocation(const FVector & _location)
{
	ATheLastBastionGroupAIController* groupC = Cast<ATheLastBastionGroupAIController>(GetController());
	if (groupC == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("groupC == nullptr - AAIGroupBase::SetMarchLocation"));
		return;
	}

	UBlackboardComponent* bbc = groupC->GetBlackboardComponent();
	if (bbc == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("bbc == nullptr - AAIGroupBase::SetMarchLocation"));
		return;
	}

	// Set new command index to 1
	// Set Target location

	bbc->SetValue<UBlackboardKeyType_Int>(groupC->GetKeyID_NewCommandIndex(), 1);
	bbc->SetValue<UBlackboardKeyType_Vector>(groupC->GetKeyID_TargetLocation(), _location);

	// Set childTarget location
	ATheLastBastionBaseAIController* enemyC = nullptr;
	UBlackboardComponent* bbcChild = nullptr;


	for (int i = 0; i < AICharactersInfo.Num(); i++)
	{
		enemyC = Cast<ATheLastBastionBaseAIController>(AICharactersInfo[i].AICharacter->GetController());
		if (enemyC == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("enemyC == nullptr - AAIGroupBase::SetMarchLocation"));
			return;
		}

		bbcChild = enemyC->GetBlackboardComponent();
		if (bbcChild == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("bbc == nullptr - AAIGroupBase::SetMarchLocation"));
			return;
		}

		bbcChild->SetValue<UBlackboardKeyType_Int>(enemyC->GetKeyID_NewCommandIndex(), 1);
	}



}

// Called to bind functionality to input
//void AAIGroupBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}



