// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyGroup.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
//#include "GameFramework/FloatingPawnMovement.h"
#include "Environment/EnemyGroupSpawner.h"

#include "AICharacters/TheLastBastionAIBase.h"
#include "TheLastBastionHeroCharacter.h"


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

	AIControllerClass = ATheLastBastionGroupAIController::StaticClass();
	
	if (MeleeVision)
	{
		MeleeVision->SetCollisionProfileName("EnemyMeleeTrigger");
	}

	// Init
	CurrentWayPoint = 0;
	bShouldCharge = false;
}

void AEnemyGroup::BeginPlay()
{
	Super::BeginPlay();
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
				// the level for children is based on hero's level
				int Level = PlayerHero->GetCharacerLevel();

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
							// Spawn
							FAICharacterInfo newCharacterInfo;

							newCharacterInfo.GroupRelativeOffset = FVector(xOffset, yOffset, 0.0f);

							FVector myLocation = this->GetActorLocation();
							FVector spawnLocation = 
								myLocation - xOffset * GetActorForwardVector() + 
								yOffset * GetActorRightVector();
							FTransform spawnTransform;
							spawnTransform.SetLocation(spawnLocation);
							spawnTransform.SetRotation(this->GetActorQuat());
							spawnTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

							//newCharacterInfo.AICharacter 
							//	= world->SpawnActor<ATheLastBastionAIBase>(ClassToSpawn, spawnLocation, this->GetActorRotation(), spawnParam);
							
							newCharacterInfo.AICharacter
								= world->SpawnActorDeferred<ATheLastBastionAIBase>(
									ClassToSpawn, spawnTransform, nullptr, nullptr, 
									ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

							if (newCharacterInfo.AICharacter)
							{
								newCharacterInfo.AICharacter->SetCharacterLevel(Level);

								UGameplayStatics::FinishSpawningActor(newCharacterInfo.AICharacter, spawnTransform);

								// End Spawn
								newCharacterInfo.AICharacter->SpawnDefaultController();
								childGroupIndex = AICharactersInfo.Num();
								newCharacterInfo.AICharacter->SetParent(this, childGroupIndex);

								AICharactersInfo.Add(newCharacterInfo);

							}


						}
						xOffset += rowPadding;
					}
					fi_currClassRowBase += maxRow;
					AIToSpawn[iClass].NumOfRow = maxRow;
				}

				SetGroupVisionVolumn();
				//maxGroupWidth = maxGroupWidth * 0.5f + SIDEPADDING;
				//maxGroupLength = (xOffset - rowPadding) * 0.5f * FRONTPADDING;

				//SetGroupVisionVolumn(maxGroupWidth, maxGroupLength);
				// set thumbNail Image for UI
				ThumbNail = AICharactersInfo[0].AICharacter->GetThumbNailImage();

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
		//float groupSpeed = MAX_FLT;

		int maxRow = FormationInfo.Num();

		for (int iRow = 0; iRow < maxRow; iRow++)
		{
			int currentRowSize = FormationInfo[iRow];
			float rowWidth = (currentRowSize - 1) * colPadding;
			float centerOffset = 0.5f * rowWidth;
			int currentCharIndex;
			//float speed;
			for (int  iCol = 0; iCol < currentRowSize; iCol++)
			{
				currentCharIndex = maxRowSize * iRow + iCol;
				yOffset = iCol * colPadding - centerOffset;
				AICharactersInfo[currentCharIndex].GroupRelativeOffset = FVector(xOffset, yOffset, 0.0f);
				//speed = AICharactersInfo[currentCharIndex].AICharacter->GetCurrentMaxSpeed();
				//if (speed < groupSpeed)
				//	groupSpeed = speed;
			}
			xOffset += rowPadding;
		}

		//float groupWidth = (FormationInfo[0] - 1) * colPadding * 0.5f + SIDEPADDING;
		//float groupLength = (xOffset - rowPadding) + 0.5f * SIDEPADDING;

		//SetGroupVisionVolumn(groupWidth, groupLength);
		SetGroupVisionVolumn();
		//GroupVolumn->SetBoxExtent(FVector(groupLength, groupWidth, GroupVolumnZ), true);
		//MoveComp->MaxSpeed = groupSpeed;
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
		UE_LOG(LogTemp, Error, TEXT("groupC == nullptr - AEnemyGroup::SetMarchLocation"));
		return;
	}


	GroupTargetLocation = _targetLocation;
	groupC->SetTargetLocation_BBC(GroupTargetLocation);

	if (bReformPending)
		OnReform();

	/// command pre-execute
	//// calculate the desired forward and right vector for child location padding
	//switch (_commandIndex)
	//{
	//case GC_GOTOLOCATION:
	//{
	//	//GroupTargetForward = _targetLocation - GetActorLocation();
	//	//FVector2D targetFwd2D = FVector2D(GroupTargetForward.X, GroupTargetForward.Y).GetSafeNormal();
	//	//GroupTargetForward = FVector(targetFwd2D.X, targetFwd2D.Y, 0);
	//	//GroupTargetRight = FVector(-targetFwd2D.Y, targetFwd2D.X, 0);
	//	//break;
	//	break;
	//}
	////case GC_GOTOLOCATION:
	////case GC_HOLDLOCATION:
	////{
	////	break;
	////}
	//default:
	//	break;
	//case GC_FORWARD:
	//case GC_BACKWARD:
	//{
	//	GroupTargetForward = this->GetActorForwardVector();
	//	GroupTargetRight = this->GetActorRightVector();
	//	break;
	//}
	//}

	//// check for going backward
	//float dir = FVector::DotProduct(GetActorForwardVector(), GroupTargetForward);
	//// swap group offset if moving backward
	//if (dir < 0)
	//{
	//	SwapChildenOrder();
	//}


	GroupTargetForward = NextWayPointTransform.GetUnitAxis(EAxis::Type::X);
	GroupTargetRight = NextWayPointTransform.GetUnitAxis(EAxis::Type::Y);


	/// Command post - execure
	// Immediate Change the rotation or reformat the group relative offset information based on command
	switch (_commandIndex)
	{
	case GC_GOTOLOCATION:
	default:
		break;
	case GC_HOLDLOCATION:
	{
		this->SetActorRotation(NextWayPointTransform.GetRotation());
		break;
	}
	}

	groupC->SetIsMoving_BBC(true);

	// give each child an march command
	SendGroupCommand(_commandIndex);
}

void AEnemyGroup::GoToNextWayPoint()
{
	AEnemyGroupSpawner* enemyGroupSpawner = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()))
		->GetEnemyGroupSpawner();

	if (enemyGroupSpawner == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("enemyGroupSpawner == nullptr - AEnemyGroup::GoToNextWayPoint"));
		return;
	}

	int NextWayPoint = CurrentWayPoint + 1;

	// If this group has next valid way point, go there, by using the transform defined by way point transform
	NextWayPointTransform = enemyGroupSpawner->GetNextWayPointFrom(PathIndex, NextWayPoint);

	
	SetMarchLocation(NextWayPointTransform.GetLocation(), GC_HOLDLOCATION);

}

void AEnemyGroup::ReachWayPoint()
{
	AEnemyGroupSpawner* enemyGroupSpawner = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()))
		->GetEnemyGroupSpawner();
	if (enemyGroupSpawner == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("enemyGroupSpawner == nullptr - AEnemyGroup::GoToNextWayPoint"));
		return;
	}


	ATheLastBastionGroupAIController* groupC = Cast<ATheLastBastionGroupAIController>(GetController());
	if (groupC == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("groupC == nullptr - AEnemyGroup::ReachWayPoint"));
		return;
	}


	CurrentWayPoint++;
	groupC->SetIsMoving_BBC(false);
	int NextWayPoint = CurrentWayPoint + 1;

	// Check if reaching the destination by checking the current waypoint has no following way point 
	bool bAtDestination = enemyGroupSpawner->HasNextWayPointOnPath(PathIndex, NextWayPoint) == false;

	if (bAtDestination)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s, Reach destination - AEnemyGroup::GoToNextWayPoint"), *GetName());
		groupC->SetAtDestination_BBC(true);
	}
	else
	{
		groupC->SetNewWayPoint_BBC(true);
	}

}

void AEnemyGroup::FindClosestTarget()
{
	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("gm == nullptr - AEnemyGroup::FindClosestTarget"));
		return;
	}

	float minimumDistance = MAX_FLT;
	float distanceSqr = 0;
	AAIGroupBase* targetGroup = nullptr;
	AAIGroupBase* playerAllyGroup = nullptr;

	// Find the cloest group
	for (int iGroup = 0; iGroup < AllyGroupMaxAmount; iGroup++)
	{
		playerAllyGroup = gm->GetAllyGroupUnitAt(iGroup);
		if (playerAllyGroup == nullptr)
			continue;

		distanceSqr = FVector2D::DistSquared(FVector2D(this->GetActorLocation()), FVector2D(playerAllyGroup->GetActorLocation()));
		if (distanceSqr < minimumDistance)
		{
			minimumDistance = distanceSqr;
			targetGroup = playerAllyGroup;
		}
	}

	// Compare with player location
	distanceSqr = FVector2D::DistSquared(FVector2D(this->GetActorLocation()), FVector2D(PlayerHero->GetActorLocation()));

	if (distanceSqr <= minimumDistance)
	{
		// targeting player
		AddThreat(PlayerHero, ThreatGain_HeroInit);
		MeleeGroupAgainstPlayer();
		SetInBattle(true);
	}
	else
	{
		// targeting player unit
		AddThreatByGroup(targetGroup);
		MeleeTargetSelectionOnOverlap(targetGroup);
		SetInBattle(true);
	}


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

void AEnemyGroup::SetInBattle(bool _val)
{
	Super::SetInBattle(_val);

	if (_val)
	{
		ATheLastBastionGroupAIController* groupC = 
			Cast<ATheLastBastionGroupAIController>(GetController());
		if (groupC == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("groupC == nullptr - AEnemyGroup::SetInBattle"));
			return;
		}
		groupC->SetIsMoving_BBC(false);
	}
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

bool AEnemyGroup::IsNearTargetLocation(float radius_Sqr) const
{
	int GroupSize = GetGroupSize();

	// check if there is any group member reach destination alive, 
	for (int iChar = 0; iChar < GroupSize; iChar++)
	{
		if (AICharactersInfo[iChar].AICharacter->GetIsDead() == false)
		{
			bool bReach = AICharactersInfo[iChar].AICharacter->IsNearTargetLocation(radius_Sqr);
			if (bReach)
				return true;
		}
	}

	return false;

	//if (bAllDamaged)
	//	bShouldCharge = true;

	//if (bShouldCharge)
	//{
	//	ATheLastBastionGroupAIController* groupC =
	//		Cast<ATheLastBastionGroupAIController>(GetController());
	//	if (groupC == nullptr)
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("groupC == nullptr - AEnemyGroup::IsNearTargetLocation"));
	//		return false;
	//	}

	//	groupC->SetIsCharging_BBC(true);
	//}
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


