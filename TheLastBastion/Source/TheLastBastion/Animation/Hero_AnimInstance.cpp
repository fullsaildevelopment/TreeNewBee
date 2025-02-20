// Fill out your copyright notice in the Description page of Project Settings.

#include "Hero_AnimInstance.h"
#include "TheLastBastionHeroCharacter.h"

#include "Combat/HeroStatsComponent.h"
#include "Combat/RangeWeapon.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine.h"

const TArray<FName> LongSwordAttack_Montage_SN
= { FName("LSCombo_0"), FName("LSCombo_1") ,FName("LSCombo_2"), FName("LSCombo_3"), FName("LSCombo_4"), FName("LSCombo_5") };

const TArray<FName> AxeMaceAttack_Montage_SN
= { FName("AMCombo_0"), FName("AMCombo_1") ,FName("AMCombo_2"), FName("AMCombo_3"), FName("AMCombo_4"), FName("AMCombo_5") };

const TArray<FName> BattleAxeAttack_Montage_SN
= { FName("BACombo_0"), FName("BACombo_1")};

const TArray<FName> GSHammerAttack_Montage_SN
= { FName("GHCombo_0"), FName("GHCombo_1") ,FName("GHCombo_2")};

const TArray<FName> KatanaAttack_Montage_SN
= { FName("KACombo_0"), FName("KACombo_1") ,FName("KACombo_2"), FName("KACombo_3"), FName("KACombo_4"), FName("KACombo_5") };

const TArray<FName> KatanaAirAttack_Montage_SN
= { FName("KACombo_A0"), FName("KACombo_A1"),FName("KACombo_A2") };

#define MONTAGE_SN_HERO_SnsEquip           TEXT("SnsEquip")
#define MONTAGE_SN_HERO_SnsUnequip         TEXT("SnsUnequip")
#define MONTAGE_SN_HERO_CBEquip            TEXT("CBEquip")
#define MONTAGE_SN_HERO_CBUnequip          TEXT("CBUnequip")
#define MONTAGE_SN_HERO_HVEquip            TEXT("HVEquip")
#define MONTAGE_SN_HERO_HVUnequip          TEXT("HVUnequip")
#define MONTAGE_SN_HERO_DHEquip            TEXT("DHEquip")
#define MONTAGE_SN_HERO_DHUnequip          TEXT("DHUnequip")

#define MONTAGE_SN_HERO_SnsDodgeFwd        TEXT("SnsFwd")
#define MONTAGE_SN_HERO_SnsDodgeBwd        TEXT("SnsBwd")
#define MONTAGE_SN_HERO_SnsDodgeLeft       TEXT("SnsLeft")
#define MONTAGE_SN_HERO_SnsDodgeRight      TEXT("SnsRight")
#define MONTAGE_SN_HERO_HVDodgeFwd         TEXT("HVFwd")
#define MONTAGE_SN_HERO_HVDodgeBwd         TEXT("HVBwd")
#define MONTAGE_SN_HERO_HVDodgeLeft        TEXT("HVLeft")
#define MONTAGE_SN_HERO_HVDodgeRight       TEXT("HVRight")
#define MONTAGE_SN_HERO_KatanaDodgeFwd     TEXT("KAFwd")
#define MONTAGE_SN_HERO_KatanaDodgeFwdF    TEXT("KAFwdF")
#define MONTAGE_SN_HERO_KatanaDodgeBwd     TEXT("KABwd")
#define MONTAGE_SN_HERO_KatanaDodgeLeft    TEXT("KALeft")
#define MONTAGE_SN_HERO_KatanaDodgeRight   TEXT("KARight")
#define MONTAGE_SN_CA_SnsFront   TEXT("Sns_Front")
#define MONTAGE_SN_CA_SnsLeft    TEXT("Sns_Left")
#define MONTAGE_SN_CA_SnsRight   TEXT("Sns_Right")
#define MONTAGE_SN_CA_Hammer     TEXT("Hammer")
#define MONTAGE_SN_CA_BattleAxe  TEXT("BattleAxe")
#define MONTAGE_SN_CA_Katana     TEXT("Katana")

#define JumpZ_Sns 800
#define JumpZ_HV  700
#define JumpZ_Katana 850
#define JumpZ_Travel 900
#define GreatSwordAnimRate 1.1f





#pragma region Class Base Interface

UHero_AnimInstance::UHero_AnimInstance(const FObjectInitializer& _objectInitalizer) :Super(_objectInitalizer)
{
	HeadTrackRate = 3.0f;

	ActivatedEquipment = EEquipType::Travel;
	AttackState = EAttackState::None;
	bTryToSprint = false;
	bTryToMove = false;
	bIsSprinting = false;
	bTryToEquip = false;
	bVelocityOverrideByAnim = false;
	bAnimInterruptRobust = false;

	MomentumRatioByActor = 0.7f;

	// Melee Init
	CurrentComboIndex = 0;
	CurrentEquipment = EEquipType::ShieldSword;

	FocusDodgeDirection = EFocusDodgeDirection::None;
	bIsFocused = false;
	//bDuringGainDpAttack = false;

	
	CurrentDefendPoseAlpha = 0.0f;
	TargetDefendPoseAlpha = 0.0f;
	// Range Init
	bTryToZoomIn = false;
	CameraZoomInRate = 10.0f;
	CameraShiftRate = 3.0f;
	CameraEquipOffset = FVector(100, 50, 0);
	CameraZoomOffset = FVector(200, 50, 10);

	Skill_Montage = AM_Skill;
	SkillBuff = ESkillBuff::None;


}

void UHero_AnimInstance::OnBeginPlay()
{
	//UE_LOG(LogTemp, Warning, TEXT("UHero_AnimInstance::OnBeginPlay"));
	APawn* pawn = TryGetPawnOwner();
	mCharacter = Cast<ATheLastBastionHeroCharacter>(pawn);
	if (mCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("The Hero can only assigned to ATheLastBastionCharacter - UHero_AnimInstance::OnBeginPlay "));
		return;
	}
	mBaseCharacter = mCharacter;

	//UpdateAnimationSetOnWeaponChange(EGearType::LongSword);

	Super::OnBeginPlay();
}

void UHero_AnimInstance::OnInit()
{
	//UE_LOG(LogTemp, Warning, TEXT("UHero_AnimInstance Call OnInit"));
}

void UHero_AnimInstance::OnUpdate(float _deltaTime)
{
	if (mCharacter != nullptr)
	{
		//Super::OnUpdate(_deltaTime);

		// Head Track
		HeadTrack();

		// Get Control Axis for strafing
		MoveForwardAxis = mCharacter->GetMoveForwardAxis();
		MoveRightAxis = mCharacter->GetMoveRightAxis();

		UCharacterMovementComponent* movementComp = mCharacter->GetCharacterMovement();

		// Check if the play is falling?
		bIsInAir = movementComp->IsFalling();
		// Check if the player try to move?
		bTryToMove = movementComp->GetCurrentAcceleration().SizeSquared() > 0;

		currentSpeed = movementComp->Velocity.Size();

		if (ActivatedEquipment == EEquipType::Travel || AttackState == EAttackState::Dodging || AttackState == EAttackState::PostDodging)
			mAccelerationDirection = movementComp->GetCurrentAcceleration().GetSafeNormal();
		else
			mAccelerationDirection = UKismetMathLibrary::GetForwardVector(mCharacter->GetControlRotation());

		Acceleration_bodySpace
			= UKismetMathLibrary::InverseTransformDirection(mCharacter->GetTransform(), mAccelerationDirection);
		turn = FMath::RadiansToDegrees(FMath::Atan2(Acceleration_bodySpace.Y, Acceleration_bodySpace.X));

		bIsShooting = Montage_IsPlaying(Fire_Montage);

		switch (CurrentEquipment)
		{
		case EEquipType::CrossBow:
			RangeUpdate(movementComp, _deltaTime);
			break;
		case EEquipType::ShieldSword:
		case EEquipType::HeavyWeapon:
		case EEquipType::TwoHandSword:
			MeleeUpdate(movementComp, _deltaTime);
			break;
		}


	}
}

void UHero_AnimInstance::MeleeUpdate(UCharacterMovementComponent * movementComp, float _deltaTime)
{
	MeleeRotateRateAndMaxSpeedUpdate(movementComp);

	///** movement update during velocity override
	if (bVelocityOverrideByAnim)
	{
		float Z = movementComp->Velocity.Z;
		FVector overrideVelocity;

		switch (AttackState)
		{
		case EAttackState::None:
		case EAttackState::PreWinding:
		case EAttackState::Attacking:
		case EAttackState::ReadyForNext:
		default:
			overrideVelocity = mCharacter->GetActorForwardVector() * GetCurveValue("Speed");
			movementComp->Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, Z);
			break;
		case EAttackState::Dodging:
		case EAttackState::PostDodging:
		{
			if (bIsFocused)
				overrideVelocity = GetFocusDodgeDirection() * GetCurveValue("Speed");
			else
				overrideVelocity = mAccelerationDirection * GetCurveValue("Speed");
			movementComp->Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, Z);
			break;
		}
		case EAttackState::BeingHit:
		case EAttackState::BeingStuned:
		{
			overrideVelocity = damageMomentum * GetCurveValue("Speed");;
			movementComp->Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, Z);
			break;
		}
		}
	}

	///** Update controller rotation and camera rotation lag during focus
	if (bIsFocused)
	{
		mCharacter->GetCameraBoom()->CameraRotationLagSpeed = mCharacter->GetLockOnCameraRotationLag();
		FRotator unFocus = mCharacter->GetController()->GetControlRotation();

		if (mCharacter->GetHeroStatsComp()->GetCurrentTarget())
		{
			FRotator Focus
				= UKismetMathLibrary::FindLookAtRotation(
					mCharacter->GetFollowCamera()->GetComponentLocation(),
					mCharacter->GetHeroStatsComp()->GetCurrentTarget()->GetActorLocation());
			Focus.Roll = unFocus.Roll;
			Focus.Pitch = unFocus.Pitch;
			mCharacter->GetController()->SetControlRotation(Focus);
		}
		else
		{
			UE_LOG(LogTemp, Warning,
				TEXT("Melee Hero has null target during focus mode, this may caused by focus exit pending"));
		}
	}
	else
	{
		mCharacter->GetCameraBoom()->CameraRotationLagSpeed
			= UKismetMathLibrary::MapRangeClamped(FMath::Abs(turn),
				0, 90.0f,
				mCharacter->GetLockOnCameraRotationLag(),
				mCharacter->GetNonLockOnCameraRotationLag());
	}

	///** disable head track during slash someone
	if (AttackState == EAttackState::None)
		HeadTrackAlpha += 0.05f;
	else
		HeadTrackAlpha -= 0.05f;
	HeadTrackAlpha = FMath::Clamp(HeadTrackAlpha, 0.0f, 1.0f);


	///** determine spine track angle and disable spine track if we are not in attacking frame
	bool isNotAttackFrame = AttackState == EAttackState::None ||
		AttackState == EAttackState::Dodging ||
		AttackState == EAttackState::PostDodging;

	if (isNotAttackFrame)
		spineAngleOverrideAlpha -= 0.05f;
	else
		spineAngleOverrideAlpha += 0.05f;

	spineAngleOverrideAlpha = FMath::Clamp(spineAngleOverrideAlpha, 0.0f, .8f);
	spineAngleRotator = FRotator(0, 0, -15.f - HeadTrackPitch);
	CurrentDefendPoseAlpha = FMath::FInterpTo(CurrentDefendPoseAlpha, TargetDefendPoseAlpha, _deltaTime, 15.0f);

	
	// Set camera back to origional, if it is not in command mode
	if (mCharacter->IsInCommandMode())
		mCharacter->GetFollowCamera()->RelativeLocation
		= UKismetMathLibrary::VInterpTo(mCharacter->GetFollowCamera()->RelativeLocation, CameraEquipOffset, _deltaTime, CameraShiftRate);
	else
		mCharacter->GetFollowCamera()->RelativeLocation
		= UKismetMathLibrary::VInterpTo(mCharacter->GetFollowCamera()->RelativeLocation, FVector::ZeroVector, _deltaTime, CameraShiftRate);

}

void UHero_AnimInstance::MeleeRotateRateAndMaxSpeedUpdate(UCharacterMovementComponent * movementComp)
{
	// the more of the angle between forward vector and acceleration, the more rotation speed
	if (ActivatedEquipment == EEquipType::Travel)
	{
		movementComp->JumpZVelocity = JumpZ_Travel;
		movementComp->RotationRate.Yaw
			= UKismetMathLibrary::MapRangeClamped(FMath::Abs(turn), 0, 180.0f,
				mCharacter->GetMinTurnRateForTravel(),
				mCharacter->GetMaxTurnRateForTravel());

		if (bIsSprinting)
			mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetSprintSpeed();
		else
			mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetJogSpeed();
	}
	else
	{
		if (AttackState == EAttackState::PreWinding)
			movementComp->RotationRate.Yaw = mCharacter->GetMaxTurnRateForCombat();
		else
			movementComp->RotationRate.Yaw
			= UKismetMathLibrary::MapRangeClamped(FMath::Abs(turn), 0, 180.0f,
				mCharacter->GetMinTurnRateForCombat(),
				mCharacter->GetMaxTurnRateForCombat());

		switch (CurrentEquipment)
		{
		case EEquipType::ShieldSword:
			movementComp->JumpZVelocity = JumpZ_Sns;
			if (bOnDefend)
			{
				mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetWalkSpeed() + 50.0f;
				MoveForwardAxis *= 0.5f;
				MoveRightAxis *= 0.5f;
			}
			else
			{
				mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetJogSpeed();
			}
			break;
		case EEquipType::HeavyWeapon:
			movementComp->JumpZVelocity = JumpZ_HV;
			if (bIsSprinting)
				mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetJogSpeed() - 100.0f;
			else
			{
				mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetWalkSpeed();
				MoveForwardAxis *= 0.5f;
				MoveRightAxis *= 0.5f;
			}
			break;
		case EEquipType::TwoHandSword:
			movementComp->JumpZVelocity = JumpZ_Katana;
			if (bIsSprinting && MoveForwardAxis > 0 && MoveRightAxis == 0)
			{
				mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetSprintSpeed();
				MoveForwardAxis *= 2.0f;
			}
			else
				mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetJogSpeed();
			break;
		default:
			break;
		}
	}

}

void UHero_AnimInstance::RangeUpdate(UCharacterMovementComponent* movementComp, float _deltaTime)
{
	movementComp->RotationRate.Yaw
		= UKismetMathLibrary::MapRangeClamped(FMath::Abs(turn), 0, 180.0f,
			mCharacter->GetMinTurnRateForTravel(),
			mCharacter->GetMaxTurnRateForTravel());

	if (ActivatedEquipment == EEquipType::Travel)
	{

		mCharacter->GetFollowCamera()->RelativeLocation
			= UKismetMathLibrary::VInterpTo(mCharacter->GetFollowCamera()->RelativeLocation, FVector::ZeroVector, _deltaTime, CameraShiftRate);
		if (bIsSprinting)
			movementComp->MaxWalkSpeed = mCharacter->GetSprintSpeed();
		else
			movementComp->MaxWalkSpeed = mCharacter->GetJogSpeed();
	}
	else
	{
		if (bTryToZoomIn)
		{
			// walk in zoom in mode
			if (!bIsSprinting)
			{
				MoveForwardAxis *= 0.5f;
				MoveRightAxis *= 0.5f;
				movementComp->MaxWalkSpeed = mCharacter->GetWalkSpeed();
			}
			else
				movementComp->MaxWalkSpeed = mCharacter->GetJogSpeed();

			mCharacter->GetFollowCamera()->RelativeLocation
				= UKismetMathLibrary::VInterpTo(mCharacter->GetFollowCamera()->RelativeLocation, CameraZoomOffset, _deltaTime, CameraZoomInRate);
		}
		else
		{
			// Sprinting mode when not aiming
			if (bIsSprinting && MoveRightAxis == 0 && MoveForwardAxis > 0)
			{
				movementComp->MaxWalkSpeed = mCharacter->GetSprintSpeed();
				MoveForwardAxis *= 2.0f;
			}
			else
				movementComp->MaxWalkSpeed = mCharacter->GetJogSpeed();


			mCharacter->GetFollowCamera()->RelativeLocation
				= UKismetMathLibrary::VInterpTo(mCharacter->GetFollowCamera()->RelativeLocation, CameraEquipOffset, _deltaTime, CameraShiftRate);
		}
	}


	if (bVelocityOverrideByAnim)
	{
		float Z = movementComp->Velocity.Z;
		FVector overrideVelocity;
		overrideVelocity = mCharacter->GetActorForwardVector() * GetCurveValue("Speed");
		movementComp->Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, Z);

		if (AttackState == EAttackState::BeingHit)
		{
			overrideVelocity = damageMomentum * GetCurveValue("Speed");;
			movementComp->Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, Z);
		}
	}

}

void UHero_AnimInstance::OnPostEvaluate()
{
}

void UHero_AnimInstance::FxMeleeSwing(bool _rightHand)
{
	if (AttackState == EAttackState::BeingHit)
	{
		return;
	}

	Super::FxMeleeSwing(_rightHand);
}

void UHero_AnimInstance::OnAutoFire()
{
	if (AttackState == EAttackState::BeingHit || AttackState == EAttackState::BeingStuned)
		return;
	UHeroStatsComponent* heroStats = mCharacter->GetHeroStatsComp();
	if (heroStats)
	{
		ARangeWeapon* rangeWeapon = Cast<ARangeWeapon>(heroStats->GetCurrentRightHandWeapon());

		if (rangeWeapon == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Current Weapon is not a range weapon - UHero_AnimInstance::OnAutoFire"));
			return;
		}

		rangeWeapon->ComboFire();
	}
}

#pragma endregion

void UHero_AnimInstance::OnMontageStartHandle(UAnimMontage * _animMontage)
{
}

void UHero_AnimInstance::OnMontageBlendOutStartHandle(UAnimMontage * _animMontage, bool _bInterruptted)
{

	Super::OnMontageBlendOutStartHandle(_animMontage, _bInterruptted);
	if (_animMontage == Attack_Montage && !_bInterruptted)
	{
		ResetCombo();
	}
	else if (_animMontage == Dodge_Montage)
	{
		OnDodgeFinish(_bInterruptted);
	}
	else if (_animMontage == Hit_Montage)
	{
		if (AttackState == EAttackState::BeingStuned && !_bInterruptted)
			PlayMontage(AM_Dazed, 1.2f);
		else
			RecoverFromBeingHit(_bInterruptted);
	}
	else if (_animMontage == AM_Dazed)
	{
		RecoverFromBeingHit(_bInterruptted);
	}
	else if (_animMontage == CounterAttack_Montage )
	{
		bAnimInterruptRobust = false;
		// recover movement from melee attack
		if (!_bInterruptted)
		{
			//bDuringGainDpAttack = false;
			SkillBuff = ESkillBuff::None;
			ResetCombo();
		}
	}
	else if (_animMontage == Skill_Montage)
	{
		bAnimInterruptRobust = false;
		SkillBuff = ESkillBuff::None;

		//bDuringGainDpAttack = false;

		// recover movement from melee attack
		if (!_bInterruptted)
		{
			ResetCombo();
		}
	}
}

void UHero_AnimInstance::OnMiddleMouseButtonPressed()
{
	// Handled by child class
	switch (CurrentEquipment)
	{
	case EEquipType::ShieldSword:
	case EEquipType::TwoHandSword:
	case EEquipType::HeavyWeapon:
	{
		if (mCharacter)
		{
			UHeroStatsComponent* heroStats = mCharacter->GetHeroStatsComp();

			if (heroStats)
			{
				heroStats->OnFocus();
				OnFocus();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("heroStats is Null - UMeleeHero_AnimInstance::OnMiddleMouseButtonPressed "));
			}
		}
	}

		break;
	case EEquipType::CrossBow:
		break;
	default:
		break;
	}
}

void UHero_AnimInstance::OnCorLAltPressed()
{
	switch (CurrentEquipment)
	{
	case EEquipType::ShieldSword:
	case EEquipType::TwoHandSword:
	case EEquipType::HeavyWeapon:
		OnDodge();
		break;
	case EEquipType::CrossBow:
		break;
	default:
		break;
	}
}

void UHero_AnimInstance::OnRightMouseButtonPressed()
{
	switch (CurrentEquipment)
	{
	case EEquipType::ShieldSword:
		OnDefendOn_Sns();
		break;
	case EEquipType::TwoHandSword:
	case EEquipType::HeavyWeapon:
		OnDefendOn_Dh();
		break;
	case EEquipType::CrossBow:
		OnZoomIn();
		break;
	default:
		break;
	}
}

void UHero_AnimInstance::OnRightMouseButtonReleased()
{
	switch (CurrentEquipment)
	{
	case EEquipType::ShieldSword:
	case EEquipType::TwoHandSword:
	case EEquipType::HeavyWeapon:
		OnDefendOff();
		break;
	default:
		break;
	case EEquipType::CrossBow:
		OnZoomOut();
		break;
	}

}

void UHero_AnimInstance::OnSprintPressed()
{
	bTryToSprint = true;

	bool ableToSprint = !bIsInAir;

	if (ableToSprint)
	{
		//mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetSprintSpeed();
		bIsSprinting = true;
	}
}

void UHero_AnimInstance::OnSprintReleased()
{
	bTryToSprint = false;

	//mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetJogSpeed();;
	bIsSprinting = false;
}

void UHero_AnimInstance::StartOverrideSpeed()
{
	if (ActivatedEquipment == EEquipType::Travel && bTryToEquip == false)
		bVelocityOverrideByAnim = true;
}

void UHero_AnimInstance::StopOverrideSpeed()
{

	bool ignore = 
		AttackState == EAttackState::Dodging || 
		AttackState == EAttackState::Attacking ||
		AttackState == EAttackState::PreWinding;
	if (ignore)
		return;	

	bVelocityOverrideByAnim = false;
	mCharacter->GetCharacterMovement()->MaxWalkSpeed 
		= (bIsSprinting)? mCharacter->GetSprintSpeed() : mCharacter->GetJogSpeed();
}

void UHero_AnimInstance::OnFocus()
{

	// if we dont have a target at all, why bother to focus, right?
	if (mCharacter->GetHeroStatsComp()->GetCurrentTarget() == nullptr)
	{
		// Toggle the focus off, the hero stats comp will clear the current target, if we are currently dodging, we need to set
		// this focus exit to pending
		if (AttackState == EAttackState::Dodging || AttackState == EAttackState::PostDodging)
			bIsFocusExitPending = true;
		else
			ToggleFocusMode(false);
		return;
	}

	// how can we enter the focus mode without get our blade on, right? Focus always false in Travel
	if (ActivatedEquipment == EEquipType::Travel)
	{
		// Equip, play animation
		UE_LOG(LogTemp, Warning, TEXT("Focus during travel, equip is implemented automatically"));
		OnEquip();
	}

	// Toggle focus mode


	if (bIsFocused == false)
	{
		// if we are going to focus during dodging, set focus to pending, and enter focus when this dodging is finished
		if (AttackState == EAttackState::Dodging || AttackState == EAttackState::PostDodging)
			bIsFocusEnterPending = true;
		else
			ToggleFocusMode(true);

	}
	else
	{
		// this is for using dodge quit out of enemy detector
		if (AttackState == EAttackState::Dodging || AttackState == EAttackState::PostDodging)
			bIsFocusExitPending = true;
		else
			ToggleFocusMode(false);
	}

}

void UHero_AnimInstance::ToggleFocusMode(bool _IsOn)
{
	bIsFocused = _IsOn;
	mCharacter->bIsYawControllDisabled = bIsFocused;

	if (ActivatedEquipment == EEquipType::Travel)
	{
		mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = _IsOn;
		mCharacter->GetCharacterMovement()->bOrientRotationToMovement = !_IsOn;
	}
	else
	{
		if (AttackState == EAttackState::Attacking || AttackState == EAttackState::ReadyForNext)
		{
			mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = _IsOn;
			mCharacter->GetCharacterMovement()->bOrientRotationToMovement = !_IsOn;
		}
	}
	bIsFocusEnterPending = false;
	bIsFocusExitPending = false;
}


#pragma region Equip && Unequip


bool UHero_AnimInstance::OnEquip()
{
	bool ignore
		= AttackState != EAttackState::None
		|| Montage_IsPlaying(Equip_Montage)
		|| Equip_Montage == nullptr;

	if (ignore)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Equip Function is ignore due to wrong attack state, running montage or null Montage"));
		return false;
	}
	else
	{
		// Equip
		if (ActivatedEquipment != CurrentEquipment)
		{
			//DisableJump();
			bVelocityOverrideByAnim = false;
			bTryToEquip = true;
		}
		// UnEquip
		else
		{
			/** No need to be in focus mode, if we already sheath our weapon*/
			if (bIsFocused)
			{
				OnFocus();
				UE_LOG(LogTemp, Warning, TEXT("Unequip during focus, unfocus is implemented automatically"));
			}
			bTryToEquip = false;
			bTryToZoomIn = false;
		}
		/** play animation montage based on CurrentEquipment */
		switch (CurrentEquipment)
		{
		case EEquipType::ShieldSword:
		{
			if (ActivatedEquipment == EEquipType::Travel)
				// Equip
				this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_SnsEquip);
			else if (ActivatedEquipment == CurrentEquipment)
				// Unequip
				this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_SnsUnequip);

			break;

		}
		case EEquipType::TwoHandSword:
		{
			if (ActivatedEquipment == EEquipType::Travel)
				// Equip
				this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_DHEquip);
			else if (ActivatedEquipment == CurrentEquipment)
				// Unequip
				this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_DHUnequip);
			break;

		}
		case EEquipType::HeavyWeapon:
		{
			if (ActivatedEquipment == EEquipType::Travel)
				// Equip
				this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_HVEquip);
			else if (ActivatedEquipment == CurrentEquipment)
				// Unequip
				this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_HVUnequip);
			break;

		}
		case EEquipType::CrossBow:
		{
			if (ActivatedEquipment == EEquipType::Travel)
			{
				// Equip
				mCharacter->ToggleFireMode(true);
				this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_CBEquip);
			}
			else if (ActivatedEquipment == CurrentEquipment)
			{
				// Unequip
				mCharacter->ToggleFireMode(false);
				this->PlayMontage(Equip_Montage, 1.0f, MONTAGE_SN_HERO_CBUnequip);
			}
			break;
		}
		}
		return true;
	}

}

void UHero_AnimInstance::OnSwapBetweenMeleeAndRange()
{

	UHeroStatsComponent* heroStats = mCharacter->GetHeroStatsComp();
	bool ignore
		= AttackState != EAttackState::None
		|| Montage_IsPlaying(Equip_Montage)
		|| Equip_Montage == nullptr || heroStats == nullptr;

	if (ignore)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("OnSwapBetweenMeleeAndRange is ignore due to wrong attack state, running montage, null Montage or heroStats"));
		return;
	}

	
	bool success = mCharacter->GetHeroStatsComp()->OnSwapBetweenMeleeAndRange();
	if (success)
	{
		OnChangeWeaponFinsh();
	}


}

void UHero_AnimInstance::OnChangeWeapon(EEquipType _next)
{

	UHeroStatsComponent* heroStats = mCharacter->GetHeroStatsComp();
	bool ignore
		= AttackState != EAttackState::None
		|| Montage_IsPlaying(Equip_Montage) 
		|| (_next == ActivatedEquipment)
		|| Equip_Montage == nullptr 
		|| heroStats == nullptr;

	if (ignore)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("OnChangeWeapon is ignore due to wrong attack state, running montage, null Montage or heroStats"));
		return;
	}

	bool inventoryCheck = heroStats->OnSwitchWeapon(_next);

	if (inventoryCheck)
	{
		OnChangeWeaponFinsh();
	}

}

void UHero_AnimInstance::OnChangeWeaponFinsh()
{
	if (ActivatedEquipment == EEquipType::Travel)
	{
		bVelocityOverrideByAnim = false;
		bTryToEquip = true;
	}
	else if (ActivatedEquipment == EEquipType::CrossBow)
	{
		bTryToZoomIn = false;
	}


	// Check the current main weapon after swap, update the combo array
	AGear* mainWeapon = mCharacter->GetHeroStatsComp()->GetCurrentRightHandWeapon();

	FName sectionToPlay;

	switch (mainWeapon->GetGearType())
	{
	case EGearType::LongSword:
	case EGearType::WarAxe:
	case EGearType::Mace:
		CurrentEquipment = EEquipType::ShieldSword;
		sectionToPlay = MONTAGE_SN_HERO_SnsEquip;
		break;
	case EGearType::DoubleHandWeapon:
		CurrentEquipment = EEquipType::TwoHandSword;
		sectionToPlay = MONTAGE_SN_HERO_DHEquip;
		break;
	case EGearType::BattleAxe:
	case EGearType::Hammer:
	case EGearType::GreatSword:
		CurrentEquipment = EEquipType::HeavyWeapon;
		sectionToPlay = MONTAGE_SN_HERO_HVEquip;
		break;
	case EGearType::CrossBow:
	{
		CurrentEquipment = EEquipType::CrossBow;
		if (bIsFocused)
			OnFocus();
		sectionToPlay = MONTAGE_SN_HERO_CBEquip;
		break;
	}
	}
	UpdateAnimationSetOnWeaponChange(mainWeapon->GetGearType());

	// play EquipAnimation?
	this->PlayMontage(Equip_Montage, 1.0f, sectionToPlay);
}

// Draw weapon
void UHero_AnimInstance::OnEquipWeapon(bool _playSfx)
{
	ActivatedEquipment = CurrentEquipment;
	mCharacter->GetHeroStatsComp()->OnEquipWeapon();
	mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	if (_playSfx)
		FxOnDraw();
}

// Sheath Weapon
void UHero_AnimInstance::OnSheathWeapon()
{
	ActivatedEquipment = EEquipType::Travel;
	mCharacter->GetHeroStatsComp()->OnSheathWeapon();

	// After we sheath our weapon, and the sprint button is not release, we will restore sprinting
	if (bTryToSprint)
	{
		mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetSprintSpeed();
		bIsSprinting = true;
	}
	mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	mCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	FxOnEquip();

	// if we have encharted weapon, end encharted
	if (mCharacter->HasEnchartedWeapon())
	{
		mCharacter->OnWeaponEnchantStop();
	}
}

/** Skip equipAnimation direction to equip mode, change the movement role to strafe*/
void UHero_AnimInstance::SkipEquip()
{
	OnEquipWeapon(false);
	bVelocityOverrideByAnim = false;
}

void UHero_AnimInstance::AttachWeapon()
{
	ActivatedEquipment = CurrentEquipment;
	mCharacter->GetHeroStatsComp()->OnEquipWeapon();
	bVelocityOverrideByAnim = false;
}


#pragma endregion

#pragma region Jump

void UHero_AnimInstance::EnableJump()
{
	bEnableJump = true;
}

void UHero_AnimInstance::DisableJump()
{
	bEnableJump = false;
}

void UHero_AnimInstance::OnJumpStart()
{
	if (bEnableJump)
	{
		if (AttackState == EAttackState::None)
		{
			bTryToJump = true;
			mCharacter->Jump();
		}
	}
}

void UHero_AnimInstance::OnJumpStop()
{
	bTryToJump = false;
	mCharacter->StopJumping();
}

#pragma endregion

#pragma region Attack
void UHero_AnimInstance::OnAttack()
{

	switch (CurrentEquipment)
	{
	case EEquipType::CrossBow:
		OnRangeAttack();
		break;
	case EEquipType::ShieldSword:
	case EEquipType::TwoHandSword:
	case EEquipType::HeavyWeapon:
		OnMeleeAttack();
		break;
	}

}

void UHero_AnimInstance::OnMeleeAttack()
{
	bool ignore =
		Montage_IsPlaying(Hit_Montage)
		|| (bIsInAir)
		|| Attack_Montage == nullptr;

	// Apply Input Filter
	if (ignore)
	{
		UE_LOG(LogTemp, Warning, TEXT("Abort - UHero_AnimInstance::OnAttack"));
		return;
	}


	// equip weapon to weapon socket if we havent
	if (ActivatedEquipment == EEquipType::Travel)
		SkipEquip();

	switch (AttackState)
	{
	case EAttackState::None:
	case EAttackState::ReadyForNext:
	case EAttackState::PostDodging:
	{
		// trigger the attack immediately, clear the next action marker
		LaunchCombo();
		return;
	}
	case EAttackState::Attacking:
	case EAttackState::Dodging:
	{
		// catch attack action, update next action marker, 
		// and wait if not being updated by later input
		//UE_LOG(LogTemp, Warning, TEXT("catch attack action, update next action marker"));
		NextAction = EActionType::Attack;
		return;
	}
	case EAttackState::PreWinding:
	default:
		//UE_LOG(LogTemp, Warning, TEXT("current attack state ignore attack action"));
		return;
	}

}

bool UHero_AnimInstance::OnCounterAttack(const FVector & _damageCauserRelative)
{

	if (CounterAttack_Montage == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("CounterAttack_Montage == nullptr - UHero_AnimInstance::OnCounterAttack"));
		return false;
	}


	//// Check if we are in defence mode
	//if (!bOnDefend)
	//	return false;
	//// Check Stanima
	//if (mCharacter->CounterAttackSpCheck() == false)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("No Enought Stamina - UHero_AnimInstance::LaunchCounterAttack"));
	//	return false;
	//}

	// Implement Counter Attack

	float dotFront = FVector::DotProduct(mCharacter->GetActorForwardVector(), _damageCauserRelative);

	// Check direction, Sns Has more counter attack chance 
	FName sectionName;
	switch (CurrentEquipment)
	{
	case EEquipType::ShieldSword:
		if (dotFront < 0.5f)
			return false;
		else
		{

			if (dotFront > 0.99f)
			{
				sectionName = MONTAGE_SN_CA_SnsFront;
			}
			else
			{
				float dotRight = FVector::DotProduct(mCharacter->GetActorRightVector(), _damageCauserRelative);
				if (dotRight > 0)
					sectionName = MONTAGE_SN_CA_SnsRight;
				else
					sectionName = MONTAGE_SN_CA_SnsLeft;
			}
			LaunchCounterAttack(sectionName);
			return true;
		}
	case EEquipType::TwoHandSword:
		if (dotFront < 0.7f)
			return false;
		else
		{
			sectionName = MONTAGE_SN_CA_Katana;
			LaunchCounterAttack(sectionName);
			return true;
		}
	case EEquipType::HeavyWeapon:
		if (dotFront < 0.7f)
			return false;
		else
		{
			AGear* currentWeapon = mCharacter->GetCurrentWeapon();
			if (currentWeapon)
			{
				if (currentWeapon->GetGearType() == EGearType::BattleAxe)
				{
					sectionName = MONTAGE_SN_CA_BattleAxe;
					LaunchCounterAttack(sectionName);
					return true;
				}
				else 
				{
					sectionName = MONTAGE_SN_CA_Hammer;
					float rate = (currentWeapon->GetGearType() == EGearType::GreatSword) ? GreatSwordAnimRate : 1.0f;
					LaunchCounterAttack(sectionName, rate);
					return true;
				}
			}
			return false;
		}
	default:
		return false;

	}
}

void UHero_AnimInstance::OnDefendOn_Sns()
{

	if (ActivatedEquipment == EEquipType::Travel)
	{
		OnEquip();
		return;
	}

	bool wrongState = AttackState == EAttackState::Dodging 
		|| AttackState == EAttackState::BeingStuned
		|| Montage_IsPlaying(CounterAttack_Montage);
	if (wrongState)
		return;


	if ((AttackState == EAttackState::ReadyForNext || AttackState == EAttackState::PreWinding) && Montage_IsPlaying(Attack_Montage))
	{
		// when interrupt combo, stop combo animation, and reset combo
		Montage_Stop(0.2f);
		ResetCombo();
	}
	else if (AttackState == EAttackState::PostDodging)
	{
		OnDodgeFinish(false);
	}
	else if (AttackState == EAttackState::BeingHit)
	{
		RecoverFromBeingHit(false);
	}


	bOnDefend = true;
	TargetDefendPoseAlpha = 1.0f;
}

void UHero_AnimInstance::OnDefendOn_Dh()
{

	if (ActivatedEquipment == EEquipType::Travel)
	{
		OnEquip();
		return;
	}


	// Condition Check
	bool ignore = MoveRightAxis != 0.0f || MoveForwardAxis != 0.0f;
	if (ignore)
		return;

	bool wrongState = AttackState == EAttackState::BeingHit ||
		AttackState == EAttackState::BeingStuned ||
		AttackState == EAttackState::Attacking ||
		AttackState == EAttackState::Dodging ||
		Montage_IsPlaying(CounterAttack_Montage);
	if (wrongState)
		return;


	if (AttackState == EAttackState::ReadyForNext || AttackState == EAttackState::PreWinding)
	{
		// when interrupt combo, stop combo animation, and reset combo
		Montage_Stop(0.2f);
		ResetCombo();
	}
	else if (AttackState == EAttackState::PostDodging)
	{
		OnDodgeFinish(false);
	}

	bOnDefend = true;
	TargetDefendPoseAlpha = 1.0f;
}

void UHero_AnimInstance::OnDefendOff()
{
	bOnDefend = false;
	TargetDefendPoseAlpha = 0.0f;
}

void UHero_AnimInstance::LaunchCounterAttack(const FName & _sectionName, float _rate)
{
	//UE_LOG(LogTemp, Log, TEXT(" UHero_AnimInstance::LaunchCounterAttack"));

	this->PlayMontage(CounterAttack_Montage, _rate, _sectionName);
	bVelocityOverrideByAnim = true;

	if (!bIsFocused)
	{
		mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	AttackState = EAttackState::PreWinding;
	NextAction = EActionType::None;

	// Counter attack will not be interrupted by hit response
	bAnimInterruptRobust = true;	
	// reset defend pose
	OnDefendOff();
}

void UHero_AnimInstance::OnSkill(int _skillIndex)
{
	// Apply Input Filter
	bool ignore =
		(bIsInAir)
		|| Skill_Montage == nullptr; //	Montage_IsPlaying(Hit_Montage)

	if (ignore)
	{
		UE_LOG(LogTemp, Warning, TEXT("Abort - UHero_AnimInstance::OnSkill_0"));
		return;
	}

	// equip weapon to weapon socket if we havent
	if (ActivatedEquipment == EEquipType::Travel)
	{
		OnEquip();
		return;

	}
	//SkipEquip();

	switch (AttackState)
	{
	case EAttackState::None:
	case EAttackState::ReadyForNext:
	case EAttackState::PostDodging:
	case EAttackState::BeingHit:
	{
		// trigger the skill immediately, clear the next action marker
		LaunchSkill(_skillIndex);
		return;
	}
	case EAttackState::Attacking:
	case EAttackState::Dodging:
	{
		// catch action, update next action marker, 
		// and wait if not being updated by later input
		//UE_LOG(LogTemp, Warning, TEXT("catch attack action, update next action marker"));
		NextAction = EActionType(_skillIndex + 2);
		return;
	}
	case EAttackState::PreWinding:
	default:
		//UE_LOG(LogTemp, Warning, TEXT("current attack state ignore attack action"));
		return;
	}

}

void UHero_AnimInstance::LaunchSkill(int _skillIndex)
{
	if (CurrentEquipment == EEquipType::CrossBow && (CanNotShoot()||_skillIndex == Skill__WeaponCastingFire))
			return;

	// For now we dont have anything for CrossBow Yet

	if (mCharacter->SkillCheck(_skillIndex) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Enough Stamina - UHero_AnimInstance::LaunchSkill"));
		return;
	}

	// Quit Defence Mode
	OnDefendOff();


	// Skill can launch during being hit, if we are being hit, we will get a recover from that
	if (AttackState == EAttackState::BeingHit)
		RecoverFromBeingHit(false);


	FName sectionToPlay = mCharacter->GetSkillSectionNameAt(_skillIndex);



	// Play partical on hero
	mCharacter->OnPlaySkillParticle(_skillIndex);

	float attackSpeed = (mCharacter->GetCurrentWeapon()->GetGearType() == EGearType::GreatSword) ? 1.1f : 1.0f;

	switch (_skillIndex)
	{
	case Skill__PowerHit:
	{
		switch (CurrentEquipment)
		{
		case EEquipType::ShieldSword:
			SkillBuff = ESkillBuff::GainDp;
			break;
		case EEquipType::TwoHandSword:
			SkillBuff = ESkillBuff::MultiDamage;
			break;
		case EEquipType::HeavyWeapon:
			SkillBuff = ESkillBuff::UnStoppable;
			break;

		case EEquipType::CrossBow:
		{		
			mCharacter->PowerShot();
			this->PlayMontage(Fire_Montage, 1.0f, sectionToPlay);
			return;
		}
		default:
		case EEquipType::Travel:
		{
			SkillBuff = ESkillBuff::None;
			return;
		}
		}
		break;
	}
	case Skill__Combo:
	{
		if (CurrentEquipment == EEquipType::CrossBow)
		{
			this->PlayMontage(Fire_Montage, 1.0f, sectionToPlay);
			return;
		}
		else
			SkillBuff = ESkillBuff::Combo;
	}
		break;
	case Skill__Heal:
		mCharacter->OnHealSkillCastSuccess();
		break;
	case Skill__WeaponCastingFire:
		break;
	default:
		break;
	}

	//bDuringGainDpAttack = true;

	AttackState = EAttackState::PreWinding;
	NextAction = EActionType::None;

	this->PlayMontage(Skill_Montage, attackSpeed, sectionToPlay);
	bVelocityOverrideByAnim = true;
	bAnimInterruptRobust = true;

	if (!bIsFocused)
	{
		mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	}

}

void UHero_AnimInstance::OnRangeAttack()
{
	// Condition Check
	bool ignore = Montage_IsPlaying(Hit_Montage) ||
		Montage_IsPlaying(Fire_Montage) || 
		Fire_Montage == nullptr || mCharacter == nullptr;

	if (ignore)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fire Ignore - URangeHero_AnimInstance::OnAttack"));
		return;
	}

	// If we are unarmed, equip our weapon
	if (ActivatedEquipment == EEquipType::Travel)
	{
		OnEquip();
		return;
	}

	UHeroStatsComponent* heroStats = mCharacter->GetHeroStatsComp();
	if (heroStats)
	{
		ARangeWeapon* rangeWeapon = Cast<ARangeWeapon>(heroStats->GetCurrentRightHandWeapon());

		if (rangeWeapon == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Current Weapon is not a range weapon - UHero_AnimInstance::OnRangeAttack"));
			return;
		}

		if (CanNotShoot())
			return;

		rangeWeapon->Fire();
		PlayMontage(Fire_Montage, 1.0f, MONTAGE_CB_FireOnce);

	}

}

#pragma region Fire

void UHero_AnimInstance::OnZoomIn()
{
	if (ActivatedEquipment == EEquipType::Travel)
	{
		OnEquip();
	}

	bTryToZoomIn = true;
}

void UHero_AnimInstance::OnZoomOut()
{
	if (ActivatedEquipment == EEquipType::Travel)
		return;

	bTryToZoomIn = false;
}

#pragma endregion

#pragma region  Combo Attack

void UHero_AnimInstance::LaunchCombo()
{

	// Stamina Check
	if (mCharacter->MeleeAttackSpCheck() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Enought Stamina - UHero_AnimInstance::LaunchCombo"));
		return;
	}

	// Quit Defence Mode
	OnDefendOff();


	if (CurrentComboIndex >= Current_AttackSectionName->Num())
	{
		// start over
		UE_LOG(LogTemp, Warning, TEXT("mCurrentComboIndex: %d, reach max Combo Index: %d, reset "), CurrentComboIndex, Current_AttackSectionName->Num());
		CurrentComboIndex = 0;
	}

	float attackSpeed = (mCharacter->GetCurrentWeapon()->GetGearType() == EGearType::GreatSword) ? GreatSwordAnimRate : 1.0f;
	this->PlayMontage(Attack_Montage, attackSpeed, (*Current_AttackSectionName)[CurrentComboIndex]);
	bVelocityOverrideByAnim = true;

	if (!bIsFocused)
	{
		mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	AttackState = EAttackState::PreWinding;
	NextAction = EActionType::None;
	CurrentComboIndex++;
}

void UHero_AnimInstance::ResetCombo()
{
	//UE_LOG(LogTemp, Log, TEXT("Reset Combo"));
	AttackState = EAttackState::None;
	NextAction = EActionType::None;

	if (AttackState == EAttackState::Dodging)
		return;
	CurrentComboIndex = 0;
	bVelocityOverrideByAnim = false;

	if (!bIsFocused)
	{
		mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	//mCharacter->GetMovementComponent()->Velocity = mAccelerationDirection;
}

void UHero_AnimInstance::OnEnableDamage(bool bIsright, bool bIsAll)
{
	bool rightState = AttackState == EAttackState::PreWinding || AttackState == EAttackState::Attacking;
	if (mCharacter && rightState)
	{
		AttackState = EAttackState::Attacking;
		mCharacter->GetHeroStatsComp()->SetEnableWeapon(true, bIsright, bIsAll);

		//Lock the character rotation during attacking if it is not in focus mode
		if (!bIsFocused)
		{
			mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
			mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}
}

void UHero_AnimInstance::OnDisableDamage(bool bIsright, bool bIsAll)
{
	//Super::OnDisableWeapon(bIsright, bIsAll);
	mCharacter->GetHeroStatsComp()->SetEnableWeapon(false, bIsright, bIsAll);
}

void UHero_AnimInstance::OnNextAttack()
{
	switch (NextAction)
	{
	case EActionType::None:
	default:
		// We did not catch any action, we put ourself to wait
		// and recover the rotate
		AttackState = EAttackState::ReadyForNext;
		break;
	case EActionType::Attack:
		LaunchCombo();
		break;
	case EActionType::Skill_Combo:
	case EActionType::Skill_PowerHit:
	case EActionType::Skill_WeaponCastingFire:
	case EActionType::Skill_WeaponCastingIce:
	case EActionType::Skill_Taunt:
	case EActionType::Skill_Heal:
	case EActionType::Skill_BattleCommand:
		LaunchSkill((int)NextAction - 2);
		break;
	case EActionType::Dodge:
		LaunchDodge();
		break;
	}

}

#pragma endregion

#pragma endregion

#pragma region Dodge

void UHero_AnimInstance::OnDodge()
{

	bool ignore
		= Montage_IsPlaying(Hit_Montage) ||
		bIsInAir || Dodge_Montage == nullptr;
	//(FMath::Abs(turn) > DodgeMinTurnThreshold && !bIsFocused)

	if (ignore)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Dodge is ignored due to inAir or Montage Null"));
		return;
	}

	// if we havnt equip our weapon, skip animation and attach weapon to weapon socket
	if (ActivatedEquipment == EEquipType::Travel)
		SkipEquip();

	switch (AttackState)
	{
	case EAttackState::None:
	case EAttackState::PreWinding:
	case EAttackState::ReadyForNext:
	{
		// Trigger the action immeditately, clear the next action marker
		LaunchDodge();
		return;
	}

	case EAttackState::Attacking:
	case EAttackState::PostDodging:
	{
		// Update the next action marker,
		// waiting until the current action is finished, 
		// if it is not be update by later input 
		UE_LOG(LogTemp, Warning, TEXT("Catch continuous Dodge"));
		NextAction = EActionType::Dodge;
		return;
	}
	case EAttackState::Dodging:
	default:
	{
		// Ignore
		return;
	}
	}
}
void UHero_AnimInstance::LaunchDodge()
{
	// Stamina Check
	if (mCharacter->DodgeSpCheck() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Enought Stamina - UHero_AnimInstance::LaunchDodge"));
		return;
	}

	// Quit Defence Mode
	OnDefendOff();

	// Update State
	AttackState = EAttackState::Dodging;
	// Clear Next Action Marker
	NextAction = EActionType::None;

	bVelocityOverrideByAnim = true;
	CurrentComboIndex = 0;


	if (!bIsFocused)
	{
		// when we are dodging while not in focus, we only dodge forward, in the direction
		// of our current movement acceleration
		mCharacter->bUsePreviousMovementAxis = true;
		mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
		mCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		this->PlayMontage(Dodge_Montage, 1.0f, GetForwardDodgeSection());
	}
	else
	{
		// Determine with animation to play by the input,
		// Mark the direction, for movement sync in update 

		FName dodgeSection;
		if (MoveForwardAxis == 0)
		{
			if (MoveRightAxis > 0)
			{
				dodgeSection = GetRightDodgeSection();
				FocusDodgeDirection = EFocusDodgeDirection::Right90;
			}
			else if (MoveRightAxis < 0)
			{
				dodgeSection = GetLeftDodgeSection();
				FocusDodgeDirection = EFocusDodgeDirection::Left90;
			}
			else
			{
				dodgeSection = GetBackDodgeSection();
				FocusDodgeDirection = EFocusDodgeDirection::Back;
			}
		}
		else if (MoveForwardAxis > 0)
		{
			if (MoveRightAxis > 0)
			{
				dodgeSection = GetRightDodgeSection();
				FocusDodgeDirection = EFocusDodgeDirection::Right45;
			}
			else if (MoveRightAxis<0)
			{
				dodgeSection = GetLeftDodgeSection();
				FocusDodgeDirection = EFocusDodgeDirection::Left45;
			}
			else
			{
				dodgeSection = GetForwardDodgeSection();
				FocusDodgeDirection = EFocusDodgeDirection::Forward;
			}
		}
		else
		{
			if (MoveRightAxis > 0)
			{
				dodgeSection = GetRightDodgeSection();
				FocusDodgeDirection = EFocusDodgeDirection::Right135;

			}
			else if (MoveRightAxis<0)
			{
				dodgeSection = GetLeftDodgeSection();
				FocusDodgeDirection = EFocusDodgeDirection::Left135;
			}
			else
			{
				dodgeSection = GetBackDodgeSection();
				FocusDodgeDirection = EFocusDodgeDirection::Back;
			}
		}

		this->PlayMontage(Dodge_Montage, 1.0f, dodgeSection);
	}
}
void UHero_AnimInstance::OnDodgePost()
{
	//Super::OnDodgePost();

	if (FocusDodgeDirection != EFocusDodgeDirection::None)
	{
		mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	// Check Next Action Mark
	switch (NextAction)
	{
	case EActionType::None:
	case EActionType::Dodge:
	default:
		// We did not catch any action, we put ourself to wait
		// speed and rotation still being locked up
		AttackState = EAttackState::PostDodging;
		break;
	case EActionType::Attack:
		LaunchCombo();
		break;
	case EActionType::Skill_Combo:
	case EActionType::Skill_PowerHit:
	case EActionType::Skill_Taunt:
	case EActionType::Skill_WeaponCastingIce:
	case EActionType::Skill_WeaponCastingFire:
	case EActionType::Skill_Heal:
	case EActionType::Skill_BattleCommand:
		LaunchSkill((int)(NextAction)-2);
		break;
	}
}
void UHero_AnimInstance::OnDodgeFinish(bool _bInterruptted)
{
	if (!_bInterruptted)
	{
		AttackState = EAttackState::None;
		bVelocityOverrideByAnim = false;
		FocusDodgeDirection = EFocusDodgeDirection::None;

		if (!bIsFocused)
		{
			mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
			mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}

	if (!bIsFocused)
	{
		mCharacter->bUsePreviousMovementAxis = false;
	}

	// if we have a pending focus, this is a time to do it
	if (bIsFocusEnterPending)
		ToggleFocusMode(true);
	else if (bIsFocusExitPending)
		ToggleFocusMode(false);

}

FName UHero_AnimInstance::GetForwardDodgeSection() const 
{
	switch (CurrentEquipment)
	{
	case EEquipType::ShieldSword:
	default:
		return MONTAGE_SN_HERO_SnsDodgeFwd;
	case EEquipType::TwoHandSword:
		if (bIsFocused)
			return MONTAGE_SN_HERO_KatanaDodgeFwdF;
		else
			return MONTAGE_SN_HERO_KatanaDodgeFwd;
	case EEquipType::HeavyWeapon:
		return MONTAGE_SN_HERO_HVDodgeFwd;
	}
}
FName UHero_AnimInstance::GetRightDodgeSection() const
{
	switch (CurrentEquipment)
	{
	case EEquipType::ShieldSword:
	default:
		return MONTAGE_SN_HERO_SnsDodgeRight;
	case EEquipType::TwoHandSword:
		return MONTAGE_SN_HERO_KatanaDodgeRight;
	case EEquipType::HeavyWeapon:
		return MONTAGE_SN_HERO_HVDodgeRight;
	}
}
FName UHero_AnimInstance::GetLeftDodgeSection() const
{
	switch (CurrentEquipment)
	{
	case EEquipType::ShieldSword:
	default:
		return MONTAGE_SN_HERO_SnsDodgeLeft;
	case EEquipType::TwoHandSword:
		return MONTAGE_SN_HERO_KatanaDodgeLeft;
	case EEquipType::HeavyWeapon:
		return MONTAGE_SN_HERO_HVDodgeLeft;
	}
}

FName UHero_AnimInstance::GetBackDodgeSection() const
{
	switch (CurrentEquipment)
	{
	case EEquipType::ShieldSword:
	default:
		return MONTAGE_SN_HERO_SnsDodgeBwd;
	case EEquipType::TwoHandSword:
		return MONTAGE_SN_HERO_KatanaDodgeBwd;
	case EEquipType::HeavyWeapon:
		return MONTAGE_SN_HERO_HVDodgeBwd;
	}
}

FName UHero_AnimInstance::GetHitResponseSection_SnsCB(FName _boneName, const FVector & _damageCauseRelative, const FVector & _hitLocation)
{
	FName sectionName;

	bool HitFromFront = FVector::DotProduct(mCharacter->GetActorForwardVector(), _damageCauseRelative) > 0.3f;

	if (HitFromFront)
	{
		FVector hitRelative = _hitLocation - GetSkelMeshComponent()->GetSocketLocation(_boneName);
		float hitZOffset = hitRelative.Z;
		hitRelative.Z = 0;
		hitRelative = hitRelative.GetSafeNormal();

		float vert = FVector::DotProduct(mCharacter->GetActorForwardVector(), hitRelative);
		if (vert >= 0.7f)
		{
			if (_hitLocation.Z > GetSkelMeshComponent()->GetSocketLocation(TEXT("spine_03")).Z)
				sectionName = TEXT("HitHead");
			else
				sectionName = TEXT("HitCenter");
		}
		else
		{
			float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), hitRelative);
			if (hor > 0)
				sectionName = TEXT("HitRight");
			else
				sectionName = TEXT("HitLeft");
		}

		damageMomentum = -MomentumRatioByActor * _damageCauseRelative - (1 - MomentumRatioByActor) * hitRelative;
		damageMomentum = damageMomentum.GetUnsafeNormal();
	}
	else
	{
		sectionName = TEXT("HitBack");
		damageMomentum = -_damageCauseRelative;
	}

	return sectionName;
}

FName UHero_AnimInstance::GetHitResponseSection_HV(FName _boneName, const FVector & _damageCauseRelative, const FVector & _hitLocation) 
{
	FName sectionName;

	bool HitFromFront = FVector::DotProduct(mCharacter->GetActorForwardVector(), _damageCauseRelative) > 0.3f;

	bool randomSel = FMath::RandRange(-5, 5) >= 0;

	if (HitFromFront)
	{
		FVector hitRelative = _hitLocation - GetSkelMeshComponent()->GetSocketLocation(_boneName);
		float hitZOffset = hitRelative.Z;
		hitRelative.Z = 0;
		hitRelative = hitRelative.GetSafeNormal();


		float vert = FVector::DotProduct(mCharacter->GetActorForwardVector(), hitRelative);

		if (vert > 0.86f)
			sectionName = randomSel ? TEXT("HitFront_0") : TEXT("HitFront_1");
		else
		{
			float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), hitRelative);
			if (hor > 0)
				sectionName = randomSel ? TEXT("HitRight_0") : TEXT("HitRight_1");
			else
				sectionName = randomSel ? TEXT("HitLeft_0") : TEXT("HitLeft_1");
		}


		damageMomentum = -MomentumRatioByActor * _damageCauseRelative - (1 - MomentumRatioByActor) * hitRelative;
		damageMomentum = damageMomentum.GetUnsafeNormal();
	}
	else
	{
		sectionName = randomSel ? TEXT("HitBack_0") : TEXT("HitBack_1");
		damageMomentum = -_damageCauseRelative;
	}

	return sectionName;
}

FName UHero_AnimInstance::GetHitResponseSection_Katana(FName _boneName, const FVector & _damageCauseRelative, const FVector & _hitLocation) 
{
	FName sectionName;

	bool HitFromFront = FVector::DotProduct(mCharacter->GetActorForwardVector(), _damageCauseRelative) > 0.3f;

	if (HitFromFront)
	{
		FVector hitRelative = _hitLocation - GetSkelMeshComponent()->GetSocketLocation(_boneName);
		float hitZOffset = hitRelative.Z;
		hitRelative.Z = 0;
		hitRelative = hitRelative.GetSafeNormal();


		FVector chestLocation = GetSkelMeshComponent()->GetSocketLocation(TEXT("spine_03"));

		if (_hitLocation.Z >= chestLocation.Z)
		{
			// hit top
			float vert = FVector::DotProduct(mCharacter->GetActorForwardVector(), hitRelative);

			if (vert > 0.86f)
				sectionName = TEXT("HitFront_Head");
			else
			{
				float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), hitRelative);
				if (hor > 0)
					sectionName = TEXT("HitRight_Head");
				else
					sectionName = TEXT("HitLeft_Head");
			}
		}
		else
		{
			float kneeLevel = 0.5f * (GetSkelMeshComponent()->GetSocketLocation(TEXT("calf_l")).Z + GetSkelMeshComponent()->GetSocketLocation(TEXT("calf_r")).Z);

			if (_hitLocation.Z <= kneeLevel)
				sectionName = TEXT("HitFront_Hip");
			else
			{
				float vert = FVector::DotProduct(mCharacter->GetActorForwardVector(), hitRelative);
				if (vert > 0.86f)
					sectionName = TEXT("HitFront_Hip");
				else
				{
					float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), hitRelative);
					if (hor > 0)
						sectionName = TEXT("HitRight");
					else
						sectionName = TEXT("HitLeft");
				}
			}
		}

		damageMomentum = -MomentumRatioByActor * _damageCauseRelative - (1 - MomentumRatioByActor) * hitRelative;
		damageMomentum = damageMomentum.GetUnsafeNormal();
	}
	else
	{
		sectionName = TEXT("HitBack");
		damageMomentum = -_damageCauseRelative;
	}

	return sectionName;
}

FVector UHero_AnimInstance::GetFocusDodgeDirection() const
{
	switch (FocusDodgeDirection)
	{
	case EFocusDodgeDirection::Forward:
		return mCharacter->GetActorForwardVector();

	case EFocusDodgeDirection::Right45:
		return (.5 * mCharacter->GetActorForwardVector() + mCharacter->GetActorRightVector()).GetUnsafeNormal();

	case EFocusDodgeDirection::Left45:
		return (.5f *  mCharacter->GetActorForwardVector() - mCharacter->GetActorRightVector()).GetUnsafeNormal();

	case EFocusDodgeDirection::Right135:
		return (-.5f * mCharacter->GetActorForwardVector() + mCharacter->GetActorRightVector()).GetUnsafeNormal();

	case EFocusDodgeDirection::Left135:
		return (-.5f * mCharacter->GetActorForwardVector() - mCharacter->GetActorRightVector()).GetUnsafeNormal();

	case EFocusDodgeDirection::Right90:
		return mCharacter->GetActorRightVector();
	case EFocusDodgeDirection::Back:
	default:
	{
		FVector back = mCharacter->GetActorForwardVector();
		back = FVector(-back.X, -back.Y, -back.Z);
		return back;
	}
	case EFocusDodgeDirection::Left90:
	{
		FVector left = mCharacter->GetActorRightVector();
		left = FVector(-left.X, -left.Y, -left.Z);
		return left;
	}
	}
}

#pragma endregion

void UHero_AnimInstance::OnReborn()
{
	RecoverFromBeingHit(false);
}

void UHero_AnimInstance::OnBeingHit(FName boneName, const FVector & _damageCauseRelative, const FVector & _hitLocation)
{

	if (Hit_Montage)
	{

		FName sectionName;
		switch (CurrentEquipment)
		{
		case EEquipType::ShieldSword:
		case EEquipType::CrossBow:
		default:
			sectionName = GetHitResponseSection_SnsCB(boneName, _damageCauseRelative, _hitLocation);
			break;
		case EEquipType::TwoHandSword:
			sectionName = GetHitResponseSection_Katana(boneName, _damageCauseRelative, _hitLocation);
			break;
		case EEquipType::HeavyWeapon:
			sectionName = GetHitResponseSection_HV(boneName, _damageCauseRelative, _hitLocation);
			break;
		}

		this->PlayMontage(Hit_Montage, 1.0f, sectionName);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Hit_Montage is nullptr - UHero_AnimInstance::OnBeingHit"));
}

void UHero_AnimInstance::ResetOnBeingHit()
{
	mCharacter->bIsMovementDisabled = true;
	AttackState = EAttackState::BeingHit;
	NextAction = EActionType::None;

	bVelocityOverrideByAnim = true;
 
	if (ActivatedEquipment == EEquipType::Travel)
	{
		AttachWeapon(); 
	}
	mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;

	CurrentComboIndex = 0;
	OnDisableDamage(false, true);
	// for case, that counter attack failed, such attack from back
	OnDefendOff();
}

void UHero_AnimInstance::ResetOnBeingStuned()
{
	mCharacter->bIsMovementDisabled = true;
	AttackState = EAttackState::BeingStuned;
	NextAction = EActionType::None;

	bVelocityOverrideByAnim = true;

	if (ActivatedEquipment == EEquipType::Travel)
	{
		AttachWeapon();
	}
	mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;

	CurrentComboIndex = 0;
	OnDisableDamage(false, true);
	// for case, that counter attack failed, such attack from back
	OnDefendOff();
}

void UHero_AnimInstance::RecoverFromBeingHit(bool _bInterrupted)
{
	if (_bInterrupted)
	{
		// Take a other hit, cuz only hit can interrupt hit
		return;
	}

	AttackState = EAttackState::None;
	bVelocityOverrideByAnim = false;	
	mCharacter->bIsMovementDisabled = false;

	mCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	mCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
}

/** Called When switch melee weapon and equip*/
void UHero_AnimInstance::UpdateAnimationSetOnWeaponChange(EGearType _gearType)
{

	switch (_gearType)
	{
	case EGearType::LongSword:
		CurrentEquipment = EEquipType::ShieldSword;
		Current_AttackSectionName = &LongSwordAttack_Montage_SN;
		Attack_Montage = LongSword_Montage;
		Hit_Montage = AM_Sns_HitReaction;
		
		break;
	case EGearType::WarAxe:
	case EGearType::Mace:
		CurrentEquipment = EEquipType::ShieldSword;
		Current_AttackSectionName = &AxeMaceAttack_Montage_SN;
		Attack_Montage = AxeMace_Montage;
		Hit_Montage = AM_Sns_HitReaction;

		break;
	case EGearType::DoubleHandWeapon:
		CurrentEquipment = EEquipType::TwoHandSword;
		Current_AttackSectionName = &KatanaAttack_Montage_SN;
		Attack_Montage = Katana_Montage;
		Hit_Montage = AM_TH_HitReaction;

		break;
	case EGearType::BattleAxe:
		CurrentEquipment = EEquipType::HeavyWeapon;
		Current_AttackSectionName = &BattleAxeAttack_Montage_SN;
		Attack_Montage = BattleAxe_Montage;
		Hit_Montage = AM_HV_HitReaction;
		break;

	case EGearType::GreatSword:
	case EGearType::Hammer:
		CurrentEquipment = EEquipType::HeavyWeapon;
		Current_AttackSectionName = &GSHammerAttack_Montage_SN;
		Attack_Montage = GreatSwordHammer_Montage;
		Hit_Montage = AM_HV_HitReaction;
		break;
 
	case EGearType::CrossBow:
		CurrentEquipment = EEquipType::CrossBow;
		Hit_Montage = AM_CB_HitReaction;
		break;
	}
	CurrentComboIndex = 0;
}

void UHero_AnimInstance::AnimInstanceResetOnRagDoll()
{

	// Get Control Axis for strafing
	MoveForwardAxis = 0.0f;
	MoveRightAxis = 0.0f;
	bIsInAir = false;
	// Check if the player try to move?
	bTryToMove = false;

	currentSpeed = 0.0f;
	turn = 0.0f;
	
}

void UHero_AnimInstance::OnZeroSp()
{
	OnDefendOff();
}

void UHero_AnimInstance::HeadTrack()
{
	
	if (AttackState == EAttackState::BeingStuned)
	{
		HeadTrackYaw = 0;
		HeadTrackPitch = 0;
		return;
	}

	// Calculate the headTrack yaw and pitch
	FRotator actorRotation = mCharacter->GetActorRotation();
	FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(mCharacter->GetControlRotation(), actorRotation);

	float headTrack_yaw_target = delta.Yaw;
	float headTrack_pitch_target = delta.Pitch;
	if (headTrack_yaw_target >= 135.0f)
	{
		if (headTrack_yaw_target < 90)
			headTrack_yaw_target = -headTrack_yaw_target;
		else
			headTrack_yaw_target = headTrack_yaw_target - 180.0f;

		headTrack_pitch_target = -headTrack_pitch_target;
	}
	else if (headTrack_yaw_target <= -135.0f)
	{
		if (headTrack_yaw_target > -90)
			headTrack_yaw_target = -headTrack_yaw_target;
		else
			headTrack_yaw_target = headTrack_yaw_target + 180.0f;

		headTrack_pitch_target = -headTrack_pitch_target;
	}

	HeadTrackYaw = FMath::FInterpTo(HeadTrackYaw, headTrack_yaw_target, GetWorld()->DeltaTimeSeconds, HeadTrackRate);
	HeadTrackPitch = FMath::FInterpTo(HeadTrackPitch, headTrack_pitch_target, GetWorld()->DeltaTimeSeconds, HeadTrackRate);

}

