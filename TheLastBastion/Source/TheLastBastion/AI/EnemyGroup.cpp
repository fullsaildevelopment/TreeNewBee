// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyGroup.h"
#include "Components/BoxComponent.h"

#include "AICharacters/TheLastBastionAIBase.h"

#include "TheLastBastionHeroCharacter.h"

#include "AI/TheLastBastionGroupAIController.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

AEnemyGroup::AEnemyGroup()
{
	MainTask = EM_ToCastle;

}

void AEnemyGroup::BeginPlay()
{
	Super::BeginPlay();

	if (Hero && Hero->EnemyGroupTemp == nullptr)
		Hero->EnemyGroupTemp = this;

	// set up marching routine
}

void AEnemyGroup::SpawnChildGroup()
{
	if (!bDisabled)
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

				int GroupIndex = 0;

				// Init formation info
				int fi_totalRow = 0;
				int fi_currentRowIndex = 0;

				for (int iClass = 0; iClass < AIToSpawn.Num(); iClass++)
				{
					fi_totalRow += AIToSpawn[iClass].NumOfRow;
				}

				FormationInfo.SetNum(fi_totalRow);


				for (int iClass = 0; iClass < AIToSpawn.Num(); iClass++)
				{
					TSubclassOf<ATheLastBastionAIBase> ClassToSpawn = AIToSpawn[iClass].AIClassBP;

					int Remain = AIToSpawn[iClass].TotalNumber;
					int maxRow = AIToSpawn[iClass].NumOfRow;
					float colPadding = AIToSpawn[iClass].ColumnPadding;
					float rowPadding = AIToSpawn[iClass].RowPadding;

					float maxRowWidth = -1.0f;
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

						FormationInfo[fi_currentRowIndex] = AICountInThisRow;
						fi_currentRowIndex++;
						// calculate the relative x offset base on center root
						for (int i = 0; i < AICountInThisRow; i++)
						{
							// offset on right vector direction
							float yOffset = i * colPadding - centerOffset;
							FAICharacterInfo newCharacterInfo;

							FVector myLocation = this->GetActorLocation();
							newCharacterInfo.GroupRelativeOffset = FVector(xOffset, yOffset, 0.0f);
							FVector spawnLocation = myLocation - xOffset * GetActorForwardVector() + yOffset * GetActorRightVector();
							newCharacterInfo.AICharacter = world->SpawnActor<ATheLastBastionAIBase>(ClassToSpawn, spawnLocation, this->GetActorRotation(), spawnParam);
							newCharacterInfo.AICharacter->SpawnDefaultController();
							GroupIndex = AICharactersInfo.Num();
							newCharacterInfo.AICharacter->SetParent(this, GroupIndex);

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

void AEnemyGroup::OnReform()
{
}

void AEnemyGroup::SwapChildenOrder()
{
}

void AEnemyGroup::OnGroupVolumnOverrlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
}

void AEnemyGroup::OnGroupVolumnOverrlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}

void AEnemyGroup::SetMarchLocation(const FVector & _targetLocation, int _commandIndex)
{
	ATheLastBastionGroupAIController* groupC = Cast<ATheLastBastionGroupAIController>(GetController());
	if (groupC == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("groupC == nullptr - AAIGroupBase::SetMarchLocation"));
		return;
	}

	UBlackboardComponent* bbcGroup = groupC->GetBlackboardComponent();
	if (bbcGroup == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("bbc == nullptr - AAIGroupBase::SetMarchLocation"));
		return;
	}

	// Set new command index to _commandIndex
	// Set Target location

	FVector targetFwd, targetRight;// , targetLocation;


								   /// command pre-execute
								   // calculate the desired forward and right vector for child location padding
	switch (_commandIndex)
	{
	case GC_GOTOLOCATION:
	{
		targetFwd = _targetLocation - GetActorLocation();
		FVector2D targetFwd2D = FVector2D(targetFwd.X, targetFwd.Y).GetSafeNormal();
		targetFwd = FVector(targetFwd2D.X, targetFwd2D.Y, 0);
		targetRight = FVector(-targetFwd2D.Y, targetFwd2D.X, 0);
		break;
	}
	case GC_HOLDLOCATION:
	case GC_DISTRIBUTE:
	default:
		break;
	case GC_FORWARD:
	{
		targetFwd = this->GetActorForwardVector();
		targetRight = this->GetActorRightVector();
		break;
	}
	}

	// check for going backward
	float dir = FVector::DotProduct(GetActorForwardVector(), targetFwd);

	// swap group offset if moving backward
	if (dir < 0)
	{
		SwapChildenOrder();
	}

	/// Command post - execure
	// Immediate Change the rotation or reformat the group relative offset information based on command
	switch (_commandIndex)
	{
	case GC_GOTOLOCATION:
	default:
	{
		this->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(_targetLocation, _targetLocation + 10 * targetFwd));
		break;
	}
	case GC_HOLDLOCATION:
	case GC_FORWARD:
	case GC_DISTRIBUTE:
	{
		break;
	}
	}

	bbcGroup->SetValue<UBlackboardKeyType_Vector>(groupC->GetKeyID_TargetLocation(), _targetLocation);
	bbcGroup->SetValue<UBlackboardKeyType_Vector>(groupC->GetKeyID_TargetForward(), targetFwd);
	bbcGroup->SetValue<UBlackboardKeyType_Vector>(groupC->GetKeyID_TargetRight(), targetRight);

	// give group march command
	bbcGroup->SetValue<UBlackboardKeyType_Int>(groupC->GetKeyID_NewCommandIndex(), _commandIndex);

	// give each child an march command
	ATheLastBastionBaseAIController* baseAICtrl = nullptr;
	UBlackboardComponent* bbcChild = nullptr;
	for (int i = 0; i < AICharactersInfo.Num(); i++)
	{
		ATheLastBastionAIBase* baseAI = AICharactersInfo[i].AICharacter;
		if (baseAI && !baseAI->GetIsDead())
		{
			baseAICtrl = Cast<ATheLastBastionBaseAIController>(AICharactersInfo[i].AICharacter->GetController());
			if (baseAICtrl == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("baseAICtrl == nullptr - AAIGroupBase::SetMarchLocation"));
				continue;
			}

			bbcChild = baseAICtrl->GetBlackboardComponent();
			if (bbcChild == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("bbc == nullptr - AAIGroupBase::SetMarchLocation"));
				continue;
			}

			bbcChild->SetValue<UBlackboardKeyType_Int>(baseAICtrl->GetKeyID_NewCommandIndex(), _commandIndex);
		}
	}
}

