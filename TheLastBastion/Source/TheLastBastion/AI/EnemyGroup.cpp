// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyGroup.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/FloatingPawnMovement.h"


#include "AICharacters/TheLastBastionAIBase.h"


#include "AI/TheLastBastionGroupAIController.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "AI/EnemyGroup.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "CustomType.h"

#include "Kismet/GameplayStatics.h"
#include "GameMode/SinglePlayerGM.h"
#include "DrawDebugHelpers.h"



AEnemyGroup::AEnemyGroup()
{
	MainTask = EM_MeleeAgainstPlayer;

	if (GroupHUD)
	{
		TSubclassOf<UUserWidget> HUD_Class;
		UCustomType::FindClass<UUserWidget>(HUD_Class, TEXT("/Game/UI/In-Game/WBP_GroupHUD"));
		GroupHUD->SetWidgetClass(HUD_Class);
	}

	if (MeleeVision)
	{
		MeleeVision->SetCollisionProfileName("EnemyMeleeTrigger");
	}


}

void AEnemyGroup::BeginPlay()
{
	Super::BeginPlay();

	// temp code for testing
	if (PlayerHero && PlayerHero->EnemyGroupTemp == nullptr)
		PlayerHero->EnemyGroupTemp = this;
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

				// offset on forward direction
				float xOffset = 0.0f;
				float yOffset = 0.0f;

				float maxGroupWidth = -1, maxGroupLength = -1;

				int childGroupIndex = 0;

				// the first row of current class
				int fi_currClassRowBase = 0;
				float colPadding = 0;
				float rowPadding = 0;

				for (int iClass = 0; iClass < AIToSpawn.Num(); iClass++)
				{
					TSubclassOf<ATheLastBastionAIBase> ClassToSpawn = AIToSpawn[iClass].AIClassBP;
					int Remain = AIToSpawn[iClass].TotalNumber;
					int maxRowSize = AIToSpawn[iClass].MaxNumOfCol;
					ensure(maxRowSize > 0);
					colPadding = AIToSpawn[iClass].ColumnPadding;
					rowPadding = AIToSpawn[iClass].RowPadding;

					if (Remain == 0)
					{
						UE_LOG(LogTemp, Error, TEXT("MaxNumOfCol can not be zero - AEnemyGroup::SpawnChildGroup"));
						return;					
					}
					// find out how many row we have for this class
					while (Remain > 0)
					{
						int temp = Remain - maxRowSize;
						if (temp > 0)
						{
							FormationInfo.Add(maxRowSize);
							Remain = temp;
						}
						else
						{
							FormationInfo.Add(Remain);
							break;
						}
					}

					int maxRow = FormationInfo.Num() - fi_currClassRowBase;
					for (int iRow = 0; iRow < maxRow; iRow++)
					{
						int currentRow = fi_currClassRowBase + iRow;
						int currentRowSize = FormationInfo[currentRow];
						float rowWidth = (currentRowSize - 1) * colPadding;
						if (rowWidth > maxGroupWidth)
							maxGroupWidth = rowWidth;
						float centerOffset = 0.5f * rowWidth;

						for (int iCol = 0; iCol < currentRowSize; iCol++)
						{
							yOffset = iCol * colPadding - centerOffset;
							FAICharacterInfo newCharacterInfo;

							FVector myLocation = this->GetActorLocation();
							newCharacterInfo.GroupRelativeOffset = FVector(xOffset, yOffset, 0.0f);
							FVector spawnLocation = myLocation - xOffset * GetActorForwardVector() + yOffset * GetActorRightVector();
							newCharacterInfo.AICharacter = world->SpawnActor<ATheLastBastionAIBase>(ClassToSpawn, spawnLocation, this->GetActorRotation(), spawnParam);
							newCharacterInfo.AICharacter->SpawnDefaultController();
							childGroupIndex = AICharactersInfo.Num();
							newCharacterInfo.AICharacter->SetParent(this, childGroupIndex);

							AICharactersInfo.Add(newCharacterInfo);
						}
						xOffset += rowPadding;
					}
					fi_currClassRowBase += maxRow;
					AIToSpawn[iClass].NumOfRow = maxRow;
				}

				maxGroupWidth = maxGroupWidth * 0.5f + SIDEPADDING;
				maxGroupLength = (xOffset - rowPadding);

				SetGroupVisionVolumn(maxGroupWidth, maxGroupLength);
				//GroupVolumn->SetBoxExtent(FVector(maxGroupLength, maxGroupWidth, GroupVolumn->GetUnscaledBoxExtent().Z), true);
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
	int totalNumOfSection = AIToSpawn.Num();

	if (totalNumOfSection == 1)
	{
		int totalCount = AICharactersInfo.Num();
		int maxRowSize = AIToSpawn[0].MaxNumOfCol;
		int Remain = totalCount;
		float colPadding = AIToSpawn[0].ColumnPadding;
		float rowPadding = AIToSpawn[0].RowPadding;
		float xOffset = 0;
		float yOffset = 0;
		float groupSpeed = MAX_FLT;

		int maxRow = FormationInfo.Num();

		for (int iRow = 0; iRow < maxRow; iRow++)
		{
			int currentRowSize = FormationInfo[iRow];
			float rowWidth = (currentRowSize - 1) * colPadding;
			float centerOffset = 0.5f * rowWidth;
			int currentCharIndex;
			float speed;
			for (int  iCol = 0; iCol < currentRowSize; iCol++)
			{
				currentCharIndex = maxRowSize * iRow + iCol;
				yOffset = iCol * colPadding - centerOffset;
				AICharactersInfo[currentCharIndex].GroupRelativeOffset = FVector(xOffset, yOffset, 0.0f);
				speed = AICharactersInfo[currentCharIndex].AICharacter->GetCurrentMaxSpeed();
				if (speed < groupSpeed)
					groupSpeed = speed;
			}
			xOffset += rowPadding;
		}

		float groupWidth = (FormationInfo[0] - 1) * colPadding * 0.5f + SIDEPADDING;
		float groupLength = (xOffset - rowPadding) + 0.5f * SIDEPADDING;

		SetGroupVisionVolumn(groupWidth, groupLength);
		//GroupVolumn->SetBoxExtent(FVector(groupLength, groupWidth, GroupVolumnZ), true);
		MoveComp->MaxSpeed = groupSpeed;
	}


	bReformPending = false;
}

void AEnemyGroup::SwapChildenOrder()
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

	GroupTargetLocation = _targetLocation;

	if (bReformPending)
		OnReform();

	/// command pre-execute
	// calculate the desired forward and right vector for child location padding
	switch (_commandIndex)
	{
	case GC_GOTOLOCATION:
	{
		GroupTargetForward = _targetLocation - GetActorLocation();
		FVector2D targetFwd2D = FVector2D(GroupTargetForward.X, GroupTargetForward.Y).GetSafeNormal();
		GroupTargetForward = FVector(targetFwd2D.X, targetFwd2D.Y, 0);
		GroupTargetRight = FVector(-targetFwd2D.Y, targetFwd2D.X, 0);
		break;
	}
	case GC_HOLDLOCATION:
	{
		break;
	}
	default:
		break;
	case GC_FORWARD:
	case GC_BACKWARD:
	{
		GroupTargetForward = this->GetActorForwardVector();
		GroupTargetRight = this->GetActorRightVector();
		break;
	}
	}

	// check for going backward
	float dir = FVector::DotProduct(GetActorForwardVector(), GroupTargetForward);

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
	{
		this->SetActorRotation(
			UKismetMathLibrary::FindLookAtRotation(_targetLocation, _targetLocation + 10 * GroupTargetForward));
		break;
	}
	case GC_HOLDLOCATION:
		break;
	default:
		break;

	}

	// give group march command
	groupC->SetTargetLocation_BBC(GroupTargetLocation);
	groupC->SetNewCommandIndex_BBC(_commandIndex);

	// give each child an march command
	SendGroupCommand(_commandIndex);
}

void AEnemyGroup::OnChildDeath(int _childIndex)
{
	AICharactersInfo.RemoveAt(_childIndex);
	int totalCharacterCount = AICharactersInfo.Num();
	int totalNumOfSection = AIToSpawn.Num();


	// find out which section this casualty belong
	// find out which row suffer this casualty

	int sectionBaseOffset = 0;
	int sufferedSection = 0;
	int sectionRangeIndex = 0;
	int sufferedRow = 0;

	for (int iSection = 0; iSection < totalNumOfSection; iSection++)
	{
		sectionRangeIndex = sectionBaseOffset + AIToSpawn[iSection].TotalNumber - 1;
		if (_childIndex <= sectionRangeIndex)
		{
			sufferedSection = iSection;
			sufferedRow = sufferedRow + AIToSpawn[iSection].NumOfRow - 1;
			break;
		}
		else
		{
			sectionBaseOffset += AIToSpawn[iSection].TotalNumber;
			sufferedRow += AIToSpawn[iSection].NumOfRow;
		}
	}

	// update section, and formation info that has this casualty
	AIToSpawn[sufferedSection].TotalNumber--;

	// if this suffered section has no one left
	if (AIToSpawn[sufferedSection].TotalNumber == 0)
	{
		// delete section
		AIToSpawn.RemoveAt(sufferedSection);
		// if there is no section left
		if (AIToSpawn.Num() == 0)
		{
			// Unregister this group in game mode
			ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
			if (gm)
				gm->UnRegisterEnemyGroupAt(GroupIndex);

			// delete the group
			Destroy();
			return;
		}
		else
		{
			// delete section will delete the last section row
			FormationInfo.RemoveAt(sufferedRow);
		}
	}
	else
	{

		FormationInfo[sufferedRow]--;
		// if the suffer row has no one left
		if (FormationInfo[sufferedRow] == 0)
		{
			// delete the row
			AIToSpawn[sufferedSection].NumOfRow--;
			FormationInfo.RemoveAt(sufferedRow);
		}
	}

	// Update All Children index
	for (int iChildren = 0; iChildren < AICharactersInfo.Num(); iChildren++)
	{
		AICharactersInfo[iChildren].AICharacter->SetGroupIndex(iChildren);
	}
	bReformPending = true;
}

int AEnemyGroup::GetMaxColoumnCount() const
{
	int NumberOfSection = AIToSpawn.Num();
	if (NumberOfSection == 1)
	{
		return FormationInfo[0];
		//return AIToSpawn[0].MaxNumOfCol;
	}

	return 0;
}

int AEnemyGroup::GetMaxRowCount() const
{
	int NumberOfSection = AIToSpawn.Num();
	if (NumberOfSection == 1)
	{
		return AIToSpawn[0].NumOfRow;
	}
	return 0;
}

void AEnemyGroup::MeleeAgainstPlayer_OnEnemyGroupMission()
{
	FVector marchLocation = PlayerHero->GetActorLocation()- 90 * FVector::UpVector;

	DrawDebugSphere(GetWorld(), marchLocation, 50, 8, FColor::Purple, false, 6);


	SetMarchLocation(marchLocation, GC_GOTOLOCATION);
}

void AEnemyGroup::MeleeGroupAgainstPlayer()
{
	for (int iCharacter = 0; iCharacter < AICharactersInfo.Num(); iCharacter++)
	{
		AICharactersInfo[iCharacter].AICharacter->SetTarget(PlayerHero);
	}
}


