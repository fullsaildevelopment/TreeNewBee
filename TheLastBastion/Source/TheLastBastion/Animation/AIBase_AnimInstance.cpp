// Fill out your copyright notice in the Description page of Project Settings.

#include "AIBase_AnimInstance.h"
#include "AICharacters/TheLastBastionAIBase.h"
#include "AI/TheLastBastionBaseAIController.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Combat/PawnStatsComponent.h"
#include "Combat/Gear.h"
#include "Kismet/KismetMathLibrary.h"



UAIBase_AnimInstance::UAIBase_AnimInstance(const FObjectInitializer& _objectInitalizer) :Super(_objectInitalizer)
{
	CurrentActionState = EAIActionState::None;
	MomentumRatioByActor = 0.7f;
}

void UAIBase_AnimInstance::OnBeginPlay()
{
	mCharacter = Cast<ATheLastBastionAIBase>(TryGetPawnOwner());
	if (mCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("The AIBase_AnimInstance can only assigned to ATheLastBastionAIBase - UAIBase_AnimInstance "));
		return;
	}

	mBaseCharacter = mCharacter;

	Super::OnBeginPlay();

	//OnMontageStarted.AddDynamic(this, &UAIMelee_AnimInstance::OnMontageStartHandle);
	//OnMontageBlendingOut.AddDynamic(this, &UAIMelee_AnimInstance::OnMontageBlendOutStartHandle);
}

void UAIBase_AnimInstance::OnInit()
{

}

void UAIBase_AnimInstance::OnUpdate(float _deltaTime)
{
	Super::OnUpdate(_deltaTime);

	if (mCharacter == nullptr)
		return;

	if (mCharacter->GetIsDead())
		return;

	UCharacterMovementComponent* movementComp = mCharacter->GetCharacterMovement();
	FVector velocity = movementComp->Velocity;
	currentSpeed = velocity.SizeSquared();
	FVector velocityLocalDir = UKismetMathLibrary::InverseTransformDirection(mCharacter->GetTransform(), velocity);
	velocityLocalDir = FVector(velocityLocalDir.X, velocityLocalDir.Y, 0).GetSafeNormal();


	if (mCharacter->IsWalking())
	{
		MoveForwardAxis = velocityLocalDir.X;
		MoveRightAxis = velocityLocalDir.Y;
	}
	else
	{
		MoveForwardAxis = 2 * velocityLocalDir.X;
		MoveRightAxis = 2 * velocityLocalDir.Y;
	}


	// Get whether the target is closed enough from BT
	ATheLastBastionBaseAIController* baseAICtrl
		= Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
	if (baseAICtrl == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("baseAICtrl is nullptr -  UAIBase_AnimInstance::SyncMotionForMeleeAttack"));
		return;
	}

	bInMeleeRange = baseAICtrl->GetToTargetActorDistanceSqr() <= mCharacter->GetMeleeAttackDist_Sq();


	switch (CurrentActionState)
	{
	case EAIActionState::MeleeAttack:
	case EAIActionState::MeleePreAttack:
		SyncMotionForMeleeAttack();
		break;
	case EAIActionState::GettingHurt:
	case EAIActionState::GettingStuned:
		SyncMotionForGettingHurt();
		break;

	case EAIActionState::Defend:
		SyncMotionForDefend();
		break;
	case EAIActionState::Dodge:
		SyncMotionForDodge();
		break;
	case EAIActionState::None:
		break;
	default:
		break;
	}


}

void UAIBase_AnimInstance::OnPostEvaluate()
{
}

void UAIBase_AnimInstance::ResetOnBeingHit()
{
	CurrentActionState = EAIActionState::GettingHurt;
	mCharacter->GetCharacterMovement()->RotationRate.Yaw = AICharacter_RotatingRate;

	ATheLastBastionBaseAIController* baseAICtrl 
		= Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
	if (baseAICtrl == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("baseAICtrl is nullptr - UAIBase_AnimInstance::ResetOnBeingHit"));
		return;
	}

	baseAICtrl->SetIsPaused_BBC(true);


	//baseAICtrl->OnBeingHit(mCharacter->GetCharacterType());
}

void UAIBase_AnimInstance::ResetOnBeingStuned()
{
	CurrentActionState = EAIActionState::GettingStuned;

	mCharacter->GetCharacterMovement()->RotationRate.Yaw = AICharacter_RotatingRate;
	ATheLastBastionBaseAIController* baseAICtrl
		= Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
	if (baseAICtrl == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("baseAICtrl is nullptr - UAIBase_AnimInstance::ResetOnBeingHit"));
		return;
	}

	baseAICtrl->SetIsPaused_BBC(true);
}

void UAIBase_AnimInstance::OnBeingHit(FName boneName, const FVector & _damageCauseRelative, const FVector & _hitLocation)
{
	//Hit_Montage = AM_SingleHandWeapon_HitReaction;

	if (Hit_Montage == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Hit Montage is nullptr"));
		return;
	}

	AGear* rightHandWeapon = mCharacter->GetCurrentWeapon();
	if (rightHandWeapon == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("rightHandWeapon == nullptr ,  UAIBase_AnimInstance::OnBeingHit"));
		return;
	}


	FName sectionToPlay;
	switch (rightHandWeapon->GetGearType())
	{

	case EGearType::LongSword:
	case EGearType::WarAxe:
	case EGearType::Mace:
	{
		bool isSH = mCharacter->GetCurrentSecondaryWeapon() == nullptr;
		
		sectionToPlay = isSH ? HitReaction_SHSword(boneName, _damageCauseRelative, _hitLocation) : 
			HitReaction_Sns_CB(boneName, _damageCauseRelative, _hitLocation);
		break;
	}
	case EGearType::DoubleHandWeapon:
	case EGearType::DaiKatana:
		sectionToPlay = HitReaction_Katana(boneName, _damageCauseRelative, _hitLocation);
		break;

	case EGearType::CrossBow:
		sectionToPlay = HitReaction_Sns_CB(boneName, _damageCauseRelative, _hitLocation);
		break;

	case EGearType::BattleAxe:
	case EGearType::GreatSword:
	case EGearType::Hammer:
		sectionToPlay = HitReaction_HV(boneName, _damageCauseRelative, _hitLocation);
		break;
	default:
		break;
	}

	PlayMontage(Hit_Montage, 1.0f, sectionToPlay);
}

void UAIBase_AnimInstance::OnCounterAttack(FName sectionName, float _rate)
{
	CurrentActionState = EAIActionState::MeleePreAttack;

	if (CounterAttack_Montage == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Parry_Montage == nullptr ,UAIBase_AnimInstance::OnParry "));
		return;
	}

	mCharacter->GetCharacterMovement()->RotationRate.Yaw = AICharacter_RotatingRate;

	//OnDisableWeapon(true, true);
	PlayMontage(CounterAttack_Montage, _rate, sectionName);
}

void UAIBase_AnimInstance::OnParry(FName sectionName)
{
	CurrentActionState = EAIActionState::Defend;
	if (Parry_Montage == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Parry_Montage == nullptr ,UAIBase_AnimInstance::OnParry "));
		return;
	}
	mCharacter->GetCharacterMovement()->RotationRate.Yaw = AICharacter_RotatingRate;

	OnDisableWeapon(true, true);
	PlayMontage(Parry_Montage, 1.0f, sectionName);
}

void UAIBase_AnimInstance::OnDodge(FName sectionName)
{
	CurrentActionState = EAIActionState::Dodge;
	if (Dodge_Montage == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Dodge_Montage == nullptr ,UAIBase_AnimInstance::OnDodge "));
		return;
	}
	mCharacter->GetCharacterMovement()->RotationRate.Yaw = AICharacter_RotatingRate;

	OnDisableWeapon(true, true);
	PlayMontage(Dodge_Montage, 1.0f, sectionName);

}

void UAIBase_AnimInstance::UpdateAnimationSetOnWeaponChange(EGearType _gearType)
{
	switch (_gearType)
	{
	case EGearType::LongSword:
	case EGearType::WarAxe:
	case EGearType::Mace:
	{
		bool bSH = mCharacter->GetCurrentSecondaryWeapon() == nullptr;
		if (bSH)
		{
			Hit_Montage = AM_SingleHandWeapon_HitReaction;
			Parry_Montage = AM_SH_Parry;
			Dodge_Montage = AM_SH_Dodge;
			
		}
		else
		{
			Hit_Montage = AM_Sns_HitReaction;
			Parry_Montage = AM_Sns_Parry;
			CounterAttack_Montage = AM_CounterAttack;
			Dodge_Montage = nullptr;
		}
		break;
	}
	case EGearType::DoubleHandWeapon:
		Hit_Montage = AM_TH_HitReaction;
		break;
	case EGearType::BattleAxe:
	case EGearType::GreatSword:
	case EGearType::Hammer:
		Hit_Montage = AM_HV_HitReaction;
		Parry_Montage = AM_HV_ParryDodge;
		break;
	case EGearType::CrossBow:
		Hit_Montage = AM_CB_HitReaction;
		break;
	case EGearType::DaiKatana:
		Hit_Montage = AM_TH_HitReaction;
		Parry_Montage = AM_Katana_Parry;
		Dodge_Montage = AM_Katana_Dodge;
		CounterAttack_Montage = AM_CounterAttack;
		break;
	default:
		break;
	}
}

void UAIBase_AnimInstance::OnMontageStartHandle(UAnimMontage * _animMontage) {}

void UAIBase_AnimInstance::OnMontageBlendOutStartHandle(UAnimMontage * _animMontage, bool _bInterruptted)
{
	Super::OnMontageBlendOutStartHandle(_animMontage, _bInterruptted);

	if (!_bInterruptted)
	{

		if (_animMontage == Hit_Montage)
		{
			if (CurrentActionState == EAIActionState::GettingStuned)
				OnTurnToDazedLoop();
			else
				OnHitMontageEnd();
		}
		else if (_animMontage == Parry_Montage || _animMontage == Dodge_Montage || _animMontage == CounterAttack_Montage || _animMontage == AM_Dazed)
		{
			OnHitMontageEnd();
		}
		else if (_animMontage == GetUp_Montage && mCharacter->IsOldKnockOut())
		{
			OnGetupMontageEnd();
		}
	}
	else
	{



		//// if get up is interrupt by something, only recover the focus
		//if (_animMontage == GetUp_Montage && mCharacter->IsOldKnockOut())
		//{
		//	ATheLastBastionBaseAIController* baseAICtrl = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
		//	// focus on target
		//	AActor* target = baseAICtrl->GetTargetActor_BBC();
		//	if (target)
		//		baseAICtrl->SetFocus(target);
		//}
	}

}

void UAIBase_AnimInstance::FxMeleeSwing(bool _rightHand)
{
	if (CurrentActionState == EAIActionState::MeleeAttack && !mCharacter->GetIsDead())
	{
		Super::FxMeleeSwing(_rightHand);
	}
}

void UAIBase_AnimInstance::Fire(const AActor * _target)
{
}

void UAIBase_AnimInstance::StopFire()
{
}

void UAIBase_AnimInstance::FinishAttack()
{
}

void UAIBase_AnimInstance::SyncMotionForMeleeAttack()
{
	UCharacterMovementComponent* movementComp = mCharacter->GetCharacterMovement();
	// Sync Velocity
	float speed = GetCurveValue("Speed");
	FVector Velocity = movementComp->Velocity;
	movementComp->Velocity = mCharacter->GetActorForwardVector() * speed;
	movementComp->Velocity.Z = Velocity.Z;

	// Sync Rotation rate, the faster move the slower it rotates
	// float rotateRate = UKismetMathLibrary::MapRangeClamped(speed, 200.0f, 600.0f,360.0f, 90.0f);

	movementComp->RotationRate.Yaw = GetCurveValue("RotationRate");


}

void UAIBase_AnimInstance::SyncMotionForGettingHurt()
{
	UCharacterMovementComponent* movementComp = mCharacter->GetCharacterMovement();

	// Sync Velocity
	float speed = GetCurveValue("Speed");

	FVector Velocity = movementComp->Velocity;
	movementComp->Velocity = -mCharacter->GetActorForwardVector() * speed;
	movementComp->Velocity.Z = Velocity.Z;

}

void UAIBase_AnimInstance::SyncMotionForDefend()
{
	UCharacterMovementComponent* movementComp = mCharacter->GetCharacterMovement();

	// Sync Velocity
	float speed = GetCurveValue("Speed");

	FVector Velocity = movementComp->Velocity;
	movementComp->Velocity = -mCharacter->GetActorForwardVector() * speed;
	movementComp->Velocity.Z = Velocity.Z;

}

void UAIBase_AnimInstance::SyncMotionForDodge()
{
	UCharacterMovementComponent* movementComp = mCharacter->GetCharacterMovement();

	// Sync Velocity
	float speed = GetCurveValue("Speed");

	FVector Velocity = movementComp->Velocity;
	movementComp->Velocity = -mCharacter->GetActorForwardVector() * speed;
	movementComp->Velocity.Z = Velocity.Z;

}

void UAIBase_AnimInstance::SyncMotionForNone()
{

}

FName UAIBase_AnimInstance::HitReaction_SHSword(FName boneName, const FVector& _damageCauseRelative, const FVector& _hitLocation)
{
	// assume always face to attacker
	FName sectionName;

	FVector hitRelative = _hitLocation - GetSkelMeshComponent()->GetSocketLocation(boneName);
	float hitZOffset = hitRelative.Z;
	hitRelative.Z = 0;
	hitRelative = hitRelative.GetSafeNormal();

	float vert = FVector::DotProduct(mCharacter->GetActorForwardVector(), hitRelative);

	FVector chestLocation = GetSkelMeshComponent()->GetSocketLocation("spine_03");

	bool bHeadHit = _hitLocation.Z > chestLocation.Z;
	if (bHeadHit)
	{
		if (vert > 0.7f)
			sectionName = SH_HitReaction_HEAD_FRONT;
		else
		{
			float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), hitRelative);
			if (hor > 0)
				sectionName = SH_HitReaction_HEAD_RIGHT;
			else
				sectionName = SH_HitReaction_HEAD_LEFT;
		}
	}
	else
	{
		bool bBottomHit = GetSkelMeshComponent()->GetSocketLocation("pelvis").Z > _hitLocation.Z;
		if (bBottomHit)
		{
			float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), hitRelative);
			if (hor > 0)
				sectionName = SH_HitReaction_LEG_RIGHT;
			else
				sectionName = SH_HitReaction_LEG_LEFT;
		}

		else
		{
			if (vert > 0.7f)
				sectionName = SH_HitReaction_BODY_FRONT;
			else
			{
				float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), hitRelative);
				if (hor > 0)
					sectionName = SH_HitReaction_BODY_RIGHT;
				else
					sectionName = SH_HitReaction_BODY_LEFT;
			}
		}
	}

	damageMomentum = -_damageCauseRelative;

	return sectionName;
}

FName UAIBase_AnimInstance::HitReaction_Katana(FName boneName, const FVector & _shotFromDirection, const FVector & _hitLocation)
{
	// assume always face to attacker
	FName sectionName;

	
	FVector hitRelative = _hitLocation - GetSkelMeshComponent()->GetSocketLocation(boneName);
	float hitZOffset = hitRelative.Z;
	hitRelative.Z = 0;
	hitRelative = hitRelative.GetSafeNormal();

	FVector chestLocation = GetSkelMeshComponent()->GetSocketLocation(TEXT("spine_03"));
	bool bHeadHit = chestLocation.Z < _hitLocation.Z;


	float vert = FVector::DotProduct(mCharacter->GetActorForwardVector(), hitRelative);


	if (vert > 0.7f)
		sectionName = bHeadHit ? TH_HitReact_FrontTop : TH_HitReact_Front;
	else
	{
		ATheLastBastionBaseAIController* baseAICtrl = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
		if (baseAICtrl == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("baseAICtrl == nullptr, UAIBase_AnimInstance::HitReaction_HV"));
			return FName();
		}

		// if this ai is not targetting anyone, and it get damage from its back
		if (vert < -0.5f && baseAICtrl->GetTargetActor_BBC() == nullptr)
		{
			sectionName = TH_HitReact_Back;
		}
		else
		{
			float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), hitRelative);
			if (hor > 0)
				sectionName = bHeadHit ? TH_HitReact_RightTop : TH_HitReact_Right;
			else
				sectionName = bHeadHit ? TH_HitReact_LeftTop : TH_HitReact_Left;
		}
	}

	damageMomentum = -mCharacter->GetActorForwardVector();

	return sectionName;
}

FName UAIBase_AnimInstance::HitReaction_HV(FName boneName, const FVector & _shotFromDirection, const FVector & _hitLocation)
{
	// assume always face to attacker
	FName sectionName;

	FVector hitRelative = _hitLocation - GetSkelMeshComponent()->GetSocketLocation(boneName);
	float hitZOffset = hitRelative.Z;
	hitRelative.Z = 0;
	hitRelative = hitRelative.GetSafeNormal();

	float vert = FVector::DotProduct(mCharacter->GetActorForwardVector(), hitRelative);

	bool bSEL = FMath::RandRange(-5, 5) > 0;

	if (vert > 0.7f)
		sectionName = bSEL ? HV_HitReact_Front_0 : HV_HitReact_Front_1;
	else
	{
		ATheLastBastionBaseAIController* baseAICtrl = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
		if (baseAICtrl == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("baseAICtrl == nullptr, UAIBase_AnimInstance::HitReaction_HV"));
			return FName();
		}

		// if this ai is not targetting anyone, and it get damage from its back
		if (vert < -0.5f && baseAICtrl ->GetTargetActor_BBC() == nullptr)
		{
			sectionName = bSEL ? HV_HitReact_Back_0 : HV_HitReact_Back_1;
		}
		else
		{
			float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), hitRelative);
			if (hor > 0)
				sectionName = bSEL ? HV_HitReact_Right_0 : HV_HitReact_Right_1;
			else
				sectionName = bSEL ? HV_HitReact_Left_0 : HV_HitReact_Left_1;
		}
	}

	damageMomentum = -mCharacter->GetActorForwardVector();

	return sectionName;
}

FName UAIBase_AnimInstance::HitReaction_Sns_CB(FName boneName, const FVector & _shotFromDirection, const FVector & _hitLocation)
{
	// assume always face to attacker
	FName sectionName;

	FVector hitRelative = _hitLocation - GetSkelMeshComponent()->GetSocketLocation(boneName);
	float hitZOffset = hitRelative.Z;
	hitRelative.Z = 0;
	hitRelative = hitRelative.GetSafeNormal();

	float vert = FVector::DotProduct(mCharacter->GetActorForwardVector(), hitRelative);
	if (vert > 0.7f)
	{
		FVector chestLocation = GetSkelMeshComponent()->GetSocketLocation(TEXT("spine_03"));
		bool bHeadHit = chestLocation.Z < _hitLocation.Z;

		sectionName = bHeadHit ? SnsCB_HitReact_FrontTop : SnsCB_HitReact_Front;
	}
	else
	{
		ATheLastBastionBaseAIController* baseAICtrl = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
		if (baseAICtrl == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("baseAICtrl == nullptr, UAIBase_AnimInstance::HitReaction_HV"));
			return FName();
		}

		// if this ai is not targetting anyone, and it get damage from its back
		if (vert < -0.5f && baseAICtrl->GetTargetActor_BBC() == nullptr)
		{
			sectionName = SnsCB_HitReact_Back;
		}
		else
		{
			float hor = FVector::DotProduct(mCharacter->GetActorRightVector(), hitRelative);
			if (hor > 0)
				sectionName = SnsCB_HitReact_Right;
			else
				sectionName = SnsCB_HitReact_Left;
		}
	}

	damageMomentum = -mCharacter->GetActorForwardVector();

	return sectionName;
}

void UAIBase_AnimInstance::OnHitMontageEnd()
{
	//UE_LOG(LogTemp, Log, TEXT("I am recover from being hit - OnMontageBlendOutStartHandle"));
	CurrentActionState = EAIActionState::None;
	// Tell BT that this attack is done
	if (mCharacter)
	{
		ATheLastBastionBaseAIController* baseAICtrl = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
		// recover the rotation rate from melee attack motion sync
		mCharacter->GetCharacterMovement()->RotationRate.Yaw = AICharacter_RotatingRate;
		if (baseAICtrl)
		{
			//baseAICtrl->SetAICurrentActionState_BBC(CurrentActionState);
			baseAICtrl->SetIsPaused_BBC(false);
			UBehaviorTreeComponent* btc = baseAICtrl->GetBTComp();
			if (btc)
			{
				OnRecoverFromHitSignature.ExecuteIfBound(btc);
			}
		}
	}

}

void UAIBase_AnimInstance::OnGetupMontageEnd()
{
	//UE_LOG(LogTemp, Log, TEXT("I am recover from being hit - OnMontageBlendOutStartHandle"));
	CurrentActionState = EAIActionState::None;
	// Tell BT that this attack is done
	if (mCharacter)
	{
		ATheLastBastionBaseAIController* baseAICtrl = Cast<ATheLastBastionBaseAIController>(mCharacter->GetController());
		// recover the rotation rate from melee attack motion sync
		mCharacter->GetCharacterMovement()->RotationRate.Yaw = AICharacter_RotatingRate;
		if (baseAICtrl)
		{
			//baseAICtrl->SetAICurrentActionState_BBC(CurrentActionState);
			baseAICtrl->SetIsPaused_BBC(false);

			UBehaviorTreeComponent* btc = baseAICtrl->GetBTComp();
			if (btc)
			{
				OnRecoverFromHitSignature.ExecuteIfBound(btc);
			}
			
			// focus on target
			AActor* target = baseAICtrl->GetTargetActor_BBC();
			if (target)
				baseAICtrl->SetFocus(target);
		}
	}

}

void UAIBase_AnimInstance::OnTurnToDazedLoop()
{
	mCharacter->GetCharacterMovement()->RotationRate.Yaw = 0.0f;
	PlayMontage(AM_Dazed);
}
