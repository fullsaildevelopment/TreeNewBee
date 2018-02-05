// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionAIBase.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AIBase_AnimInstance.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "AI/TheLastBastionGroupAIController.h"
#include "AIGroupBase.h"


#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "UI/InGameAIHUD.h"
#include "CustomType.h"

#define NavPointHeightAdjustLimit 500.0f;

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

	UInGameAIHUD* aiHUD = Cast<UInGameAIHUD>(InfoHUD->GetUserWidgetObject());
	if (aiHUD == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("aiHUD must be a UInGameAIHUD - ATheLastBastionEnemyCharacter"));
		return;
	}

	FAIHUDInitializer initializer;
	initializer.AIName = AiName;
	initializer.AILevel = AILevel;

	aiHUD->InitRowHeader(initializer);
	aiHUD->SetVisibility(ESlateVisibility::Hidden);

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

void ATheLastBastionAIBase::CalculateMarchTargetPosition()
{
	if (mGroup == nullptr)
	{
		UE_LOG(LogTemp, Warning, 
			TEXT("%s is not in a group -- ATheLastBastionAIBase::CalculateMarchTargetPosition"));
		return;
	}
	
	ATheLastBastionGroupAIController* groupC = Cast<ATheLastBastionGroupAIController>(mGroup->GetController());
	if (groupC == nullptr)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("groupC is null -- ATheLastBastionAIBase::CalculateMarchTargetPosition"));
		return;
	}

	UBlackboardComponent* bbcGroup = groupC->GetBlackboardComponent();
	if (bbcGroup == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("bbcGroup == nullptr - ATheLastBastionAIBase::CalculateMarchTargetPosition"));
		return;
	}

	// Get group location from its blackboard
	FVector groupTargetLocation = bbcGroup->GetValue<UBlackboardKeyType_Vector>(groupC->GetKeyID_TargetLocation()),
		groupTargetForward = bbcGroup->GetValue<UBlackboardKeyType_Vector>(groupC->GetKeyID_TargetForward()),
		groupTargetRight = bbcGroup->GetValue<UBlackboardKeyType_Vector>(groupC->GetKeyID_TargetRight());


	ATheLastBastionBaseAIController *baseAICtrl = Cast<ATheLastBastionBaseAIController>(this->GetController());
	UBlackboardComponent* bbcChild = nullptr;

	if (baseAICtrl == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("baseAICtrl == nullptr - AAIGroupBase::CalculateMarchTargetPosition"));
		return;
	}
	bbcChild = baseAICtrl->GetBlackboardComponent();
	if (bbcChild == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("bbc == nullptr - AAIGroupBase::CalculateMarchTargetPosition"));
		return;
	}

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
	int groupCommand = bbcChild->GetValue<UBlackboardKeyType_Int>(baseAICtrl->GetKeyID_NewCommandIndex());
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
		baseAICtrl->SetFocalPoint(focusPoint, EAIFocusPriority::Gameplay);
		break;

	default:
		break;
	}

	

	bbcChild->SetValue<UBlackboardKeyType_Vector>(baseAICtrl->GetKeyID_TargetLocation(), childtargetLocation);
	bbcChild->SetValue<UBlackboardKeyType_Int>(baseAICtrl->GetKeyID_NewCommandIndex(), 0);

}

void ATheLastBastionAIBase::OnDead()
{

	bIsDead = true;
	UInGameAIHUD* aiHUD = Cast<UInGameAIHUD>(InfoHUD->GetUserWidgetObject());
	aiHUD->ToggleUI(false, false);

	// condition for ragdoll
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// disable BT
	ATheLastBastionBaseAIController* baseAICtrl = Cast<ATheLastBastionBaseAIController>(GetController());
	baseAICtrl->UnPossess();
	baseAICtrl->Destroy();

	// Tell my Group that I am dead
	mGroup->OnChildDeath(mGroupIndex);

	// Launch kill timer
	GetWorldTimerManager().SetTimer(mKillTimer, this, &ATheLastBastionAIBase::Kill, 1.0f, false, 10.0f);

	//// Tell GM that I am dead
	//ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	//gm->UpdateEnemyAmount(-1);

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





