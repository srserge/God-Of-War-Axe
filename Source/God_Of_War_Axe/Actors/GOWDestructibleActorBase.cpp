// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/GOWDestructibleActorBase.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"

AGOWDestructibleActorBase::AGOWDestructibleActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	USceneComponent* RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(RootSceneComponent);
	
	m_StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	m_StaticMeshComponent->SetupAttachment(RootComponent);
	m_StaticMeshComponent->SetVisibility(false);
	
	m_GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	m_GeometryCollectionComponent->SetupAttachment(RootComponent);
	m_GeometryCollectionComponent->SetSimulatePhysics(false);

	// Enable notifications when it's destroyed and when the broken parts are removed.
	m_GeometryCollectionComponent->SetNotifyBreaks(true);
	m_GeometryCollectionComponent->SetNotifyRemovals(true);
}

void AGOWDestructibleActorBase::BeginPlay()
{
	Super::BeginPlay();

	// Notifies when Chaos simulation has been triggered.
	// After a few seconds, the actor is removed from the scene.
	m_GeometryCollectionComponent->OnChaosBreakEvent.AddDynamic(this, &AGOWDestructibleActorBase::OnGeometryBreak);
}

void AGOWDestructibleActorBase::OnGeometryBreak(const FChaosBreakEvent& BreakEvent)
{
	SetLifeSpan(m_DestroyDelay);
}

void AGOWDestructibleActorBase::OnHit(const FHitResult& HitResult)
{
	if (m_bHasFractured) return;
	
	m_bHasFractured = true;
	
	m_GeometryCollectionComponent->SetSimulatePhysics(true);

	// Static Mesh destroyed because is no longer useful.
	m_StaticMeshComponent->DestroyComponent();
	
	m_GeometryCollectionComponent->AddRadialImpulse(
		HitResult.ImpactPoint,
		m_ImpactImpulseMagnitude * 0.5f, 
		m_ImpactImpulseMagnitude,         
		ERadialImpulseFalloff::RIF_Linear,
		true                                
	);
	
	if (ensureAlwaysMsgf(m_BarrelSmashSound, TEXT("Barrel Smash Sound is not set!")) && ensureAlwaysMsgf(m_SoundAttenuation, TEXT("Barrel Smash Sound Attenuation is not set!")))
	{
		UGameplayStatics::PlaySoundAtLocation(this, m_BarrelSmashSound, GetActorLocation(), 1.0f, 1.0f, 0.0f, m_SoundAttenuation);
	}
}