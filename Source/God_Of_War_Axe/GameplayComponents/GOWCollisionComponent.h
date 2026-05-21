// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GOWCollisionComponent.generated.h"

class IGOWDamageableInterface;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnvironmentHitSignature, const FHitResult& Hit);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDamageableHitSignature, const FHitResult& Hit);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GOD_OF_WAR_AXE_API UGOWCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnEnvironmentHitSignature  OnEnvironmentHit;
	FOnDamageableHitSignature   OnDamageableHit;

public:
	UGOWCollisionComponent();

	// Launches BoxTraceMulti and send a broadcast if there is a hit during Throw Axe. Returns true if there was a collision.
	bool ActivateThrowCollision();

	// Same as above but for Recall flight state. Ensures greater accuracy on hit.
	bool ActivateRecallCollision();

	// Called by the axe at the start of ThrowTick() and RecallTick() to reset tracking.
	void SetLastFrameLocation(const FVector& Location);

	// Adds the parameter actor to an array of ignored actors.
	void AddIgnoredActor(const AActor* Actor);
	
	void ClearIgnoredActors();
	
protected:
	virtual void BeginPlay() override;

private:
	// Classifies each hit result and broadcasts the appropriate delegate. Called internally by ActivateThrowCollision().
	void HandleCollision();
	
	UPROPERTY(EditAnywhere, Category = "Collision", meta = (DisplayName = "Box Half Size"))
	FVector m_BoxHalfSize{ 1.5f, 1.5f, 1.5f };

	UPROPERTY(EditAnywhere, Category = "Collision", meta = (DisplayName = "Sphere Collision Radius"))
	float m_SphereCollisionRadius { 30.f };
	
	TArray<FHitResult>      m_HitResults{};
	TArray<AActor*>         m_IgnoredActors{};   
	FVector                 m_LocationLastFrame{};
};