// Fill out your copyright notice in the Description page of Project Settings.

#include "PawnStatsComponent.h"
#include "GameFramework/Character.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

#include "Combat/Weapon.h"
#include "Combat/Armor.h"
#include "CustomType.h"
#include "TheLastBastionCharacter.h"

// Sets default values for this component's properties
UPawnStatsComponent::UPawnStatsComponent()
{

	mCharacter = Cast<ATheLastBastionCharacter>(this->GetOwner());

	if (mCharacter != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PawnStatsComponent is owned by TheLastBastion Character"));

		UCustomType::FindClass<AWeapon>(LeftHandWeapon_ClassBp, TEXT("/Game/Blueprints/Gears/Tsun_Shield"));
		UCustomType::FindClass<AWeapon>(RightHandWeapon_ClassBp, TEXT("/Game/Blueprints/Gears/Tsun_SHSword"));
		UCustomType::FindClass<AArmor>(Armor_ClassBp, TEXT("/Game/Blueprints/Gears/Tsun_Armor"));

		Head = CreateDefaultSubobject<USphereComponent>(TEXT("Head"));
		Head->SetupAttachment(mCharacter->GetMesh(), TEXT("head"));
		Head->InitSphereRadius(12);
		Head->RelativeLocation = FVector(5, 2.5f, 0);
		Head->SetCollisionProfileName(TEXT("HeroBody"));
		Head->bGenerateOverlapEvents = true;
		Head->bHiddenInGame = false;

		Body = CreateDefaultSubobject<UBoxComponent>(TEXT("Body"));
		Body->SetupAttachment(mCharacter->GetMesh(), TEXT("spine_02"));
		Body->InitBoxExtent(FVector(40, 15, 25));
		Body->RelativeLocation = FVector(-10, 0, 0);
		Body->SetCollisionProfileName(TEXT("HeroBody"));
		Body->bGenerateOverlapEvents = true;
		Body->bHiddenInGame = false;

	}
	else
		UE_LOG(LogTemp, Warning, TEXT("PawnStatsComponent owner must be a TheLastBastion Character"));

}


// Called when the game starts
void UPawnStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();

	// Equip the owner character
	if (world)
	{
		if (LeftHandWeapon_ClassBp)
		{
			LeftHandWeapon = world->SpawnActor<AWeapon>(LeftHandWeapon_ClassBp);
			LeftHandWeapon->AttachToComponent(mCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Shield"));
		}

		if (RightHandWeapon_ClassBp)
		{
			RightHandWeapon = world->SpawnActor<AWeapon>(RightHandWeapon_ClassBp);
			RightHandWeapon->AttachToComponent(mCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("SHSwordEquip"));
		}

		if (Armor_ClassBp)
		{
			Armor = world->SpawnActor<AArmor>(Armor_ClassBp);
			Armor->AttachToComponent(mCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Root"));
			Armor->Equip(mCharacter->GetMesh());
		}
	}

	if (Head && Body && LeftHandWeapon && RightHandWeapon)
	{
		Head->OnComponentBeginOverlap.AddDynamic(this, &UPawnStatsComponent::OnHeadHit);
		Body->OnComponentBeginOverlap.AddDynamic(this, &UPawnStatsComponent::OnBodyHit);
		LeftHandWeapon->GetWeaponMeshRef()
			->OnComponentBeginOverlap.AddDynamic(this, &UPawnStatsComponent::OnLeftHandWeaponHit);
		RightHandWeapon->GetWeaponMeshRef()
			->OnComponentBeginOverlap.AddDynamic(this, &UPawnStatsComponent::OnRightHandWeaponHit);
	}
}

// Called every frame
void UPawnStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UPawnStatsComponent::EnableWeapon(bool _bIsRightHand, bool _bIsAll)
{

	UE_LOG(LogTemp, Warning, TEXT("enable Weapon"));
	if (_bIsAll)
	{
		LeftHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		RightHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		if (_bIsRightHand)
			RightHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		else
			LeftHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

}

void UPawnStatsComponent::DisableWeapon(bool _bIsRightHand, bool _bIsAll)
{
	UE_LOG(LogTemp, Warning, TEXT("disable Weapon"));

	if (_bIsAll)
	{
		LeftHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RightHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		if (_bIsRightHand)
			RightHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		else
			LeftHandWeapon->GetWeaponMeshRef()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

}

void UPawnStatsComponent::OnEquipWeapon()
{
	RightHandWeapon->AttachToComponent(mCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("SingleHandWeapon"));
}

void UPawnStatsComponent::OnSheathWeapon()
{
	RightHandWeapon->AttachToComponent(mCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("SHSwordEquip"));

}


void UPawnStatsComponent::OnBodyHit(UPrimitiveComponent * _overlappedComponent, AActor * _otherActor, UPrimitiveComponent * _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult & _SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Body hit by %s"), *_otherActor->GetName());
}

void UPawnStatsComponent::OnHeadHit(UPrimitiveComponent * _overlappedComponent, AActor * _otherActor, UPrimitiveComponent * _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult & _SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Head hit by %s"), *_otherActor->GetName());
}

void UPawnStatsComponent::OnRightHandWeaponHit(UPrimitiveComponent * _overlappedComponent, AActor * _otherActor, UPrimitiveComponent * _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult & _SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Right Hand Weapon hit %s"), *_otherActor->GetName());

}

void UPawnStatsComponent::OnLeftHandWeaponHit(UPrimitiveComponent * _overlappedComponent, AActor * _otherActor, UPrimitiveComponent * _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult & _SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Left Hand Weapon hit %s"), *_otherActor->GetName());

}


