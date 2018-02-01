// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionEnemyCharacter.h"
#include "GameMode/SinglePlayerGM.h"
#include "Kismet/GameplayStatics.h"
#include "Combat/PawnStatsComponent.h"
#include "Combat/Gear.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AIBase_AnimInstance.h"
#include "CustomType.h"
#include "UI/InGameAIHUD.h"
#include "TimerManager.h"
#include "AI/TheLastBastionBaseAIController.h"
#include "TheLastBastionHeroCharacter.h"
#include "UI/InGameFloatingText.h"
#include "GameFramework/CharacterMovementComponent.h"



ATheLastBastionEnemyCharacter::ATheLastBastionEnemyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(50.0f, 90.0f);
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

void ATheLastBastionEnemyCharacter::OnTakeAnyDamageHandle(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
}

// only hero attack me will show hp, floating text
void ATheLastBastionEnemyCharacter::OnTakePointDamageHandle(AActor * DamagedActor, 
	float Damage, AController * InstigatedBy, FVector HitLocation, 
	UPrimitiveComponent * FHitComponent, FName BoneName, FVector ShotFromDirection, 
	const UDamageType * DamageType, AActor * DamageCauser)
{
	if (bIsDead)
		return;

	bool isCritical = false, isStun = false;

	float totalDamage = EnemyStats->CalculateDamage(Damage, DamageCauser, isCritical, isStun);

	/// if the attack is our hero, we will render the hp bar, the floating text for this AI
	const ATheLastBastionHeroCharacter* heroAttacker = Cast<ATheLastBastionHeroCharacter>(DamageCauser);
	if (heroAttacker)
	{
		UInGameAIHUD* aiHUD = Cast<UInGameAIHUD>(InfoHUD->GetUserWidgetObject());
		aiHUD->UpdateHealthBar(EnemyStats);

		// if this AI is not being locked on, 
		// we will display UI temporary with a opacity animation
		if (!bAIHUDisDisplayedForLockedOn)
			aiHUD->ToggleUI(true, true);

		// pop up some floating text
		TSubclassOf<UUserWidget> fT_WBP = EnemyStats->GetFloatingText_WBP();
		if (fT_WBP == nullptr)
		{
			UE_LOG(LogTemp, Error,
				TEXT("fT_WBP == nullptr - ATheLastBastionEnemyCharacter::OnTakePointDamageHandle"));
		}
		FVector2D screenPos;
		UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(this, 0),HitLocation, screenPos);
		UWorld* world = GetWorld();

		if (world && fT_WBP)
		{
			UInGameFloatingText* damageFT = Cast<UInGameFloatingText>(CreateWidget<UUserWidget>(world, fT_WBP));
			if (damageFT)
			{
				damageFT->SetInGameFTProperty(FText::AsNumber((int)totalDamage));
				damageFT->SetRenderTranslation(screenPos);
				damageFT->AddToViewport();
				UInGameFloatingText* criticalFT = nullptr;
				UInGameFloatingText* stunFT = nullptr;
				// if it is a critical hit
				if (isCritical)
				{					
					criticalFT = Cast<UInGameFloatingText>(CreateWidget<UUserWidget>(world, fT_WBP));
					criticalFT->SetInGameFTProperty(FText::FromString(TEXT("Critical !!!")));
					criticalFT
						->SetRenderTranslation(
							FVector2D(screenPos.X + FMath::RandRange(-20.0f, 20.0f), 
							screenPos.Y + FMath::RandRange(-10.0f, -30.0f)));
					criticalFT->AddToViewport();

					damageFT->SetFontSize(FontSize_Critical);
					criticalFT->SetFontSize(FontSize_Critical);
				}

				if (isStun)
				{
					stunFT = Cast<UInGameFloatingText>(CreateWidget<UUserWidget>(world, fT_WBP));
					stunFT->SetInGameFTProperty(FText::FromString(TEXT("Stun !!!")));
					stunFT
						->SetRenderTranslation(
							FVector2D(screenPos.X + FMath::RandRange(-20.0f, 20.0f),
								screenPos.Y + FMath::RandRange(10.0f, 20.0f)));

					stunFT->AddToViewport();
					stunFT->SetFontSize(FontSize_Critical);
					stunFT->SetStyle(EFloatingTextStyle::Stun);
				}

				// paint the text
				switch (heroAttacker->GetCurrentWeapon()->GetElementalType())
				{
				case::EElementalType::None:
					if (criticalFT)
					{
						damageFT->SetStyle(EFloatingTextStyle::Critical);
						criticalFT->SetStyle(EFloatingTextStyle::Critical);
					}
					break;
				case::EElementalType::Fire:

					damageFT->SetStyle(EFloatingTextStyle::Fire);
					if (criticalFT)
						criticalFT->SetStyle(EFloatingTextStyle::Fire);
					break;
				case::EElementalType::Ice:
					damageFT->SetStyle(EFloatingTextStyle::Ice);
					if (criticalFT)
						criticalFT->SetStyle(EFloatingTextStyle::Ice);
					break;
				}
			}		
		}		
	}

	if (EnemyStats->GetHpCurrent() <= 0)
	{
		OnDead();
		return;
	}

	// if this ai is not get stunned, play hit animation
	mAnimInstanceRef->OnBeingHit(BoneName, ShotFromDirection, HitLocation);

	if (isStun)
	{		
		// if this ai is not get simulate ragdoll physics, play hit animation

	}
	else
	{
		// if this ai is not get stunned, play hit animation
		mAnimInstanceRef->OnBeingHit(BoneName, ShotFromDirection, HitLocation);
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


