// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GOWDestructibleActorBase.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class GOD_OF_WAR_AXE_API AGOWDestructibleActorBase : public AActor
{
	GENERATED_BODY()
	
public:
	
	AGOWDestructibleActorBase();
	
	// Triggers the Chaos fracture simulation on the GeometryCollection when hitted.
	void OnHit(const FHitResult& HitResult);

	UFUNCTION()
	void OnGeometryBreak(const FChaosBreakEvent& BreakEvent);

protected:

	virtual void BeginPlay() override;

private:
	// Allow Destruction within Chaos system and defines the settings that determine how it breaks apart.
	UPROPERTY(VisibleAnywhere, Category = "Actor", meta = (DisplayName = "Geometry Collection"))
	TObjectPtr<UGeometryCollectionComponent> m_GeometryCollectionComponent;

	// Used as collision trigger. Once triggered it's hidden and destroyed for better integration with the Chaos simulation.
	// Hidden by default in the editor.
	UPROPERTY(VisibleAnywhere, Category = "Actor", meta = (DisplayName = "Static Mesh"))
	TObjectPtr<UStaticMeshComponent> m_StaticMeshComponent;

	// Impulse magnitude applied at the impact point to drive the fracture.
	UPROPERTY(EditAnywhere, Category = "Destructible", meta = (DisplayName = "Impact Impulse Magnitude"))
	float m_ImpactImpulseMagnitude{ 500.f };
	
	// Prevents multiple simultaneous axe hits from triggering the simulation twice.
	bool m_bHasFractured{ false };

	// Delay required for Chaos to complete the simulation before destroying the actor.
	UPROPERTY(EditAnywhere, Category = "Destructible", meta = (DisplayName = "Destroy Delay After Break"))
	float m_DestroyDelay{ 10.f };
		
	UPROPERTY(EditAnywhere, Category = "Sound", meta = (DisplayName = "Barrel Smash Sound"))
	TObjectPtr<USoundBase> m_BarrelSmashSound{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Sound", meta = (DisplayName = "Sound Attenuation"))
	TObjectPtr<USoundAttenuation> m_SoundAttenuation{ nullptr };
};
