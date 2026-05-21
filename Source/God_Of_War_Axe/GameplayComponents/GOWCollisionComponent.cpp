// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayComponents/GOWCollisionComponent.h"
#include "Interfaces/GOWDamageableInterface.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UGOWCollisionComponent::UGOWCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGOWCollisionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//  Adds Leviathan Axe actor as an ignored actor.
	m_IgnoredActors.Add(GetOwner());
}

bool UGOWCollisionComponent::ActivateThrowCollision()
{
	AActor* const LeviathanAxe { GetOwner() };
	if (!ensure(LeviathanAxe)) return false;

	const FVector CurrentLocation{ LeviathanAxe->GetActorLocation() };
	
	m_HitResults.Reset();

	const bool bHasHit{ UKismetSystemLibrary::BoxTraceMulti
	(
		GetWorld(),
		m_LocationLastFrame,
		CurrentLocation,
		m_BoxHalfSize,
		LeviathanAxe->GetActorRotation(),
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,                         
		m_IgnoredActors,
		EDrawDebugTrace::None,
		m_HitResults,
		true                            
	)};
	
	SetLastFrameLocation(CurrentLocation);

	if (bHasHit)
	{
		HandleCollision();
	}

	return bHasHit;
}

bool UGOWCollisionComponent::ActivateRecallCollision()
{
	AActor* const LeviathanAxe { GetOwner() };
	if (!ensure(LeviathanAxe)) return false;

	const FVector CurrentLocation{ LeviathanAxe->GetActorLocation() };
	
	m_HitResults.Reset();

	const bool bHasHit{ UKismetSystemLibrary::SphereTraceMulti
	(
		GetWorld(),
		m_LocationLastFrame,
		CurrentLocation,
		m_SphereCollisionRadius,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,                         
		m_IgnoredActors,
		EDrawDebugTrace::None,
		m_HitResults,
		true                            
	)};
	
	SetLastFrameLocation(CurrentLocation);

	if (bHasHit)
	{
		HandleCollision();
	}

	return bHasHit;
}

void UGOWCollisionComponent::HandleCollision()
{
	for (const FHitResult& Hit : m_HitResults)
	{
		AActor* const HitActor{ Hit.GetActor() };
		if (!HitActor) continue;
		
		if (m_IgnoredActors.Contains(HitActor)) continue;
		m_IgnoredActors.AddUnique(HitActor);

		IGOWDamageableInterface* const AxeDamageableActor(Cast<IGOWDamageableInterface>(HitActor));
		if (AxeDamageableActor && AxeDamageableActor->IsAlive())
		{
			OnDamageableHit.Broadcast(Hit);
		}
		else
		{
			OnEnvironmentHit.Broadcast(Hit);
		}
	}
}

void UGOWCollisionComponent::SetLastFrameLocation(const FVector& Location)
{
	m_LocationLastFrame = Location;
}

void UGOWCollisionComponent::AddIgnoredActor(const AActor* Actor)
{
	if (Actor)
	{
		m_IgnoredActors.AddUnique(const_cast<AActor*>(Actor));
	}
}

void UGOWCollisionComponent::ClearIgnoredActors()
{
	m_IgnoredActors.Reset();

	// After clearing the IgnoredActor array, always set again the Axe (Owner) as a ignored actor
	m_IgnoredActors.Add(GetOwner());
}