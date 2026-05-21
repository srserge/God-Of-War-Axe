// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/GOWWeaponBase.h"

// Sets default values
AGOWWeaponBase::AGOWWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh"));
	SetRootComponent(m_WeaponMesh);

	m_WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_WeaponMesh->SetGenerateOverlapEvents(false);
}
