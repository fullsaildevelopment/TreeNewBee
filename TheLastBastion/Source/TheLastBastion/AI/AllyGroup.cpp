// Fill out your copyright notice in the Description page of Project Settings.

#include "AllyGroup.h"
#include "AICharacters/TheLastBastionAIBase.h"
#include "AI/TheLastBastionGroupAIController.h"
#include "AI/TheLastBastionBaseAIController.h"

#include "TheLastBastionHeroCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

AAllyGroup::AAllyGroup()
{
	bUseSquareFormation = false;
	bUseScatterFormation = false;
}

void AAllyGroup::BeginPlay()
{
	Super::BeginPlay();
	ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (hero && hero->CommandedGroup == nullptr)
		hero->CommandedGroup = this;

}



TSubclassOf<class ATheLastBastionAIBase> AAllyGroup::GetClassDuringAllySpawn(int _currentIndex) const
{
	int baseIndex = 0;
	for (int iClass = 0; iClass < AIToSpawn.Num(); iClass++)
	{
		if (_currentIndex <= baseIndex + AIToSpawn[iClass].TotalNumber - 1)
		{
			return AIToSpawn[iClass].AIClassBP;
		}
		else
		{
			baseIndex += AIToSpawn[iClass].TotalNumber;
		}
	}
	return nullptr;

}


void AAllyGroup::SpawnChildGroup()
{
	if (bActivated == false)
		return;

	UWorld* world = GetWorld();

	if (world == nullptr)
		return;

	FActorSpawnParameters spawnParam;
	spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;


	int totalAllyCount = 0;
	int maxColCount = 0;
	for (int iClass = 0; iClass < AIToSpawn.Num(); iClass++)
	{
		totalAllyCount += AIToSpawn[iClass].TotalNumber;
	}

	// always start with square form
	 
	bUseSquareFormation = true;

	if (totalAllyCount <= 9)
		maxColCount = 3;
	else if (totalAllyCount <= 16)
		maxColCount = 4;
	else
		maxColCount = 5;

	TSubclassOf<ATheLastBastionAIBase> ClassToSpawn = nullptr;
	int maxRowCount = FMath::CeilToInt((float)totalAllyCount / maxColCount);
	FormationInfo.SetNum(maxRowCount);
	int remain = totalAllyCount;
	float xOffset = 0;
	for (int iRow = 0; iRow < maxRowCount; iRow++)
	{
		int curColCount = (remain >= maxColCount) ? maxColCount : remain;
		float currentRowWidth = (curColCount - 1) * GroupFormation_CompactPadding_Square;
		float centerOffset = currentRowWidth * 0.5f;
		float yOffset = 0;
		int GroupIndex = 0;
		// keep information of the formation layout for reform and redistribute
		FormationInfo[iRow] = curColCount;

		for (int iCol = 0; iCol < curColCount; iCol++)
		{
			// always Start with Square Compact formation
			float rowWidth = (curColCount - 1) * GroupFormation_CompactPadding_Square;
			yOffset = iCol * GroupFormation_CompactPadding_Square - centerOffset;

			FAICharacterInfo newCharacterInfo;

			FVector myLocation = this->GetActorLocation();
			newCharacterInfo.GroupRelativeOffset = FVector(xOffset, yOffset, 0.0f);
			newCharacterInfo.GroupIndexOffset = FVector2D(iRow, iCol);
			FVector spawnLocation = myLocation - xOffset * GetActorForwardVector() + yOffset * GetActorRightVector();
			GroupIndex = AICharactersInfo.Num();

			ClassToSpawn = GetClassDuringAllySpawn(GroupIndex);
			newCharacterInfo.AICharacter = world->SpawnActor<ATheLastBastionAIBase>(ClassToSpawn, spawnLocation, this->GetActorRotation(), spawnParam);
			newCharacterInfo.AICharacter->SpawnDefaultController();
			newCharacterInfo.AICharacter->SetParent(this, GroupIndex);

			AICharactersInfo.Add(newCharacterInfo);

		}
		xOffset += GroupFormation_CompactPadding_Square;
		remain -= curColCount;
	}

	float maxGroupWidth = (maxColCount - 1) * GroupFormation_CompactPadding_Square;
	float maxGroupLength = (maxRowCount - 1) * GroupFormation_CompactPadding_Square;


}

void AAllyGroup::OnGroupVolumnOverrlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
}

void AAllyGroup::OnGroupVolumnOverrlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}



void AAllyGroup::RedistributeBy(int _cenMeters)
{
	if (bUseSquareFormation)
	{
		int maxRow = FormationInfo.Num();
		float xOffset = 0;
		if (maxRow >= 1)
		{
			int maxColSize = FormationInfo[0];
			for (int iRow = 0; iRow < maxRow; iRow++)
			{
				int curNumOfCol = FormationInfo[iRow];
				float rowWidth = curNumOfCol * _cenMeters;
				float centerOffset = 0.5f * rowWidth;
				float yOffset;
				for (int iCol = 0; iCol < curNumOfCol; iCol++)
				{
					int CurrentCharacterIndex = iRow * maxColSize + iCol;
					yOffset = iCol * _cenMeters - centerOffset;
					AICharactersInfo[CurrentCharacterIndex].GroupRelativeOffset = FVector(xOffset, yOffset, 0);
				}
				xOffset += _cenMeters;
			}
		}

	}
	else
	{
		int totalNumber = 0;

		for (int iRow = 0; iRow < FormationInfo.Num(); iRow++)
		{
			totalNumber += FormationInfo[iRow];
		}

		float xOffset = 0;
		float yOffset = 0;
		float rowWidth = (totalNumber - 1) * _cenMeters;
		float centerOffset = rowWidth * 0.5f;

		for (int i = 0; i < totalNumber; i++)
		{
			yOffset = i * _cenMeters - centerOffset;
			AICharactersInfo[i].GroupRelativeOffset = FVector(xOffset, yOffset, 0);
		}

	}

}

void AAllyGroup::SwitchToScatter()
{
	bUseScatterFormation = true;
	RedistributeBy(GroupFormation_ScatterPadding_Square);
}

void AAllyGroup::SwitchToCompact()
{
	bUseScatterFormation = false;
	RedistributeBy(GroupFormation_CompactPadding_Square);
}

void AAllyGroup::SwitchToSquare()
{
	bUseSquareFormation = true;

	float padingGap
		= bUseScatterFormation ? GroupFormation_ScatterPadding_Square : GroupFormation_CompactPadding_Square;

	// use Formation info to recreate the square formation
	int curColCount = 0;
	int maxColSize = FormationInfo[0];
	float xOffset = 0;
	float yOffset = 0;
	float rowWidth = 0;
	float centerOffset = 0;
	for (int iRow = 0; iRow < FormationInfo.Num(); iRow++)
	{
		curColCount = FormationInfo[iRow];
		rowWidth = (curColCount - 1) * padingGap;
		centerOffset = rowWidth * 0.5f;

		for (int iCol = 0; iCol < curColCount; iCol++)
		{
			int CurrentCharacterIndex = iRow * maxColSize + iCol;
			yOffset = iCol * padingGap - centerOffset;
			AICharactersInfo[CurrentCharacterIndex].GroupRelativeOffset = FVector(xOffset, yOffset, 0);
		}
		xOffset += padingGap;
	}

}

void AAllyGroup::SwitchToRow()
{
	bUseSquareFormation = false;
	int totalNumber = 0;

	for (int iRow = 0; iRow < FormationInfo.Num(); iRow++)
	{
		totalNumber += FormationInfo[iRow];
	}

	//// update formation info
	//FormationInfo.Empty();
	//FormationInfo.Add(totalNumber);

	float padingGap 
		= bUseScatterFormation ? GroupFormation_ScatterPadding_Row : GroupFormation_CompactPadding_Row;

	float xOffset = 0;
	float yOffset = 0;
	float rowWidth = (totalNumber - 1) * padingGap;
	float centerOffset = rowWidth * 0.5f;

	for (int i = 0; i < totalNumber; i++)
	{
		yOffset = i * padingGap - centerOffset;
		AICharactersInfo[i].GroupRelativeOffset = FVector(xOffset, yOffset, 0);
	}

}

void AAllyGroup::SetMarchLocation(const FVector & _targetLocation, int _commandIndex)
{
	ATheLastBastionGroupAIController* groupC = Cast<ATheLastBastionGroupAIController>(GetController());
	if (groupC == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("groupC == nullptr - AAllyGroup::SetMarchLocation"));
		return;
	}

	UBlackboardComponent* bbcGroup = groupC->GetBlackboardComponent();
	if (bbcGroup == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("bbc == nullptr - AAllyGroup::SetMarchLocation"));
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
	default:
	{
		targetFwd = _targetLocation - GetActorLocation();
		FVector2D targetFwd2D = FVector2D(targetFwd.X, targetFwd.Y).GetSafeNormal();
		targetFwd = FVector(targetFwd2D.X, targetFwd2D.Y, 0);
		targetRight = FVector(-targetFwd2D.Y, targetFwd2D.X, 0);
		break;
	}
	case GC_HOLDLOCATION:
	{
		ATheLastBastionHeroCharacter * player = Cast<ATheLastBastionHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		targetFwd = player->GetActorForwardVector();
		targetRight = player->GetActorRightVector();
		break;
	}
	case GC_FORWARD:
	case GC_BACKWARD:
	case GC_DISTRIBUTE:
	case GC_REFORM:
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
		SwapChildenOrder();

	/// Command post - execure
	// Immediate Change the rotation or reformat the group relative offset information based on command
	switch (_commandIndex)
	{
	case GC_GOTOLOCATION:
	{
		this->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(_targetLocation, _targetLocation + 10 * targetFwd));
		break;
	}
	case GC_HOLDLOCATION:
	{
		ATheLastBastionHeroCharacter * player 
			= Cast<ATheLastBastionHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		this->SetActorRotation(player->GetActorRotation());
		break;
	}
	case GC_FORWARD:
	case GC_BACKWARD:
	default:
		break;
	case GC_DISTRIBUTE:
	{
		if (bUseScatterFormation)
			SwitchToCompact();
		else
			SwitchToScatter();
		break;
	}
	case GC_REFORM:
	{
		if (bUseSquareFormation)
			SwitchToRow();
		else
			SwitchToSquare();
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
