// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionAIBase.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AIBase_AnimInstance.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "AI/TheLastBastionGroupAIController.h"
#include "AIGroupBase.h"

#include "TheLastBastionHeroCharacter.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

#include "Combat/PawnStatsComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "UI/InGameAIHUD.h"
#include "CustomType.h"

#define NavPointHeightAdjustLimit 2000.0f;
#define SecondBeforeKill 10.0f

ATheLastBastionAIBase::ATheLastBastionAIBase()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(50.0f, 90.0f);

	TSubclassOf<UUserWidget> aiHUD_Class;
	UCustomType::FindClass<UUserWidget>(aiHUD_Class, TEXT("/Game/UI/In-Game/WBP_AIHealthHUD"));
	InfoHUD = CreateDefaultSubobject<UWidgetComponent>(TEXT("AIHUD"));
	InfoHUD->SetWidgetClass(aiHUD_Class);
	InfoHUD->SetWidgetSpace(EWidgetSpace::Screen);
	FVector2D size = FVector2D(120.0f, 30.0f);
	InfoHUD->SetDrawSize(size);
	InfoHUD->bGenerateOverlapEvents = false;
	InfoHUD->SetCollisionProfileName("UI");
	

	AIStats = CreateDefaultSubobject<UPawnStatsComponent>(TEXT("Stats"));
	AIControllerClass = ATheLastBastionBaseAIController::StaticClass();
	PawnStats = AIStats;

	MinimumMoveAttackDistSq = 40000.0f;
	MeleeAttackDistSq = 70000.0f;

}

void ATheLastBastionAIBase::BeginPlay()
{
	Super::BeginPlay();
	mAnimInstanceRef = Cast<UAIBase_AnimInstance>(this->GetMesh()->GetAnimInstance());
	if (mAnimInstanceRef == nullptr)
	{
		UE_LOG(LogTemp, Error,
			TEXT("ATheLastBastionAIBase can not take other AnimInstance other than AIBase_AnimInstance - ATheLastBastionAIBase::BeginPlay"));
		return;
	}
	mBaseAnimRef = mAnimInstanceRef;

	if (AIStats == nullptr)
	{
		UE_LOG(LogTemp, Error,
			TEXT("AIStats is NULL, - ATheLastBastionAIBase"));
		return;
	}

	// Init HUD
	if (InfoHUD == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("HUD is null - ATheLastBastionAIBase"));
		return;
	}

	InfoHUD->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("head"));
	InfoHUD->AddLocalOffset(FVector(30, 0, 0));

	AI_HUD = Cast<UInGameAIHUD>(InfoHUD->GetUserWidgetObject());
	if (AI_HUD == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("aiHUD must be a UInGameAIHUD - ATheLastBastionEnemyCharacter"));
		return;
	}

	AI_HUD->SetVisibility(ESlateVisibility::Hidden);

	// Set Character base speed
	if (bIsWalkingUnit)
		GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;

	ParryEndurance = GetParryEndurance();
	DodgeEndurance = GetAutoDodgeEndurance();

}

void ATheLastBastionAIBase::ToggleAIHUD(bool _val)
{
}

bool ATheLastBastionAIBase::HasFullHealth() const
{
	return AIStats->GetHpCurrent() == AIStats->GetHpMax();
}

//void ATheLastBastionAIBase::SetLevel(int _level)
//{
//	PawnStats->SetC(_level);
//}

TArray<int> ATheLastBastionAIBase::GetResourceCost_Implementation() const
{
	TArray<int> cost;
	cost.SetNum(3);
	
	for (int i = 0; i < 3; i++)
		cost[i] = 100;

	return cost;
}

void ATheLastBastionAIBase::OnTargetDeathHandle()
{
	if (bIsDead == false)
	{
		//UE_LOG(LogTemp, Log, TEXT("%s is requesting new target - ATheLastBastionAIBase::OnTargetDeathHandle"), *this->GetName());
		RequestAnotherTarget();
	}
}

int ATheLastBastionAIBase::MeleeComboSelection(float _distSq)
{
	// Toggle Attack direction every time we pick melee attack
	bAttackFromRight = !bAttackFromRight;
	return (_distSq > MinimumMoveAttackDistSq) ? GetMeleeComboSel(true) : GetMeleeComboSel(false);
}

void ATheLastBastionAIBase::SetParent(AAIGroupBase * _Group, int _groupIndex)
{
	mGroup = _Group;
	mGroupIndex = _groupIndex;
}

bool ATheLastBastionAIBase::OnGroupTaskStart()
{
	if (mGroup == nullptr)
	{
		UE_LOG(LogTemp, Warning, 
			TEXT("%s is not in a group -- ATheLastBastionAIBase::OnGroupTaskStart"));
		return false;
	}

	ATheLastBastionBaseAIController *baseAICtrl = Cast<ATheLastBastionBaseAIController>(this->GetController());

	if (baseAICtrl == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("baseAICtrl == nullptr - AAIGroupBase::OnGroupTaskStart, %s"), *this->GetName());
		return false;
	}

	int groupCommand = baseAICtrl->GetNewCommandIndex_BBC();

	// if am asked to fight, 
	// fail the group task, enter individual combat branch
	if (groupCommand == GC_FIGHT)
	{
		baseAICtrl->SetNewCommandIndex_BBC(0);
		if (baseAICtrl->GetTargetActor_BBC() == nullptr)
			baseAICtrl->SetOldCommandIndex_BBC(0);
		else
			baseAICtrl->SetOldCommandIndex_BBC(GC_FIGHT);
		return false;
	}

	// if my previous task get interrupt, get back to it
	int OldGroupCommand = baseAICtrl->GetOldCommandIndex_BBC();
	if (OldGroupCommand > 0)
		return true;

	// if I have a new task ...
	FVector groupTargetLocation = mGroup->GetGroupTargetLocation(),
		groupTargetForward = mGroup->GetGroupTargetForward(),
		groupTargetRight = mGroup->GetGroupTargetRight();

	// Calculate the nav location point based on group offset
	FVector groupRelativeOffset = mGroup->GetGroupRelativeOffsetAt(mGroupIndex);
	ChildtargetLocation = groupTargetLocation - 
		groupTargetForward * groupRelativeOffset.X + groupTargetRight * groupRelativeOffset.Y;


	//UE_LOG(LogTemp, Log, TEXT("Group Target Fwd: %f, %f, %f"), groupTargetForward.X, groupTargetForward.Y, groupTargetForward.Z);
	//UE_LOG(LogTemp, Log, TEXT("Group Target Right: %f, %f, %f"), groupTargetRight.X, groupTargetRight.Y, groupTargetRight.Z);


	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	FCollisionObjectQueryParams objQueryParams;
	objQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	FHitResult Hit;
	FVector heightOffset = FVector::UpVector * NavPointHeightAdjustLimit;

	// Shot a top - down ray
	bool const IsHit = GetWorld()->LineTraceSingleByObjectType (Hit, ChildtargetLocation + heightOffset,
		ChildtargetLocation - heightOffset, objQueryParams, QueryParams);

	//DrawDebugLine(GetWorld(), childtargetLocation + heightOffset, childtargetLocation - heightOffset,FColor::Red, false, 6.0f);
	if (IsHit)
	{
		//UE_LOG(LogTemp, Log, TEXT("Hit %s"), *Hit.GetActor()->GetName())
		if (IsEnemy() == false)
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 50.0f, 8, FColor::Red, false, 6.0f);

		ChildtargetLocation = Hit.ImpactPoint;
	}

	// Handle focus, if this AI current has no target, therefore currently dont have focus
	if (baseAICtrl->GetTargetActor_BBC() == nullptr)
	{
		FVector focusPoint = FVector::ZeroVector;
		switch (groupCommand)
		{
		case GC_GOTOLOCATION:
		case GC_FOLLOW:
			baseAICtrl->ClearFocus(EAIFocusPriority::Gameplay);
			break;
		case GC_HOLDLOCATION:
		case GC_DISTRIBUTE:
		case GC_REFORM:
		case GC_FORWARD:
		case GC_BACKWARD:

			focusPoint = mGroup->GetActorLocation() + mGroup->GetActorForwardVector() * 1000000.0f;
			baseAICtrl->ClearFocus(EAIFocusPriority::Gameplay);
			baseAICtrl->SetFocalPoint(focusPoint, EAIFocusPriority::Gameplay);
			break;
		default:
			break;
		}
	}

	baseAICtrl->SetTargetLocation_BBC(ChildtargetLocation);
	baseAICtrl->SetNewCommandIndex_BBC(0);
	baseAICtrl->SetOldCommandIndex_BBC(groupCommand);
	return true;
}

void ATheLastBastionAIBase::SetTarget(AActor * _target, bool _asGroupMember)
{
	ATheLastBastionBaseAIController *baseAICtrl = Cast<ATheLastBastionBaseAIController>(GetController());

	if (baseAICtrl == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("baseAICtrl == nullptr - AAIGroupBase::SetTarget"));
		baseAICtrl->SetOldCommandIndex_BBC(0);
		return;
	}

	AActor* currentTarget = baseAICtrl->GetTargetActor_BBC();

	if (_target != nullptr)
		baseAICtrl->SetFocus(_target, EAIFocusPriority::Gameplay);
	else
	{
		baseAICtrl->SetTargetActor_BBC(_target);
		baseAICtrl->ClearFocus(EAIFocusPriority::Gameplay);

		// if the target is nullptr then it means no threat and battle is over at this point, 
		// then clear the group fight command if this ai is doing group fight command

		if (baseAICtrl->GetOldCommandIndex_BBC() == GC_FIGHT)
		{
			baseAICtrl->SetOldCommandIndex_BBC(0);
			return;
		}
	}

	// if our target is changed to a new target
	if (_target != currentTarget)
	{
		baseAICtrl->SetTargetActor_BBC(_target);
		ATheLastBastionCharacter* aiTarget = Cast<ATheLastBastionCharacter>(_target);
		if (aiTarget && !aiTarget->GetIsDead() && !_asGroupMember)
			aiTarget->OnBecomeUnvailbleTargetEvent.AddUObject(this, &ATheLastBastionAIBase::OnTargetDeathHandle);
	}

	// if our group is not marked in battle, then interrupt current group command, with fight
	// if already in group fight, then do nothing
	if (mGroup->IsInBattle() == false)
	{
		baseAICtrl->SetNewCommandIndex_BBC(GC_FIGHT);
	}

	// Enable Indi combat 
	baseAICtrl->SetOldCommandIndex_BBC(GC_FIGHT);

}

AActor * ATheLastBastionAIBase::GetTarget() const
{
	ATheLastBastionBaseAIController *baseAICtrl = Cast<ATheLastBastionBaseAIController>(GetController());
	return 	baseAICtrl->GetTargetActor_BBC();

}

void ATheLastBastionAIBase::RequestAnotherTarget()
{
	AActor* target = mGroup->OnTargetRequest(this);
	SetTarget(target);

	//if (target)
	//{
	//	DrawDebugLine(GetWorld(), GetActorLocation(), target->GetActorLocation(), FColor::Red, true, 5);
	//	UE_LOG(LogTemp, Log, TEXT("%s is getting new target %s"), *GetName(), *target->GetName());
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Log, TEXT("no new target for %s "), *GetName());
	//}
}

void ATheLastBastionAIBase::OnTakeAnyDamageHandle(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
}

void ATheLastBastionAIBase::OnTakePointDamageHandle(AActor * DamagedActor, 
	float Damage, AController * InstigatedBy, FVector HitLocation, 
	UPrimitiveComponent * FHitComponent, FName BoneName, FVector ShotFromDirection, 
	const UDamageType * DamageType, AActor * DamageCauser)
{

	if (bIsDead)
		return;

	bool isCritical = false, isStun = false;

	// the relative position of damage causer to damaged actor
	FVector damageCauserRelative = ShotFromDirection;

	float totalDamage = AIStats->CalculateDamage(Damage, DamageCauser, isCritical, isStun);
	float currentHp = AIStats->GetHpCurrent();

	AI_HUD->UpdateHealthBar(AIStats);

	ATheLastBastionHeroCharacter* heroAttacker = Cast<ATheLastBastionHeroCharacter>(DamageCauser);
	if (heroAttacker)
	{
		OnTakeDamageFromHero(HitLocation, heroAttacker, totalDamage, isCritical, isStun);
	}

	////////////////////////////////////////////// innocent line ////////////////////////////

	EvaluateAttackerThreat(DamageCauser, currentHp);

	//how he response to this hit
	HitResponse(DamageCauser, currentHp);


	if (currentHp <= 0)
	{

		AddExp(heroAttacker);
		
		// calculate impulse direction in case this ai is killed
		FVector RagDollImpulse = HitLocation - DamageCauser->GetActorLocation();
		OnDead(RagDollImpulse, DamageCauser, BoneName);
		return;
	}

	// endurance reduction, and when it less than zero, 
	// ai will do something else based on the class
	ParryEndurance--;
	DodgeEndurance--;
	
	// play animation
	////////////////////////////////////////////// innocent line ////////////////////////////
	if (isStun)
	{

		mAnimInstanceRef->ResetOnBeingHit();

		//// if this ai is not get simulate ragdoll physics, play hit animation
		//KnockOut(RagDollImpulse, DamageCauser,BoneName);

		//AAIController* aiCtrl = Cast<AAIController>(GetController());
		//if (aiCtrl)
		//{
		//	aiCtrl->ClearFocus(EAIFocusPriority::Gameplay);
		//}
	}
	else
	{

		if (ShouldPlayHitAnimation())
		{
			mAnimInstanceRef->ResetOnBeingHit();
			//if this ai is not get stunned, play hit animation
			mAnimInstanceRef->OnBeingHit(BoneName, damageCauserRelative, HitLocation);
		}
	}

}

void ATheLastBastionAIBase::EvaluateAttackerThreat(AActor * DamageCauser, float hp) {}

void ATheLastBastionAIBase::OnDead(const FVector& dir, const AActor* _damageCauser, FName _boneName)
{
	bIsDead = true;
	//if (AI_HUD)
	//	AI_HUD->ToggleUI(true, true);

	// disable BT
	ATheLastBastionBaseAIController* baseAICtrl = Cast<ATheLastBastionBaseAIController>(GetController());
	baseAICtrl->UnPossess();
	baseAICtrl->Destroy();

	// Tell my Group that I am dead
	mGroup->OnChildDeath(mGroupIndex);

	Super::OnDead(dir, _damageCauser, _boneName);
	
	//// Launch kill timer
	GetWorldTimerManager().SetTimer(mRagDollTimer, this, &ATheLastBastionAIBase::Kill, 1.0f, false, SecondBeforeKill);

}

void ATheLastBastionAIBase::AddExp(ATheLastBastionHeroCharacter * _heroAttacker)
{

	if (IsEnemy())
	{
		// if enemies defeated by hero, hero gain 1.5 x exp
		if (_heroAttacker != nullptr)
		{
			_heroAttacker->AddExp(1.5f * this->GetExperience());
		}
		else
		{
			ATheLastBastionHeroCharacter* hero = Cast<ATheLastBastionHeroCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			if (hero)
			{
				hero->AddExp(this->GetExperience());
			}
		}
	}
}

//bool ATheLastBastionAIBase::MountainOnDodgeParry(FName _boneName, const FVector & _damageCauserRelative, const UPawnStatsComponent * const _damageCauserPawnStats)
//{
//	if (mAnimInstanceRef)
//		return 	mAnimInstanceRef->OnDodgeParray_Mountain(_boneName, _damageCauserRelative, _damageCauserPawnStats);
//	return false;
//}

void ATheLastBastionAIBase::Kill()
{
	// Destroy all the actor attach on our mesh
	TArray<USceneComponent*> children = GetMesh()->GetAttachChildren();

	for (int i = 0; i < children.Num(); i++)
	{
		children[i]->GetOwner()->Destroy();
	}

	Destroy();
}

bool ATheLastBastionAIBase::OnFriendFireCheck(const ATheLastBastionCharacter * _target)
{

	bool isGoingToFire = false;

	if (bIsEnemy)
		isGoingToFire = (_target->IsEnemy()) ? false : true;
	else
		isGoingToFire = (_target->IsEnemy()) ? true : false;

	return isGoingToFire;
}





