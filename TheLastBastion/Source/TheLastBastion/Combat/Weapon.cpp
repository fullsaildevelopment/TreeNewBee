// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Components/StaticMeshComponent.h"



AWeapon::AWeapon() 
{

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Appearence"));
	Mesh->SetCollisionProfileName("EnemyWeapon");
	RootComponent = Mesh;


}
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}


