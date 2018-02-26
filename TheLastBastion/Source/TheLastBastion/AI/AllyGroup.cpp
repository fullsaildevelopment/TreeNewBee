// Fill out your copyright notice in the Description page of Project Settings.

#include "AllyGroup.h"
#include "AICharacters/TheLastBastionAIBase.h"

#include "AI/TheLastBastionGroupAIController.h"
#include "AI/TheLastBastionBaseAIController.h"

#include "TheLastBastionHeroCharacter.h"
#include "AICharacters/TheLastBastionEnemyCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/Gameplay/AllyGroupHUD.h"

#include "GameFramework/FloatingPawnMovement.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/WidgetComponent.h"

#include "CustomType.h"
#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/SinglePlayerGM.h"




AAllyGroup::AAllyGroup()
{
	bUseSquareFormation = false;
	bUseScatterFormation = false;
	bIsFollowing = false;

	FollowingFrq = 1.0f;
	CurrentPadding = GroupFormation_CompactPadding_Square;
	
	if (MeleeVision)
	{
		MeleeVision->SetCollisionProfileName("AllyMeleeTrigger");
	}

	if (ArrowComp)
	{
		ArrowComp->ArrowColor = FColor::Green;
	}

	if (GroupHUD)
	{
		TSubclassOf<UUserWidget> HUD_Class;
		UCustomType::FindClass<UUserWidget>(HUD_Class, TEXT("/Game/UI/In-Game/WBP_AllyGroupHUD"));
		GroupHUD->SetWidgetClass(HUD_Class);
	}


	ConstructorHelpers::FObjectFinder<UBehaviorTree> bt(TEXT("/Game/Blueprints/AI/GroupPreset/BT_AllyGroupAI"));
	if (bt.Succeeded())
		BehaviorTree = bt.Object;
	else
		UE_LOG(LogTemp, Error, TEXT("Can not find behaviorTree - AAllyGroup::AAllyGroup"));

}

void AAllyGroup::BeginPlay()
{
	Super::BeginPlay();

	mFollowingLocation = GetActorLocation();
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
	if (bDisabled == true)
		return;

	UWorld* world = GetWorld();

	if (world == nullptr)
		return;

	FActorSpawnParameters spawnParam;
	spawnParam.SpawnCollisionHandlingOverride 
		= ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;


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

	// Grab the minimum speed from group member as group speed
	float groupSpeed = FLT_MAX;

	for (int iRow = 0; iRow < maxRowCount; iRow++)
	{
		int curColCount = (remain >= maxColCount) ? maxColCount : remain;
		float currentRowWidth = (curColCount - 1) * CurrentPadding;
		float centerOffset = currentRowWidth * 0.5f;
		float yOffset = 0;
		int GroupIndex = 0;
		float characterMaxSpeed = 0;
		// keep information of the formation layout for reform and redistribute
		FormationInfo[iRow] = curColCount;

		for (int iCol = 0; iCol < curColCount; iCol++)
		{
			// always Start with Square Compact formation
			float rowWidth = (curColCount - 1) * CurrentPadding;
			yOffset = iCol * CurrentPadding - centerOffset;

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

			// check if this should be the group speed
			characterMaxSpeed = newCharacterInfo.AICharacter->GetCurrentMaxSpeed();
			if (characterMaxSpeed < groupSpeed)
				groupSpeed = characterMaxSpeed;
		}
		xOffset += CurrentPadding;
		remain -= curColCount;
	}

	SetAllyGroupVisionVolumn();

	MoveComp->MaxSpeed = groupSpeed;
}

void AAllyGroup::OnReform()
{
	float xOffset = 0;
	float groupSpeed = FLT_MAX;
	int CurrentCharacterIndex = 0;

	int totalAllyCount = AICharactersInfo.Num();
	int maxColCount = FormationInfo[0];
	int maxRowCount = FormationInfo.Num();

	if (bUseSquareFormation && totalAllyCount > 3)
		SwitchToSquare();
	else
		SwitchToRow();

	// Find Slowest Speed
	float speed = 0.0f;
	for (int iCharacter = 0; iCharacter < totalAllyCount; iCharacter++)
	{
		speed = AICharactersInfo[iCharacter].AICharacter->GetCurrentMaxSpeed();
		groupSpeed = (speed < groupSpeed) ? speed : groupSpeed;
	}

	float maxGroupWidth = (maxColCount - 1) * CurrentPadding * 0.5f + SIDEPADDING;
	float maxGroupLength = (maxRowCount - 1) * CurrentPadding + 0.5 * SIDEPADDING;

	SetGroupVisionVolumn(maxGroupWidth, maxGroupLength);

	MoveComp->MaxSpeed = groupSpeed;
	bReformPending = false;
}

void AAllyGroup::SwapChildenOrder()
{
	int totalAmount = AICharactersInfo.Num();

	int swapTimes = totalAmount * 0.5f;
	//FVector offsetTemp;
	ATheLastBastionAIBase* AITemp = nullptr;

	int indexToSwap = 0;
	for (int iSwap = 0; iSwap < swapTimes; iSwap++)
	{
		indexToSwap = totalAmount - 1 - iSwap;
		AITemp = AICharactersInfo[iSwap].AICharacter;
		AICharactersInfo[iSwap].AICharacter = AICharactersInfo[indexToSwap].AICharacter;
		AICharactersInfo[indexToSwap].AICharacter = AITemp;
		AICharactersInfo[iSwap].AICharacter->SetParent(this, iSwap);
		AICharactersInfo[indexToSwap].AICharacter->SetParent(this, indexToSwap);
	}

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
				float rowWidth = (curNumOfCol - 1) * _cenMeters;
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

	CurrentPadding = (bUseSquareFormation) ? GroupFormation_ScatterPadding_Square : GroupFormation_ScatterPadding_Row;

	RedistributeBy(CurrentPadding);

	SetAllyGroupVisionVolumn();
}

void AAllyGroup::SwitchToCompact()
{
	bUseScatterFormation = false;
	CurrentPadding = (bUseSquareFormation) ? GroupFormation_CompactPadding_Square : GroupFormation_CompactPadding_Row;

	RedistributeBy(CurrentPadding);

	SetAllyGroupVisionVolumn();
}

void AAllyGroup::SwitchToSquare()
{
	if (AICharactersInfo.Num() <= 3)
		return;

	bUseSquareFormation = true;

	CurrentPadding
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
		rowWidth = (curColCount - 1) * CurrentPadding;
		centerOffset = rowWidth * 0.5f;

		for (int iCol = 0; iCol < curColCount; iCol++)
		{
			int CurrentCharacterIndex = iRow * maxColSize + iCol;
			yOffset = iCol * CurrentPadding - centerOffset;
			AICharactersInfo[CurrentCharacterIndex].GroupRelativeOffset = FVector(xOffset, yOffset, 0);
		}
		xOffset += CurrentPadding;
	}

	SetAllyGroupVisionVolumn();

}

void AAllyGroup::SwitchToRow()
{
	bUseSquareFormation = false;
	int totalNumber = AICharactersInfo.Num();

	CurrentPadding
		= bUseScatterFormation ? GroupFormation_ScatterPadding_Row : GroupFormation_CompactPadding_Row;

	float xOffset = 0;
	float yOffset = 0;
	float rowWidth = (totalNumber - 1) * CurrentPadding;
	float centerOffset = rowWidth * 0.5f;

	for (int i = 0; i < totalNumber; i++)
	{
		yOffset = i * CurrentPadding - centerOffset;
		AICharactersInfo[i].GroupRelativeOffset = FVector(xOffset, yOffset, 0);
	}

	SetAllyGroupVisionVolumn();
}

void AAllyGroup::UpdateFormationInfoByTotalNum(int _totalNum)
{
	int maxColCount;

	if (_totalNum > 16)
		maxColCount = 5;
	else if (_totalNum > 9)
		maxColCount = 4;
	else if (_totalNum >= 3)
		maxColCount = 3;
	else
		maxColCount = _totalNum;

	int maxRowCount = FMath::CeilToInt((float)_totalNum / maxColCount);

	FormationInfo.Empty();
	FormationInfo.SetNum(maxRowCount);

	int remain = _totalNum;

	// Update the FormationInfo On Reform
	for (int iRow = 0; iRow < maxRowCount; iRow++)
	{
		int curColCount = (remain >= maxColCount) ? maxColCount : remain;
		// keep information of the formation layout for reform and redistribute
		FormationInfo[iRow] = curColCount;
		remain -= curColCount;
	}
}

UTexture2D * AAllyGroup::GetThumbNailImage() const
{
	return AICharactersInfo[0].AICharacter->GetThumbNailImage();
}

void AAllyGroup::SetMarchLocation(const FVector & _targetLocation, int _commandIndex)
{
	ATheLastBastionGroupAIController* groupC = Cast<ATheLastBastionGroupAIController>(GetController());
	if (groupC == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("groupC == nullptr - AAllyGroup::SetMarchLocation"));
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
	default:
	{	
		GroupTargetForward = _targetLocation - GetActorLocation();
		FVector2D targetFwd2D = FVector2D(GroupTargetForward.X, GroupTargetForward.Y).GetSafeNormal();
		GroupTargetForward = FVector(targetFwd2D.X, targetFwd2D.Y, 0);
		GroupTargetRight = FVector(-targetFwd2D.Y, targetFwd2D.X, 0);
		break;
	}
	case GC_HOLDLOCATION:
	case GC_FOLLOW:
	{
		GroupTargetForward = PlayerHero->GetActorForwardVector();
		GroupTargetRight = PlayerHero->GetActorRightVector();
		break;
	}
	case GC_FORWARD:
	case GC_BACKWARD:
	case GC_DISTRIBUTE:
	case GC_REFORM:
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
		SwapChildenOrder();

	/// Command post - execure
	// Immediate Change the rotation or reformat the group relative offset information based on command
	switch (_commandIndex)
	{
	case GC_GOTOLOCATION:
	{
		this->SetActorRotation(UKismetMathLibrary::
			FindLookAtRotation(GroupTargetLocation, GroupTargetLocation + 10 * GroupTargetForward));
		break;
	}
	case GC_HOLDLOCATION:
	case GC_FOLLOW:
	{
		this->SetActorRotation(PlayerHero->GetActorRotation());
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

	// give group march command
	groupC->SetTargetLocation_BBC(GroupTargetLocation);
	groupC->SetNewCommandIndex_BBC(_commandIndex);

	SendGroupCommand(_commandIndex);
}

TArray<class ATheLastBastionAIBase*> AAllyGroup::GetColumnAt(int _index) const
{

	if (bUseSquareFormation)
	{
		return Super::GetColumnAt(_index);
	}
	else
	{
		TArray<class ATheLastBastionAIBase*> out;
		out.Add(AICharactersInfo[_index].AICharacter);
		return out;
	}
}

TArray<class ATheLastBastionAIBase*> AAllyGroup::GetRowAt(int _index) const
{
	if (bUseSquareFormation)
	{
		return Super::GetRowAt(_index);
	}
	else
	{
		TArray<class ATheLastBastionAIBase*> out;
		out.SetNum(AICharactersInfo.Num());
		for (int i = 0; i < out.Num(); i++)
		{
			out[i] = AICharactersInfo[i].AICharacter;
		}

		return out;
	}
}

void AAllyGroup::OnChildDeath(int _childIndex)
{
	AICharactersInfo.RemoveAt(_childIndex);

	int totalCharacterCount = AICharactersInfo.Num();
	if (totalCharacterCount == 0)
	{		

		// Unregister this group in game mode
		ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (gm)
			gm->UnRegisterAllyGroupAt(GroupIndex);

		// destroy
		Destroy();
	}
	else
	{
		UpdateFormationInfoByTotalNum(totalCharacterCount);

		for (int i = 0; i < totalCharacterCount; i++)
		{
			AICharactersInfo[i].AICharacter->SetGroupIndex(i);
		}

		bReformPending = true;
	}
}

void AAllyGroup::SetFollowingLocation()
{
	FVector targetLocation = PlayerHero->GetActorLocation() - PlayerHero->GetActorForwardVector() * 100.0f;

	float distanceSqr = FVector::DistSquared(targetLocation, mFollowingLocation);

	if (distanceSqr < 100.0f)
	{
		return;
	}

	mFollowingLocation = targetLocation;
	SetMarchLocation(targetLocation, GC_FOLLOW);
	DrawDebugSphere(GetWorld(), mFollowingLocation, 50.0f, 8, FColor::Magenta, false, 5.0f);
	UE_LOG(LogTemp, Log, TEXT("Set Following location - AAllyGroup::SetFollowingLocation"));
}

void AAllyGroup::OnStartFollowing()
{

	GetWorldTimerManager().SetTimer(mFollowingTimer, this, &AAllyGroup::SetFollowingLocation, FollowingFrq, true, 0.1f);
	bIsFollowing = true;

}

void AAllyGroup::OnStopFollowing()
{
	GetWorldTimerManager().ClearTimer(mFollowingTimer);
	bIsFollowing = false;

}

void AAllyGroup::OnSelected()
{
	for (auto& character : AICharactersInfo )
	{
		character.AICharacter->ToggleAIHUD(true);
	}
}

void AAllyGroup::OnDeSelected()
{
	for (auto& character : AICharactersInfo)
	{
		character.AICharacter->ToggleAIHUD(false);
	}

}

void AAllyGroup::OnGroupSizeChangeByNum(int _delta)
{

	int  expectedTotalNumber = AICharactersInfo.Num() + _delta;

	// Update Formation Info based on the expectecd group size after group size change
	UpdateFormationInfoByTotalNum(expectedTotalNumber);

	if (bUseSquareFormation && expectedTotalNumber <= MinSquareFormationNum)
		bUseSquareFormation = false;
	
	if (_delta > 0)
	{
		OnGroupSizeChanged_GroupSizeIncresed(expectedTotalNumber);
	}
	else
	{
		OnGroupSizeChanged_GroupSizeDecresed(expectedTotalNumber);
	}

	SetAllyGroupVisionVolumn();
}

void AAllyGroup::OnGroupSizeChanged_GroupSizeIncresed(int _expectedNum)
{

	int currCharacterMaxIndex = AICharactersInfo.Num() - 1;

	// temp variables to compute the group offseta
	int curColCount = 0;
	int maxColSize = FormationInfo[0];
	float xOffset = 0;
	float yOffset = 0;
	float rowWidth = 0;
	float centerOffset = 0;

	FActorSpawnParameters spawnParam;
	spawnParam.SpawnCollisionHandlingOverride 
		= ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// if the group size is increase
	if (bUseSquareFormation)
	{
		CurrentPadding
			= bUseScatterFormation ? GroupFormation_ScatterPadding_Square : GroupFormation_CompactPadding_Square;

		for (int iRow = 0; iRow < GetMaxRowCount(); iRow++)
		{
			curColCount = FormationInfo[iRow];
			rowWidth = (curColCount - 1) * CurrentPadding;
			centerOffset = rowWidth * 0.5f;

			for (int iCol = 0; iCol < curColCount; iCol++)
			{
				int currentCharacterIndex = iRow * maxColSize + iCol;
				yOffset = iCol * CurrentPadding - centerOffset;

				// there are some spawn we need to do
				if (currentCharacterIndex > currCharacterMaxIndex)
				{
					FAICharacterInfo newCharacterInfo;

					FVector myLocation = this->GetActorLocation();
					FVector spawnLocation = myLocation - xOffset * GetActorForwardVector() + yOffset * GetActorRightVector();

					newCharacterInfo.AICharacter = GetWorld()->SpawnActor<ATheLastBastionAIBase>(GetAllyGroupClass(), spawnLocation, this->GetActorRotation(), spawnParam);
					newCharacterInfo.AICharacter->SpawnDefaultController();
					newCharacterInfo.AICharacter->SetParent(this, currentCharacterIndex);
					AICharactersInfo.Add(newCharacterInfo);
				}
				AICharactersInfo[currentCharacterIndex].GroupRelativeOffset = FVector(xOffset, yOffset, 0);
			}
			xOffset += CurrentPadding;
		}


	}
	else
	{
		CurrentPadding
			= bUseScatterFormation ? GroupFormation_ScatterPadding_Row : GroupFormation_CompactPadding_Row;

		int totalNumber = _expectedNum;

		rowWidth = (totalNumber - 1) * CurrentPadding;
		centerOffset = rowWidth * 0.5f;

		for (int i = 0; i < totalNumber; i++)
		{
			yOffset = i * CurrentPadding - centerOffset;

			if (i > currCharacterMaxIndex)
			{
				FAICharacterInfo newCharacterInfo;

				FVector myLocation = this->GetActorLocation();
				FVector spawnLocation = myLocation - xOffset * GetActorForwardVector() + yOffset * GetActorRightVector();

				newCharacterInfo.AICharacter = GetWorld()->SpawnActor<ATheLastBastionAIBase>(GetAllyGroupClass(), spawnLocation, this->GetActorRotation(), spawnParam);
				newCharacterInfo.AICharacter->SpawnDefaultController();
				newCharacterInfo.AICharacter->SetParent(this, i);
				AICharactersInfo.Add(newCharacterInfo);
			}
			AICharactersInfo[i].GroupRelativeOffset = FVector(xOffset, yOffset, 0);
		}

	}

}

void AAllyGroup::OnGroupSizeChanged_GroupSizeDecresed(int _expectedNum)
{
	// temp variables to compute the group offseta
	int curColCount = 0;
	int maxColSize = FormationInfo[0];
	float xOffset = 0;
	float yOffset = 0;
	float rowWidth = 0;
	float centerOffset = 0;

	int currTotalNum = AICharactersInfo.Num();

	if (bUseSquareFormation)
	{
		CurrentPadding
			= bUseScatterFormation ? GroupFormation_ScatterPadding_Square : GroupFormation_CompactPadding_Square;

		for (int iRow = 0; iRow < FormationInfo.Num(); iRow++)
		{
			curColCount = FormationInfo[iRow];
			rowWidth = (curColCount - 1) * CurrentPadding;
			centerOffset = rowWidth * 0.5f;

			for (int iCol = 0; iCol < curColCount; iCol++)
			{
				int currentCharacterIndex = iRow * maxColSize + iCol;
				yOffset = iCol * CurrentPadding - centerOffset;
				AICharactersInfo[currentCharacterIndex].GroupRelativeOffset = FVector(xOffset, yOffset, 0);
			}
			xOffset += CurrentPadding;
		}

	}
	else
	{
		CurrentPadding
			= bUseScatterFormation ? GroupFormation_ScatterPadding_Row : GroupFormation_CompactPadding_Row;

		rowWidth = (_expectedNum - 1) * CurrentPadding;
		centerOffset = rowWidth * 0.5f;

		for (int i = 0; i < _expectedNum; i++)
		{
			yOffset = i * CurrentPadding - centerOffset;
			AICharactersInfo[i].GroupRelativeOffset = FVector(xOffset, yOffset, 0);
		}

	}

	// destroy child from backward to forward
	for (int iTobeDelete = currTotalNum - 1; iTobeDelete >= _expectedNum; iTobeDelete--)
	{
		AICharactersInfo[iTobeDelete].AICharacter->Kill();
		AICharactersInfo.RemoveAtSwap(iTobeDelete);
	}
}

void AAllyGroup::SetAllyGroupVisionVolumn()
{
	int maxRowCount = GetMaxRowCount();
	int maxColCount = GetMaxColoumnCount();

	float maxGroupWidth = (maxColCount - 1) * CurrentPadding * 0.5f + SIDEPADDING;
	float maxGroupLength = (maxRowCount) * CurrentPadding + 0.5 * SIDEPADDING;

	SetGroupVisionVolumn(maxGroupWidth, maxGroupLength);
}

int AAllyGroup::GetMaxColoumnCount() const
{

	if (bUseSquareFormation)
	{
		return FormationInfo[0];
	}
	else
	{
		return AICharactersInfo.Num();
	}
}

int AAllyGroup::GetMaxRowCount() const
{
	if (bUseSquareFormation)
	{
		return FormationInfo.Num();
	}
	else
	{
		return 1;
	}
}

void AAllyGroup::SetSpawnInfo(const FAISpawnInfo & _aiToSpawn)
{
	if (AIToSpawn.IsValidIndex(0))
	{
		AIToSpawn.SetNum(1);
	}
	AIToSpawn[0] = _aiToSpawn;
}

bool AAllyGroup::CanBeReformed() const
{

	if (AICharactersInfo.Num() <= 3 )
	{
		return false;
	}
	else
	{
		return true;
	}

}

void AAllyGroup::SetHUDIndex(int _index)
{
	UAllyGroupHUD* hud = Cast<UAllyGroupHUD>(GroupHUD->GetUserWidgetObject());
	if (hud)
	{
		hud->SetGroupIndexText(_index + 1);
		SetGroupIndex(_index);
	}
}
