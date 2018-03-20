// Fill out your copyright notice in the Description page of Project Settings.

#include "Shield.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"




AShield::AShield()
{
	Thickness = 40.0f;
	MeshArea = FVector2D(40, 40);
	ShieldBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ShieldBox"));
	ShieldBox->SetupAttachment(RootComponent);
	ShieldBox->SetBoxExtent(FVector(0.5f * Thickness, MeshArea.X, MeshArea.Y));
	ShieldBox->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	ShieldBox->SetCollisionProfileName(TEXT("Shield"));
	ShieldBox->RelativeLocation = FVector(Thickness *.5f, 0, 0);

	DamageEdgeOffset_start = FVector::ZeroVector;
	DamageEdgeOffset_end = DamageEdgeOffset_start;

	DamageVolumnExtend = FVector(Thickness * 0.5f, MeshArea.X * 0.5f, MeshArea.Y * 0.5f);

}


void AShield::BeginPlay()
{
	Super::BeginPlay();
	FVector box = ShieldBox->GetUnscaledBoxExtent();
	CollisionArea = FVector2D(box.Y, box.Z);
	WidthMeshColRatio = MeshArea.X / CollisionArea.X;
	HeightMeshColRatio = MeshArea.Y / CollisionArea.Y;
}

void AShield::Equip(USkeletalMeshComponent * const _skeletonMeshComponent)
{
	Super::Equip(_skeletonMeshComponent);
	ShieldBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// delete all attach actor
	TArray<AActor*> attachedBullets;
	GetAttachedActors(attachedBullets);
	for (int iBullets = 0; iBullets < attachedBullets.Num(); iBullets++)
	{
		attachedBullets[iBullets]->Destroy();
	}
}

void AShield::Arm(USkeletalMeshComponent * const _skeletonMeshComponent)
{
	Super::Arm(_skeletonMeshComponent);
	ShieldBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

FVector AShield::GetArrowAttachLocation(const FVector & _hitLocation) const
{
	FVector arrowAttachLocation = _hitLocation - this->GetActorForwardVector() * Thickness;
	arrowAttachLocation = UKismetMathLibrary::InverseTransformLocation(this->GetActorTransform(), arrowAttachLocation);
	FVector surfaceOffset = arrowAttachLocation;	
	//UE_LOG(LogTemp, Log, TEXT("surfaceOffset: %f, %f, %f - AShield::GetArrowAttachLocation"), surfaceOffset.X, surfaceOffset.Y, surfaceOffset.Z);
	surfaceOffset.Y *= WidthMeshColRatio;
	surfaceOffset.Z *= HeightMeshColRatio;
	arrowAttachLocation = this->GetActorLocation() + 
		this->GetActorRightVector() * surfaceOffset.Y + 
		surfaceOffset.Z * this->GetActorUpVector();
	//attachLocation = this->GetActorLocation() + surfaceOffset;
	return arrowAttachLocation;
}
