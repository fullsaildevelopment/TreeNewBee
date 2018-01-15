// Fill out your copyright notice in the Description page of Project Settings.

#include "Base_AnimInstance.h"


UBase_AnimInstance::UBase_AnimInstance(const FObjectInitializer& _objectInitalizer)
{
	
}

void UBase_AnimInstance::OnBeginPlay()
{
}

void UBase_AnimInstance::OnInit()
{
}

void UBase_AnimInstance::OnUpdate(float _deltaTime)
{
}

void UBase_AnimInstance::OnPostEvaluate()
{
}

void UBase_AnimInstance::OnBeingHit(float _damage, FName boneName, const FVector & _shotFromDirection, const FVector & _hitLocation, const UPawnStatsComponent * _pawnStats)
{
}


float UBase_AnimInstance::PlayMontage(class UAnimMontage* _animMontage, float _rate, FName _startSectionName)
{
	if (_animMontage)
	{
		float const duration = this->Montage_Play(_animMontage, _rate);
		if (duration > 0.f)
		{
			if (_startSectionName != NAME_None)
			{
				this->Montage_JumpToSection(_startSectionName, _animMontage);
			}
		}
		return duration;
	}
	return 0.f;

}

void UBase_AnimInstance::OnMontageStartHandle(UAnimMontage * _animMontage)
{

}

void UBase_AnimInstance::OnMontageEndHandle(UAnimMontage * _animMontage, bool _bInterruptted)
{
}


void UBase_AnimInstance::OnMontageBlendOutStartHandle(UAnimMontage * _animMontage, bool _bInterruptted)
{

}
