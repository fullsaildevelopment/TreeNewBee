// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionAIBase.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AIBase_AnimInstance.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "AI/TheLastBastionGroupAIController.h"
#include "AIGroupBase.h"

#include "DrawDebugHelpers.h"

#include "Combat/PawnStatsComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "UI/InGameAIHUD.h"
#include "CustomType.h"

#define NavPointHeightAdjustLimit 500.0f;
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

	FAIHUDInitializer initializer;
	initializer.AIName = AiName;
	initializer.AILevel = AILevel;

	AI_HUD->InitRowHeader(initializer);
	AI_HUD->SetVisibility(ESlateVisibility::Hidden);

	if (bIsWalking)
		GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;

}

void ATheLastBastionAIBase::ToggleAIHUD(bool _val)
{
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
			TEXT("%s is not in a group -- ATheLastBastionAIBase::CalculateMarchTargetPosition"));
		return false;
	}

	ATheLastBastionBaseAIController *baseAICtrl = Cast<ATheLastBastionBaseAIController>(this->GetController());

	if (baseAICtrl == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("baseAICtrl == nullptr - AAIGroupBase::CalculateMarchTargetPosition"));
		return false;
	}

	int groupCommand = baseAICtrl->GetNewCommandIndex_BBC();
	// if am asked to fight, 
	// fail the group task, enter individual combat branch
	if (groupCommand == GC_FIGHT)
	{
		baseAICtrl->SetNewCommandIndex_BBC(0);
		baseAICtrl->SetOldCommandIndex_BBC(0);
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
	FVector childtargetLocation = groupTargetLocation - 
		groupTargetForward * groupRelativeOffset.X + groupTargetRight * groupRelativeOffset.Y;


	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	FCollisionObjectQueryParams objQueryParams;
	objQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	FHitResult Hit;
	FVector heightOffset = FVector::UpVector * NavPointHeightAdjustLimit;

	// Shot a top - down ray
	bool const IsHit = GetWorld()->LineTraceSingleByObjectType (Hit, childtargetLocation + heightOffset,
		childtargetLocation - heightOffset, objQueryParams, QueryParams);

	//DrawDebugLine(GetWorld(), childtargetLocation + heightOffset, childtargetLocation - heightOffset,FColor::Red, false, 6.0f);
	if (IsHit)
	{
		//UE_LOG(LogTemp, Log, TEXT("Hit %s"), *Hit.GetActor()->GetName())
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 50.0f, 8, FColor::Red, false, 6.0f);
		childtargetLocation = Hit.ImpactPoint;
	}

	// Handle focus

	FVector focusPoint = FVector::ZeroVector;
	switch (groupCommand)
	{
	case GC_GOTOLOCATION:
	case GC_FOLLOW:
		baseAICtrl->ClearFocus(EAIFocusPriority::Gameplay);
		break;
	case GC_HOLDLOCATION:
	case GC_BACKWARD:
	case GC_DISTRIBUTE:
	case GC_REFORM:
	case GC_FORWARD:
		focusPoint = mGroup->GetActorLocation() + mGroup->GetActorForwardVector() * 1000000.0f;
		baseAICtrl->ClearFocus(EAIFocusPriority::Gameplay);
		baseAICtrl->SetFocalPoint(focusPoint, EAIFocusPriority::Gameplay);
		break;

	default:
		break;
	}

	baseAICtrl->SetTargetLocation_BBC(childtargetLocation);
	baseAICtrl->SetNewCommandIndex_BBC(0);
	baseAICtrl->SetOldCommandIndex_BBC(groupCommand);
	return true;
}

void ATheLastBastionAIBase::SetTarget(AActor * _target)
{
	ATheLastBastionBaseAIController *baseAICtrl = Cast<ATheLastBastionBaseAIController>(GetController());

	if (baseAICtrl == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("baseAICtrl == nullptr - AAIGroupBase::CalculateMarchTargetPosition"));
		return;
	}

	AActor* currentTarget = baseAICtrl->GetTargetActor_BBC();
	if (_target != currentTarget)
	{
		baseAICtrl->SetTargetActor_BBC(_target);
		baseAICtrl->ClearFocus(EAIFocusPriority::Gameplay);
		baseAICtrl->SetFocus(_target, EAIFocusPriority::Gameplay);
	}
	baseAICtrl->SetNewCommandIndex_BBC(GC_FIGHT);
}

void ATheLastBastionAIBase::RequestAnotherTarget()
{
	AActor* target = mGroup->OnTargetRequest(this);
	SetTarget(target);
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

	float totalDamage = AIStats->CalculateDamage(Damage, DamageCauser, isCritical, isStun);
	float currentHp = AIStats->GetHpCurrent();

	AI_HUD->UpdateHealthBar(AIStats);

	const ATheLastBastionHeroCharacter* heroAttacker = Cast<ATheLastBastionHeroCharacter>(DamageCauser);
	if (heroAttacker)
		GenerateFloatingText(HitLocation, heroAttacker, totalDamage, isCritical, isStun);

	EvaluateAttackerThreat(DamageCauser, currentHp);

	if (currentHp <= 0)
	{
		OnDead();
		return;
	}
	
	// if this ai is alive, how he response to this hit
	HitResponse(DamageCauser);

	// if this ai is not get stunned, play hit animation
	mAnimInstanceRef->OnBeingHit(BoneName, ShotFromDirection, HitLocation);

	if (isStun)
	{
		// if this ai is not get simulate ragdoll physics, play hit animation
	}
	else
	{
		// if this ai is not get stunned, play hit animation
		//mAnimInstanceRef->OnBeingHit(BoneName, ShotFromDirection, HitLocation);
	}
}

void ATheLastBastionAIBase::GenerateFloatingText(const FVector & HitLocation,
	const ATheLastBastionHeroCharacter * heroAttacker, float totalDamage, bool isCritical, bool isStun) {}

void ATheLastBastionAIBase::EvaluateAttackerThreat(AActor * DamageCauser, float hp) {}

/** How I should response to damage causer */
void ATheLastBastionAIBase::HitResponse(AActor* DamageCauser)
{

}

void ATheLastBastionAIBase::OnDead()
{
	bIsDead = true;
	if (AI_HUD)
		AI_HUD->ToggleUI(false, false);

	// disable BT
	ATheLastBastionBaseAIController* baseAICtrl = Cast<ATheLastBastionBaseAIController>(GetController());
	baseAICtrl->UnPossess();
	baseAICtrl->Destroy();

	// Tell my Group that I am dead
	mGroup->OnChildDeath(mGroupIndex);

	// Launch kill timer
	GetWorldTimerManager().SetTimer(mKillTimer, this, &ATheLastBastionAIBase::Kill, 1.0f, false, SecondBeforeKill);

	Super::OnDead();

}

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





