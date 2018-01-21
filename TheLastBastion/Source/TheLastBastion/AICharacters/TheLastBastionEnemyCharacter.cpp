// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionEnemyCharacter.h"
#include "GameMode/SinglePlayerGM.h"
#include "Kismet/GameplayStatics.h"
#include "Combat/PawnStatsComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AIBase_AnimInstance.h"
#include "CustomType.h"
#include "UI/InGameAIHUD.h"
#include "TimerManager.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "GameFramework/CharacterMovementComponent.h"


ATheLastBastionEnemyCharacter::ATheLastBastionEnemyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(70.0f, 90.0f);
	GetCapsuleComponent()->SetCollisionProfileName("Enemy");

	GetMesh()->SetCollisionProfileName("EnemyBody");

	TSubclassOf<UUserWidget> aiHUD_Class;
	UCustomType::FindClass<UUserWidget>(aiHUD_Class, TEXT("/Game/UI/In-Game/WBP_AIHealthHUD"));
	InfoHUD = CreateDefaultSubobject<UWidgetComponent>(TEXT("AIHUD"));
	InfoHUD->SetWidgetClass(aiHUD_Class);
	InfoHUD->SetWidgetSpace(EWidgetSpace::Screen);
	FVector2D size = FVector2D(120.0f, 30.0f);
	InfoHUD->SetDrawSize(size);
	InfoHUD->bGenerateOverlapEvents = false;
	InfoHUD->SetCollisionProfileName("HUD");


	EnemyStats = CreateDefaultSubobject<UPawnStatsComponent>(TEXT("Stats"));
	PawnStats = EnemyStats;

	AiName = FText::FromString(TEXT("Base AI"));
	AILevel = 1;
}

void ATheLastBastionEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	mAnimInstanceRef = Cast<UAIBase_AnimInstance>(this->GetMesh()->GetAnimInstance());
	if (mAnimInstanceRef == nullptr) 
	{ 
		UE_LOG(LogTemp, Error,
			TEXT("ATheLastBastionEnemyCharacter can not take other AnimInstance other than AIBase_AnimInstance, - ATheLastBastionEnemyCharacter")); 
		return; 
	}

	if (EnemyStats == nullptr)
	{
		UE_LOG(LogTemp, Error,
			TEXT("PawnStats is NULL, - ATheLastBastionEnemyCharacter"));
		return;
	}

	//EnemyStats->OnHealthChanged.AddDynamic(this, &ATheLastBastionEnemyCharacter::OnHealthChangedHandle);

	// Init HUD
	if (InfoHUD == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("HUD is null - ATheLastBastionEnemyCharacter"));
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

void ATheLastBastionEnemyCharacter::OnHealthChangedHandle(const UPawnStatsComponent * _pawnStatsComp, float _damage, const UDamageType * _damageType, FName _boneName, const FVector & _shotFromDirection, const FVector & _hitLocation)
{

	UInGameAIHUD* aiHUD = Cast<UInGameAIHUD>(InfoHUD->GetUserWidgetObject());

	aiHUD->UpdateHealthBar(_pawnStatsComp);

	// if this enemy is not being locked on, 
	// we will display UI temporary with a opacity animation

	if (_pawnStatsComp->GetHpCurrent() <= 0)
	{
		OnDead();
		return;
	}

	if (!bAIHUDisDisplayedForLockedOn)
		aiHUD->ToggleUI(true, true);

	// Animation Call
	if (mAnimInstanceRef)
	{
		mAnimInstanceRef->OnBeingHit(_damage, _boneName, _shotFromDirection, _hitLocation, _pawnStatsComp);
	}

}

void ATheLastBastionEnemyCharacter::ToggleAIHUD(bool _val)
{
	bAIHUDisDisplayedForLockedOn = _val;
	UInGameAIHUD* aiHUD = Cast<UInGameAIHUD>(InfoHUD->GetUserWidgetObject());

	if (aiHUD)
	{
		if (bAIHUDisDisplayedForLockedOn)
			aiHUD->ToggleUI(true, false);
		else
			aiHUD->ToggleUI(false, false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" aiHUD is NULL - ATheLastBastionEnemyCharacter::ToggleAIHUD"));
	}
}

void ATheLastBastionEnemyCharacter::OnDead()
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
	ATheLastBastionBaseAIController* enemyC = Cast<ATheLastBastionBaseAIController>(GetController());
	enemyC->UnPossess();
	enemyC->Destroy();
	
	GetWorldTimerManager().SetTimer(mKillTimer, this, &ATheLastBastionEnemyCharacter::Kill, 1.0f, false, 10.0f);

	// Tell GM that I am dead
	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	gm->UpdateEnemyAmount(-1);
	
}

void ATheLastBastionEnemyCharacter::Kill()
{
	
	// Destroy all the actor attach on our mesh
	TArray<USceneComponent*> children = GetMesh()->GetAttachChildren();

	for (int i = 0; i < children.Num(); i++)
	{
		children[i]->GetOwner()->Destroy();
	}


	


	Destroy();
}


