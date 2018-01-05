// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionEnemyCharacter.h"
#include "Combat/PawnStatsComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AIBase_AnimInstance.h"
#include "CustomType.h"
#include "UI/InGameAIHUD.h"



ATheLastBastionEnemyCharacter::ATheLastBastionEnemyCharacter()
{

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
}

void ATheLastBastionEnemyCharacter::OnHealthChangedHandle(const UPawnStatsComponent * _pawnStatsComp, float _damage,const UDamageType * _damageType, FName _boneNmame, FVector _shotFromDirection)
{

	UInGameAIHUD* aiHUD = Cast<UInGameAIHUD>(InfoHUD->GetUserWidgetObject());

	aiHUD->UpdateHealthBar(_pawnStatsComp);

	// if this enemy is not being locked on, 
	// we will display UI temporary with a opacity animation
	if (!bAIHUDisEnabledForLockedOn)
		aiHUD->ToggleUI(true, true);

	// Animation Call
}

void ATheLastBastionEnemyCharacter::ToggleAIHUD(bool _val)
{
	bAIHUDisEnabledForLockedOn = _val;
	UInGameAIHUD* aiHUD = Cast<UInGameAIHUD>(InfoHUD->GetUserWidgetObject());

	if (aiHUD)
	{
		if (bAIHUDisEnabledForLockedOn)
			aiHUD->ToggleUI(true, false);
		else
			aiHUD->ToggleUI(false, false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" aiHUD is NULL - ATheLastBastionEnemyCharacter::ToggleAIHUD"));
	}
}


