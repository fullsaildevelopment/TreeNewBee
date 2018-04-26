// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastBastionHeroCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

#include "Animation/Hero_AnimInstance.h"
#include "CustomType.h"
#include "Combat/Weapon.h"
#include "Combat/Armor.h"
#include "Combat/HeroStatsComponent.h"

#include "GI_TheLastBastion.h"

#include "PCs/SinglePlayerPC.h"
#include "GameMode/SinglePlayerGM.h"

#include "Kismet/GameplayStatics.h"
#include "AI/AllyGroup.h"
#include "AI/TheLastBastionGroupAIController.h"

#include "AICharacters/TheLastBastionEnemyCharacter.h"

#include "UI/InGameFloatingText.h"
#include "UI/InGameHUD.h"

#include "AudioManager.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"



#include "VfxManager.h"
#include "Particles/ParticleSystemComponent.h"

#include "DrawDebugHelpers.h"



ATheLastBastionHeroCharacter::ATheLastBastionHeroCharacter()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->RelativeLocation = FVector(0, 0, 60);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;
	// Camera does not rotate relative to arm

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	TargetDetector = CreateDefaultSubobject<USphereComponent>(TEXT("MeleeTargetDetector"));
	TargetDetector->SetupAttachment(GetMesh(), TEXT("Root"));
	TargetDetector->InitSphereRadius(1200);
	TargetDetector->SetCanEverAffectNavigation(false);
	TargetDetector->bGenerateOverlapEvents = true;
	TargetDetector->bHiddenInGame = false;
	TargetDetector->SetCollisionProfileName("EnemyDetector");


	GetMesh()->SetCollisionProfileName("HeroBody");

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 90.0f);
	GetCapsuleComponent()->SetCollisionProfileName("Hero");

	bUsePreviousMovementAxis = false;
	bIsYawControllDisabled = false;
	bIsMovementDisabled = false;
	bIsInCommandMode = false; 
	bControlEnemyGroup = false;
	bHpRecovering = false;
	bCanOpenTradeMenu = false;
	bCanOpenRecruitMenu = false;

	Focus_CamRotationLagging = 15.0f;
	Unfocus_CamRotationLagging = 30.0f;

	CommanderPresence = 0.05f;
	CurrentExp = 0.0f;
	MaxExp = 100.0f;
	MaxExpDiv = 1.0f / MaxExp;
	bHasEnchartedWeapon = false;

	HeroStats = CreateDefaultSubobject<UHeroStatsComponent>(TEXT("Stats"));
	PawnStats = HeroStats;	

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Init Skill Slot

	InitSkillSlotProperties();


}

void ATheLastBastionHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	CameraBoom->CameraRotationLagSpeed = Unfocus_CamRotationLagging;

	// Get Anim Bp Reference
	mAnimInstanceRef = Cast<UHero_AnimInstance>(this->GetMesh()->GetAnimInstance());
	if (mAnimInstanceRef == nullptr) { UE_LOG(LogTemp, Warning, TEXT("ATheLastBastionCharacter can not take other AnimInstance other than UHero_AnimInstance, - ATheLastBastionCharacter")); return; }
	mBaseAnimRef = mAnimInstanceRef;

	ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(GetController());
	if (pc == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("pc is NULL - ATheLastBastionHeroCharacter::BeginPlay"));
		return;
	}

	mInGameHUD = pc->GetInGameHUD();
	if (mInGameHUD == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("mInGameHUD is NULL - ATheLastBastionHeroCharacter::BeginPlay"));
		return;
	}

	mInGameHUD->InitStats(HeroStats);

	bCanStartNextWave = true;

}

void ATheLastBastionHeroCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATheLastBastionHeroCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATheLastBastionHeroCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &ATheLastBastionHeroCharacter::AddControllerYaw);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATheLastBastionHeroCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATheLastBastionHeroCharacter::LookUpAtRate);


	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnJumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ATheLastBastionHeroCharacter::OnJumpReleased);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnSprintPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATheLastBastionHeroCharacter::OnSprintReleased);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnAttackPressed);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnEquipPressed);
	PlayerInputComponent->BindAction("Focus", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnMiddleMouseButtonPressed);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCorLAltPressed);

	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnRightMouseButtonPressed);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &ATheLastBastionHeroCharacter::OnRightMouseButtonReleased);

	PlayerInputComponent->BindAction("TabMeleeRange", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnTABPressed);

	PlayerInputComponent->BindAction("UseRangeWeapon", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnUseRangeWeapon);
	PlayerInputComponent->BindAction("UseSHWeapon",    IE_Pressed, this, &ATheLastBastionHeroCharacter::OnUseSHWeapon);
	PlayerInputComponent->BindAction("UseTHWeapon",    IE_Pressed, this, &ATheLastBastionHeroCharacter::OnUseTHWeapon);
	PlayerInputComponent->BindAction("UseHeavyWeapon", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnUseHeavyWeapon);

	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnPause);

	PlayerInputComponent->BindAction("Command", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandPressed);
	PlayerInputComponent->BindAction("Command", IE_Released, this, &ATheLastBastionHeroCharacter::OnCommandReleased);

	PlayerInputComponent->BindAction("MarchCommand", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandMarch);
	PlayerInputComponent->BindAction("Command_Hold", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandHold);
	//PlayerInputComponent->BindAction("Command_Follow", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandFollowing);
	PlayerInputComponent->BindAction("Command_Reform", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandReform);
	PlayerInputComponent->BindAction("Command_Distribute", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandDistribute);
	PlayerInputComponent->BindAction("Command_Forward", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandForward);
	PlayerInputComponent->BindAction("Command_Backward", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnCommandBackward);

	PlayerInputComponent->BindAction("SelectedCrew_1", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnSelectedCrew_1);
	PlayerInputComponent->BindAction("SelectedCrew_2", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnSelectedCrew_2);
	PlayerInputComponent->BindAction("SelectedCrew_3", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnSelectedCrew_3);
	PlayerInputComponent->BindAction("SelectedCrew_4", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnSelectedCrew_4);

	PlayerInputComponent->BindAction("Skill_0", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnSkillPressed_0);
	PlayerInputComponent->BindAction("Skill_1", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnSkillPressed_1);
	PlayerInputComponent->BindAction("Skill_2", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnSkillPressed_2);
	PlayerInputComponent->BindAction("Skill_3", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnSkillPressed_3);
	PlayerInputComponent->BindAction("Skill_4", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnSkillPressed_4);
	PlayerInputComponent->BindAction("Skill_5", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnSkillPressed_5);
	//PlayerInputComponent->BindAction("Skill_6", IE_Pressed, this, &ATheLastBastionHeroCharacter::OnSkillPressed_6);


	// Bind Open Menu input with functions
	PlayerInputComponent->BindAction("OpenTradeMenu", IE_Pressed, this, &ATheLastBastionHeroCharacter::OpenTradeMenu);
	PlayerInputComponent->BindAction("OpenRecruitMenu", IE_Pressed, this, &ATheLastBastionHeroCharacter::OpenRecruitMenu);

	// player has the ability to control when each wave should start
	PlayerInputComponent->BindAction("EnableEnemySpawner", IE_Pressed, this, &ATheLastBastionHeroCharacter::EnableEnemySpawner);
}

void ATheLastBastionHeroCharacter::Tick(float _deltaTime)
{
	UpdateHeroStats(_deltaTime);
}

#pragma region On Player Input
void ATheLastBastionHeroCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());

}

void ATheLastBastionHeroCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATheLastBastionHeroCharacter::MoveForward(float Value)
{

	if (bIsMovementDisabled)
		return;
	// catch the forward axis
	if (!bUsePreviousMovementAxis)
		MoveForwardAxis = Value;
	else
	{
		// the default previous movement axis is forward, i.e W is pressed
		if (MoveRightAxis == 0.0f && MoveForwardAxis == 0.0f)
			MoveForwardAxis = -1.0f;
	}

	if ((Controller != NULL) && (MoveForwardAxis != 0.0f))
	{

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, MoveForwardAxis);
	}
}

void ATheLastBastionHeroCharacter::MoveRight(float Value)
{
	if (bIsMovementDisabled)
		return;

	// Catch the right axis 
	if (!bUsePreviousMovementAxis)
		MoveRightAxis = Value;

	if ((Controller != NULL) && (MoveRightAxis != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, MoveRightAxis);
	}
}

void ATheLastBastionHeroCharacter::OnSprintPressed()
{

	mAnimInstanceRef->OnSprintPressed();
	//UE_LOG(LogTemp, Warning, TEXT("OnSprintPressed"));
}

void ATheLastBastionHeroCharacter::OnSprintReleased()
{
	mAnimInstanceRef->OnSprintReleased();
	//UE_LOG(LogTemp, Warning, TEXT("OnSprintReleased"));
}

void ATheLastBastionHeroCharacter::OnJumpPressed()
{
	mAnimInstanceRef->OnJumpStart();
}

void ATheLastBastionHeroCharacter::OnJumpReleased()
{
	mAnimInstanceRef->OnJumpStop();
}

void ATheLastBastionHeroCharacter::OnAttackPressed()
{
	mAnimInstanceRef->OnAttack();
}

void ATheLastBastionHeroCharacter::OnEquipPressed()
{
	mAnimInstanceRef->OnEquip();
}

void ATheLastBastionHeroCharacter::OnMiddleMouseButtonPressed()
{
	// figure out which target to focus
	mAnimInstanceRef->OnMiddleMouseButtonPressed();
}

void ATheLastBastionHeroCharacter::OnCorLAltPressed()
{
	mAnimInstanceRef->OnCorLAltPressed();
}

void ATheLastBastionHeroCharacter::OnRightMouseButtonPressed()
{
	mAnimInstanceRef->OnRightMouseButtonPressed();
}

void ATheLastBastionHeroCharacter::OnRightMouseButtonReleased()
{
	mAnimInstanceRef->OnRightMouseButtonReleased();
}

void ATheLastBastionHeroCharacter::AddControllerYaw(float _yaw)
{
	if (!bIsYawControllDisabled)
	{
		this->AddControllerYawInput(_yaw);
	}
}

void ATheLastBastionHeroCharacter::OnUseRangeWeapon()
{
	mAnimInstanceRef->OnChangeWeapon(EEquipType::CrossBow);
}

void ATheLastBastionHeroCharacter::OnUseSHWeapon()
{
	mAnimInstanceRef->OnChangeWeapon(EEquipType::ShieldSword);
}

void ATheLastBastionHeroCharacter::OnUseTHWeapon()
{
	mAnimInstanceRef->OnChangeWeapon(EEquipType::TwoHandSword);
}

void ATheLastBastionHeroCharacter::OnUseHeavyWeapon()
{
	mAnimInstanceRef->OnChangeWeapon(EEquipType::HeavyWeapon);
}

void ATheLastBastionHeroCharacter::OnPause()
{
	ASinglePlayerPC* pc = Cast<ASinglePlayerPC>(GetController());
	if (pc)
	{
		pc->OnPauseButtonIsPressed();
	}
}

void ATheLastBastionHeroCharacter::OpenTradeMenu()
{
	if (bCanOpenTradeMenu == true)
	{
		ASinglePlayerPC* PlayerController = Cast<ASinglePlayerPC>(GetController());
		if (PlayerController)
		{
			PlayerController->OpenTradeMenu();
			PlayerController->GetInGameHUD()->SetPopUpNotificationVisibility(false);
		}
	}
}

void ATheLastBastionHeroCharacter::OpenRecruitMenu()
{
	if (bCanOpenRecruitMenu == true)
	{
		ASinglePlayerPC* PlayerController = Cast<ASinglePlayerPC>(GetController());
		if (PlayerController)
		{
			PlayerController->OpenRecruitMenu();
			PlayerController->GetInGameHUD()->SetPopUpNotificationVisibility(false);
		}
	}
}

void ATheLastBastionHeroCharacter::EnableEnemySpawner()
{
	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm && GetCanStartNextWave())
	{
		gm->GetEnemyGroupSpawner()->EnableSpawning();
		SetCanStartNextWave(false);
	}
}

void ATheLastBastionHeroCharacter::OnCommandMarch()
{
	FVector EyesLocation;
	FRotator EyesRotation;
	GetActorEyesViewPoint(EyesLocation, EyesRotation);

	FVector ShotDirection = EyesRotation.Vector();
	FVector TraceEnd = EyesLocation + (ShotDirection * COMMAND_RANGE);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FCollisionObjectQueryParams objQueryParams;
	objQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	objQueryParams.AddObjectTypesToQuery(ECC_EnemyBody);

	FHitResult Hit;
	bool const IsHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyesLocation, TraceEnd, objQueryParams, QueryParams);
	if (IsHit)
	{

		FVector ImpactLocation = Hit.ImpactPoint;
		//UE_LOG(LogTemp, Log, TEXT("%s"), *Hit.GetActor()->GetName());
		//DrawDebugSphere(GetWorld(), ImpactLocation, 50.0f, 8, FColor::Green, false, 5.0f);

		if (CommandedGroup)
		{
			if (CommandedGroup->IsFollowing())
				CommandedGroup->OnStopFollowing();

			CommandedGroup->SetMarchLocation(ImpactLocation, GC_GOTOLOCATION);

		}
		// Temp code
		//if (EnemyGroupTemp && bControlEnemyGroup)
		//{
		//	EnemyGroupTemp->SetMarchLocation(ImpactLocation, GC_GOTOLOCATION);
		//}

		// Play sfx here
		USoundCue* VocalCommand = UAudioManager::GetSFX(ESoundEffectType::EPlayerVocalCommandsOnGroup);
		AudioComp->SetSound(VocalCommand);
		AudioComp->SetIntParameter(TEXT("Command"), 7);
		AudioComp->AttenuationSettings = VocalCommand->AttenuationSettings;
		AudioComp->Play();
	}
}

void ATheLastBastionHeroCharacter::OnCommandHold()
{
	if (CommandedGroup)
	{
		if (CommandedGroup ->IsFollowing())
			CommandedGroup->OnStopFollowing();

		FVector targetLocation = GetActorLocation() - GetActorForwardVector() * HOLD_POSITION_BWD_OFFSET;
		CommandedGroup->SetMarchLocation(targetLocation, GC_HOLDLOCATION);
		DrawDebugSphere(GetWorld(), targetLocation, 50.0f, 8, FColor::Green, false, 5.0f);

		// Play sfx here
		USoundCue* VocalCommand = UAudioManager::GetSFX(ESoundEffectType::EPlayerVocalCommandsOnGroup);
		AudioComp->SetSound(VocalCommand);
		AudioComp->SetIntParameter(TEXT("Command"), 0);
		AudioComp->AttenuationSettings = VocalCommand->AttenuationSettings;
		AudioComp->Play();
	}

}

void ATheLastBastionHeroCharacter::OnCommandFollowing()
{
	if (CommandedGroup)
	{
		CommandedGroup->OnStartFollowing();
	}

}

/** Switch from between scatter and compact */
void ATheLastBastionHeroCharacter::OnCommandDistribute()
{
	if (CommandedGroup)
	{
		if (CommandedGroup->IsFollowing())
			CommandedGroup->OnStopFollowing();

		if (CommandedGroup->IsInBattle())
			CommandedGroup->RotateGroupByGroupMember();

		// Play sfx here
		USoundCue* VocalCommand = UAudioManager::GetSFX(ESoundEffectType::EPlayerVocalCommandsOnGroup);
		AudioComp->SetSound(VocalCommand);

		if (CommandedGroup->IsScatterFormation() == false)
			AudioComp->SetIntParameter(TEXT("Command"), 5);

		else if (CommandedGroup->IsScatterFormation() == true)
			AudioComp->SetIntParameter(TEXT("Command"),6);

		AudioComp->AttenuationSettings = VocalCommand->AttenuationSettings;
		AudioComp->Play();

		//
		FVector targetLocation = CommandedGroup->GetActorLocation() + CommandedGroup->GetActorForwardVector() * 100.0f; 
		CommandedGroup->SetMarchLocation(targetLocation, GC_DISTRIBUTE);
		DrawDebugSphere(GetWorld(), targetLocation, 50.0f, 8, FColor::Green, false, 5.0f);
	}
}

void ATheLastBastionHeroCharacter::OnCommandReform()
{
	if (CommandedGroup)
	{

		bool reformCheck = CommandedGroup->CanBeReformed();

		if (reformCheck)
		{
			if (CommandedGroup->IsFollowing())
				CommandedGroup->OnStopFollowing();

			if (CommandedGroup->IsInBattle())
				CommandedGroup->RotateGroupByGroupMember();

			// Play sfx here
			USoundCue* VocalCommand = UAudioManager::GetSFX(ESoundEffectType::EPlayerVocalCommandsOnGroup);
			AudioComp->SetSound(VocalCommand);

			if (CommandedGroup->IsSquareFormation() == false)
				AudioComp->SetIntParameter(TEXT("Command"), 1);

			else if (CommandedGroup->IsSquareFormation() == true)
				AudioComp->SetIntParameter(TEXT("Command"), 2);

			AudioComp->AttenuationSettings = VocalCommand->AttenuationSettings;
			AudioComp->Play();

			//
			FVector targetLocation = CommandedGroup->GetActorLocation() + CommandedGroup->GetActorForwardVector() * 300.0f;
			CommandedGroup->SetMarchLocation(targetLocation, GC_REFORM);
			DrawDebugSphere(GetWorld(), targetLocation, 50.0f, 8, FColor::Green, false, 5.0f);
		}
	}

}

void ATheLastBastionHeroCharacter::OnCommandForward()
{   
	// Play sfx here
	USoundCue* VocalCommand = UAudioManager::GetSFX(ESoundEffectType::EPlayerVocalCommandsOnGroup);
	AudioComp->SetSound(VocalCommand);
	AudioComp->SetIntParameter(TEXT("Command"), 3);
	AudioComp->AttenuationSettings = VocalCommand->AttenuationSettings;
	AudioComp->Play();

	//
	OnCommandFwdBwd(true);
}

void ATheLastBastionHeroCharacter::OnCommandBackward()
{   
	// Play sfx here
	USoundCue* VocalCommand = UAudioManager::GetSFX(ESoundEffectType::EPlayerVocalCommandsOnGroup);
	AudioComp->SetSound(VocalCommand);
	AudioComp->SetIntParameter(TEXT("Command"), 4);
	AudioComp->AttenuationSettings = VocalCommand->AttenuationSettings;
	AudioComp->Play();

	//
	OnCommandFwdBwd(false);
}

void ATheLastBastionHeroCharacter::OnCommandFwdBwd(bool _Fwd)
{
	int command = (_Fwd) ? GC_FORWARD : GC_BACKWARD;
	int sign = (_Fwd) ? 1 : -1;

	if (CommandedGroup)
	{
		if (CommandedGroup->IsFollowing())
			CommandedGroup->OnStopFollowing();

		// if the group is not in battle, the group forward is nearly equal to group member's forward
		// if the group is in battle, rotate the group based on the average yaw of group members
		if (CommandedGroup->IsInBattle())
		{
			CommandedGroup->RotateGroupByGroupMember();
		}

		FVector targetLocation = CommandedGroup->GetFirstRowLocation() + sign * CommandedGroup->GetActorForwardVector() * 1000.0f;

		CommandedGroup->SetMarchLocation(targetLocation, command);
		DrawDebugSphere(GetWorld(), targetLocation, 50.0f, 8, FColor::Green, false, 5.0f);
	}
}

void ATheLastBastionHeroCharacter::OnCommandPressed()
{
	if (mAnimInstanceRef->GetActivatedEquipmentType() != EEquipType::CrossBow)
	{
		mInGameHUD->ToggleFireMode(true);
	}
	bIsInCommandMode = true;
	mInGameHUD->ToggleCommandList(true);
	
	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm)
	{
		gm->ToggleAllGroupUI(true);
	}
}

void ATheLastBastionHeroCharacter::OnCommandReleased()
{
	if (mAnimInstanceRef->GetActivatedEquipmentType() != EEquipType::CrossBow)
	{
		mInGameHUD->ToggleFireMode(false);
	}
	bIsInCommandMode = false;
	mInGameHUD->ToggleCommandList(false);

	ASinglePlayerGM* gm = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm)
	{
		gm->ToggleAllGroupUI(false);
	}

}

void ATheLastBastionHeroCharacter::OnSelectedCrew_1()
{
	OnSelectedCrewOnIndex(0);
}

void ATheLastBastionHeroCharacter::OnSelectedCrew_2()
{
	OnSelectedCrewOnIndex(1);
}

void ATheLastBastionHeroCharacter::OnSelectedCrew_3()
{
	OnSelectedCrewOnIndex(2);
}

void ATheLastBastionHeroCharacter::OnSelectedCrew_4()
{
	OnSelectedCrewOnIndex(3);
}

void ATheLastBastionHeroCharacter::OnSelectedCrewOnIndex(int _index)
{
	// check if we have an ally group unit on that index
	ASinglePlayerGM* spGameMode = Cast<ASinglePlayerGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (spGameMode == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("spGameMode == nullptr - ATheLastBastionHeroCharacter::OnSelectedCrewOnIndex(int _index)"));
		return;
	}

	AAllyGroup* const ally = spGameMode->GetAllyGroupUnitAt(_index);
	if (ally != nullptr)
	{
		if (CommandedGroup)
			CommandedGroup->OnDeSelected();
		CommandedGroup = ally;
		CommandedGroup->OnSelected();

		// if we have assign that ally group under our command and show up in UI
		ASinglePlayerPC* spPC = Cast<ASinglePlayerPC>(GetController());
		if (spPC)
		{
			spPC->OnSelectedCrewAt(_index);
		}

		// Play group selection sfx
		USoundCue* PlayerVoice = UAudioManager::GetSFX(ESoundEffectType::EPlayerVoiceOnGroupSelection);
		AudioComp->SetSound(PlayerVoice);
		AudioComp->SetBoolParameter(TEXT("IsRangeGroup"), ally->IsRangeGroup());
		AudioComp->AttenuationSettings = PlayerVoice->AttenuationSettings;
		AudioComp->Play();
	}
	else
	{
		UE_LOG(LogTemp, Warning, 
			TEXT("Ally unit at index: %d is nullptr - ATheLastBastionHeroCharacter::OnSelectedCrewOnIndex(int _index)"), _index);
	}
	
}

void ATheLastBastionHeroCharacter::OnSkillPressed_0()
{
	TryToUseSkill = Skill__Combo;
	mAnimInstanceRef->OnSkill(TryToUseSkill);
}

void ATheLastBastionHeroCharacter::OnSkillPressed_1()
{
	TryToUseSkill = Skill__PowerHit;
	mAnimInstanceRef->OnSkill(TryToUseSkill);
}

void ATheLastBastionHeroCharacter::OnSkillPressed_2()
{
	TryToUseSkill = Skill__Heal;
	mAnimInstanceRef->OnSkill(TryToUseSkill);


}

void ATheLastBastionHeroCharacter::OnSkillPressed_3()
{
	TryToUseSkill = Skill__WeaponCastingFire;

	if (bHasEnchartedWeapon == false)
	{
		OnWeaponEnchantStart();
	}
	else
	{
		OnWeaponEnchantStop();
	}
}

void ATheLastBastionHeroCharacter::OnSkillPressed_4()
{	

}

void ATheLastBastionHeroCharacter::OnSkillPressed_5()
{
	//TryToUseSkill = Skill__BattleCommand;
	//mAnimInstanceRef->OnSkill(TryToUseSkill);
}

//void ATheLastBastionHeroCharacter::OnSkillPressed_6()
//{
//}



void ATheLastBastionHeroCharacter::OnTABPressed()
{
	mAnimInstanceRef->OnSwapBetweenMeleeAndRange();
}

#pragma endregion

void ATheLastBastionHeroCharacter::UpdateHeroStats(float _deltaTime)
{
	// Recover Hp by time
	if (bHpRecovering)
	{
		if (HeroStats->IsFullHp() == false)
		{
			//UE_LOG(LogTemp, Log, TEXT("Recovering Hp - ATheLastBastionHeroCharacter::UpdateHeroStats"));
			HeroStats->AddHpByPercent(_deltaTime * HeroStats->GetHeroHpRecoverRate());
			mInGameHUD->SetHpOnHealthChange(HeroStats);
		}
		else
			DisnableHpRecovering();
	}


	if (mAnimInstanceRef->IsOnDefend())
	{
		//Reduce Sp by Time
		//UE_LOG(LogTemp, Log, TEXT("Consuming Sp - ATheLastBastionHeroCharacter::UpdateHeroStats"));
		HeroStats->AddSpBy(_deltaTime * HeroStats->GetSpCost_Defence());
		mInGameHUD->SetSpOnStaminaChange (HeroStats);
		// if stamina == 0, then quit defend mode
		if (HeroStats->GetStaminaCurrent() <= 0.0f)
			mAnimInstanceRef->OnZeroSp();
		//HeroStats->SetSp(120);
		//mInGameHUD->SetSpOnStaminaChange(HeroStats);

	}
	else
	{
		// Recover Sp by Time
		if (HeroStats->IsFullSp() == false)
		{
			//UE_LOG(LogTemp, Log, TEXT("Recovering Sp - ATheLastBastionHeroCharacter::UpdateHeroStats"));
			HeroStats->AddSpByPercent(_deltaTime * HeroStats->GetHeroSpRecoverRate());
			mInGameHUD->SetSpOnStaminaChange(HeroStats);

			//HeroStats->SetSp(120);

		}
	}
}

void ATheLastBastionHeroCharacter::OnWeaponEnchantStart()
{
	mAnimInstanceRef->OnSkill(TryToUseSkill);
	bHasEnchartedWeapon = true;
	HeroStats->EnableFaith(bHasEnchartedWeapon);
}

void ATheLastBastionHeroCharacter::InitSkillSlotProperties()
{
	SkillSlots.SetNum(SkillSlot_Size);

	// Set Up cool down time for skill
	SkillSlots[Skill__Combo].CoolDownTime = Skill_PowerHit_CD;
	SkillSlots[Skill__PowerHit].CoolDownTime = Skill_PowerHit_CD;
	SkillSlots[Skill__WeaponCastingFire].CoolDownTime = Skill_WeaponCastingFire_CD;
	SkillSlots[Skill__Heal].CoolDownTime = Skill_Heal_CD;

	SkillSlots[Skill__PowerHit].SpCost = SpCost_PowerHit;
	SkillSlots[Skill__Heal].SpCost = SpCost_Heal;
	SkillSlots[Skill__WeaponCastingFire].SpCost = 0;

	SkillSlots[Skill__Heal].AM_sectionName = FName("Heal");
	SkillSlots[Skill__WeaponCastingFire].AM_sectionName = FName("Heal");
}

void ATheLastBastionHeroCharacter::OnPlaySkillParticle(int _skillIndex)
{
	switch (_skillIndex)
	{
	case Skill__WeaponCastingFire:
	{
		AGear * currentWeapon = GetCurrentWeapon();
		if (currentWeapon)
			WeaponEnchantment_PSC = currentWeapon->OnWeaponEnchant();
		break;
	}
	case Skill__Heal:
	{
		UParticleSystem* healVFX = UVfxManager::GetVfx(EVfxType::PlayerHealing);
		if (healVFX)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), healVFX, GetActorLocation());
		break;
	}
	default:
		break;
	}
}


void ATheLastBastionHeroCharacter::OnWeaponEnchantStop()
{

	if (WeaponEnchantment_PSC)
		WeaponEnchantment_PSC->DestroyComponent();

	bHasEnchartedWeapon = false;
	HeroStats->EnableFaith(bHasEnchartedWeapon);
}

FVector ATheLastBastionHeroCharacter::GetPawnViewLocation() const
{
	if (FollowCamera != nullptr)
	{
		return FollowCamera->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();

}

void ATheLastBastionHeroCharacter::OnGetUp()
{
	Super::OnGetUp();

	//GetMesh()->SetCollisionProfileName("HeroBody");
	//GetCapsuleComponent()->SetCollisionProfileName("Hero");
}

void ATheLastBastionHeroCharacter::RagDollRecoverOnFinish()
{
	Super::RagDollRecoverOnFinish();
	APlayerController* pc = Cast<APlayerController>(GetController());
	if (pc)
	{
		EnableInput(pc);
	}
}

void ATheLastBastionHeroCharacter::ToggleFireMode(bool _val)
{
	mInGameHUD->ToggleFireMode(_val);
}

bool ATheLastBastionHeroCharacter::MeleeAttackSpCheck()
{

	float spCost = HeroStats->GetSpCost_MeleeAttack();
	float spRemain = HeroStats->GetStaminaCurrent() + spCost;

	bool success = spRemain > 0;
	if (success)
	{
		HeroStats->SetSp(spRemain);
		mInGameHUD->SetSpOnStaminaChange(HeroStats);
	}

	return success;
}

bool ATheLastBastionHeroCharacter::DodgeSpCheck()
{
	float spCost = HeroStats->GetSpCost_Dodge();
	float spRemain = HeroStats->GetStaminaCurrent() + spCost;

	bool success = spRemain > 0;
	if (success)
	{
		HeroStats->SetSp(spRemain);
		mInGameHUD->SetSpOnStaminaChange(HeroStats);
	}

	return success;
}

bool ATheLastBastionHeroCharacter::CounterAttackSpCheck()
{
	float spCost = HeroStats->GetSpCost_CounterAttack();
	float spRemain = HeroStats->GetStaminaCurrent() + spCost;

	bool success = spRemain > 0;
	if (success)
	{
		HeroStats->SetSp(spRemain);
		mInGameHUD->SetSpOnStaminaChange(HeroStats);
	}

	return success;
}

bool ATheLastBastionHeroCharacter::SkillCheck(int _skillIndex)
{
	float spCost = GetSkillSpCostAt(_skillIndex) * HeroStats->GetHeroSpConsumeRate_Scaler();
	float spRemain = HeroStats->GetStaminaCurrent() + spCost;
	bool success = spRemain > 0 && IsSkillCooled(_skillIndex);

	if (success)
	{
		HeroStats->SetSp(spRemain);
		mInGameHUD->SetSpOnStaminaChange(HeroStats);
		mInGameHUD->OnLaunchSkillAt(_skillIndex, GetSkillCoolDownTimeAt(_skillIndex));
	}
	return success;
}

void ATheLastBastionHeroCharacter::OnHealSkillCastSuccess()
{
	FVector myLocation = GetActorLocation();

	TArray<FHitResult> OutHits;
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(CollisionObjectType_Ally);
	ObjectParams.AddObjectTypesToQuery(CollisionObjectType_Hero);

	UWorld* World = GetWorld();
	bool const bHit = World ? World->SweepMultiByObjectType(OutHits, myLocation, myLocation, FQuat::Identity, ObjectParams, 
		FCollisionShape::MakeSphere(HeroStats->GetHealRadius())) : false;
	ATheLastBastionAIBase* aiBase = nullptr;

	for (int i = 0; i < OutHits.Num(); i++)
	{
		aiBase = Cast<ATheLastBastionAIBase>(OutHits[i].GetActor());
		if (aiBase && !aiBase->GetIsDead())
		{
			aiBase->ToggleAIHUD(true);
			aiBase->GetPawnStatsComp()->AddHpByPercent(HeroStats->GetHealAmount());
			aiBase->UpdateHUD();
		}
	}

}

float ATheLastBastionHeroCharacter::PostDamageCalculate(float _damage) const
{

	if (bHasEnchartedWeapon)
		_damage *= WeaponCasting_DamageBuff;
	ESkillBuff currentSkillBuff = mAnimInstanceRef->GetCurrentSkillBuff();
	switch (currentSkillBuff)
	{
	case ESkillBuff::Combo:
		return _damage * HeroStats->GetCombo_DamageMultiplier();
	case ESkillBuff::UnStoppable:
	case ESkillBuff::MultiDamage:
	case ESkillBuff::GainDp:
		return _damage * HeroStats->GetPowerHit_DamageMultiplier();
	default:
		return _damage;
	}
}

bool ATheLastBastionHeroCharacter::IsStuned() const
{
	return mAnimInstanceRef->GetCurrentAttackState() == EAttackState::BeingStuned;
}

bool ATheLastBastionHeroCharacter::OnCounterAttack(const FDamageInfo * const _damageInfo, const UPawnStatsComponent * const _damageCauserPawnStats)
{

	bool accept = IsOnDefend() && CounterAttackSpCheck();

	if (!accept)
		return false;

	// wpn check 
	EGearType heroGear = HeroStats->GetCurrentRightHandWeapon()->GetGearType();
	EGearType damageCauserGear = _damageCauserPawnStats->GetCurrentRightHandWeapon()->GetGearType();

	bool damageCauserHoldingHV = damageCauserGear == EGearType::GreatSword
		|| damageCauserGear == EGearType::BattleAxe || damageCauserGear == EGearType::Hammer;

	bool heroHoldingKatana = heroGear == EGearType::DoubleHandWeapon;
	if (heroHoldingKatana && damageCauserHoldingHV)
	{
		UE_LOG(LogTemp, Warning, 
			TEXT("Try to use katana to counter heavy weapon failed - ATheLastBastionHeroCharacter::OnCounterAttack"));
		return false;
	}

	if (mAnimInstanceRef)
	{
		return mAnimInstanceRef->OnCounterAttack(_damageInfo->hitDirection);
	}

	return false;
}

bool ATheLastBastionHeroCharacter::IsDoingGainDpAttack() const
{
	return mAnimInstanceRef->IsDoingGainDpAttack();
}


bool ATheLastBastionHeroCharacter::ShouldPlayHitAnimation() const
{

	if (mAnimInstanceRef->GetCurrentEquipmentType() == EEquipType::HeavyWeapon)
	{
		return mAnimInstanceRef->GetCurrentAttackState() != EAttackState::BeingHit 
			&& !mAnimInstanceRef->IsAnimCanNotInterruptByTakingDamage();
	}
	else
	{
		return !mAnimInstanceRef->IsAnimCanNotInterruptByTakingDamage();
	}
}

bool ATheLastBastionHeroCharacter::IsUnStopableAttack() const
{
	return mAnimInstanceRef->GetCurrentSkillBuff() == ESkillBuff::UnStoppable;;
}

bool ATheLastBastionHeroCharacter::IsSkillCooled(int _index) const
{
	return	mInGameHUD->IsSkilledCooledDown(_index);
}

bool ATheLastBastionHeroCharacter::IsIntentedSkillCooled() const
{
	return 	mInGameHUD->IsSkilledCooledDown(TryToUseSkill);
}

void ATheLastBastionHeroCharacter::SetSkillCoolDownTimeByLevelAt(int _skillIndex, int _level)
{
	switch (_skillIndex)
	{
	case Skill__Combo:
		SkillSlots[_skillIndex].CoolDownTime = Skill_Combo_CD - _level * Skill_LevelUp_CD_Deduction;
		break;

	case Skill__PowerHit:
		SkillSlots[_skillIndex].CoolDownTime = Skill_PowerHit_CD - _level * Skill_LevelUp_CD_Deduction;
		break;

	case Skill__Heal:
		SkillSlots[_skillIndex].CoolDownTime = Skill_Heal_CD - _level * Skill_LevelUp_CD_Deduction;
		break;
	default:
		break;
	}
}


void ATheLastBastionHeroCharacter::AddExp(float _val)
{
	CurrentExp += _val;
	if (CurrentExp >= MaxExp)
	{
		HeroStats->LevelUp();
		mInGameHUD->SetPlayerRowOnLevelUp(HeroStats);
		CurrentExp = CurrentExp - MaxExp;
	}

	mInGameHUD->SetExpBarValue(CurrentExp * MaxExpDiv);
}


void ATheLastBastionHeroCharacter::OnTakeAnyDamageHandle(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
}

void ATheLastBastionHeroCharacter::OnTakePointDamageHandle(AActor * DamagedActor, float Damage, AController * InstigatedBy, FVector HitLocation, UPrimitiveComponent * FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType * DamageType, AActor * DamageCauser)
{

	if (bIsDead)
		return;

	bool isCritical = false, isStun = false;

	// the relative position of damage causer to damaged actor
	FVector damageCauserRelative = ShotFromDirection;

	float totalDamage = HeroStats->CalculateDamage(Damage, DamageCauser, isCritical, isStun);

	if (mAnimInstanceRef->IsAnimCanNotInterruptByTakingDamage())
		isStun = false;

	bool HasDpLeft = HeroStats->GetDpCurrent() > 0;

	/// update HUD hp or dp bar
	mInGameHUD->SetDpOnDpChange(HeroStats);
	if (!HasDpLeft)
		mInGameHUD->SetHpOnHealthChange(HeroStats);

#pragma region  Pop Up floating text

	TSubclassOf<UUserWidget> fT_WBP = HeroStats->GetFloatingText_WBP();

	if (fT_WBP == nullptr)
	{
		UE_LOG(LogTemp, Error,
			TEXT("fT_WBP == nullptr - ATheLastBastionEnemyCharacter::OnTakePointDamageHandle"));
	}

	FVector2D screenPos;
	UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(this, 0), HitLocation, screenPos);
	UWorld* world = GetWorld();
	if (world && fT_WBP)
	{
		UInGameFloatingText* criticalFT = nullptr;
		UInGameFloatingText* stunFT = nullptr;

		UInGameFloatingText* damageFT = Cast<UInGameFloatingText>(CreateWidget<UUserWidget>(world, fT_WBP));
		if (damageFT)
		{
			damageFT->SetInGameFTProperty(FText::AsNumber((int)totalDamage));
			damageFT->SetRenderTranslation(screenPos);
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
				criticalFT->SetStyle(EFloatingTextStyle::Enemy);
			}
			else if (isStun)
			{
				stunFT = Cast<UInGameFloatingText>(CreateWidget<UUserWidget>(world, fT_WBP));
				stunFT->SetInGameFTProperty(FText::FromString(TEXT("Stun !!!")));
				stunFT
					->SetRenderTranslation(
						FVector2D(screenPos.X + FMath::RandRange(-20.0f, 20.0f),
							screenPos.Y + FMath::RandRange(10.0f, 20.0f)));

				stunFT->AddToViewport();
				stunFT->SetStyle(EFloatingTextStyle::Enemy);
				stunFT->SetFontSize(FontSize_Critical);
				//stunFT->SetStyle(EFloatingTextStyle::Stun);
			}

			damageFT->SetStyle(EFloatingTextStyle::Enemy);
			damageFT->AddToViewport();
		}

	}

#pragma endregion

	bool resetHpCovering = true; // are we reset the hp covering timer after this hit

	/// Animation response to Hit
	FVector RagDollImpulse = HitLocation - DamageCauser->GetTargetLocation();

	if (isStun)
	{		
		// play hit response and mark it for stun
		mAnimInstanceRef->ResetOnBeingStuned();
		mAnimInstanceRef->OnBeingHit(BoneName, damageCauserRelative, HitLocation);
		resetHpCovering = false;
	}
	else
	{
		// we have dp left, we will not react to the hit by animation

		if (HasDpLeft == false)
		{
			if (ShouldPlayHitAnimation())
			{
				// reset the attack state and movement override, cuz we are being attack except we are doing counter attack or skill
				mAnimInstanceRef->ResetOnBeingHit();
				mAnimInstanceRef->OnBeingHit(BoneName, damageCauserRelative, HitLocation);
			}
		}
		else
			resetHpCovering = false;
	}

	/// Hp recovering reset
	if (resetHpCovering)
	{
		bHpRecovering = false; 
		GetWorldTimerManager().ClearTimer(HpRecoverTimer);
		GetWorldTimerManager().SetTimer(HpRecoverTimer, this, &ATheLastBastionHeroCharacter::EnableHpRecovering, 0.1f, false, HeroStats->GetHeroHpRecoverDelay());
	}

}

void ATheLastBastionHeroCharacter::KnockOut(const FVector & dir, const AActor * _damageCauser, FName _boneName)
{
	Super::KnockOut(dir, _damageCauser, _boneName);
	APlayerController* pc = Cast<APlayerController>(GetController());
	if (pc)
	{
		DisableInput(pc);
	}
}
