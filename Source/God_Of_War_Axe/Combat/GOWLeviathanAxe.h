// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/GOWWeaponBase.h"
#include "GOWLeviathanAxe.generated.h"

class UProjectileMovementComponent;
class URotatingMovementComponent;
class USkeletalMeshSocket;
class UNiagaraComponent;
class IGOWDamageableInterface;
class UGOWCollisionComponent;

DECLARE_DELEGATE(FOnFlyingAxeCaughtSignature);

UCLASS()
class GOD_OF_WAR_AXE_API AGOWLeviathanAxe : public AGOWWeaponBase
{
	GENERATED_BODY()

public:
	enum class EAxeState : uint8
	{
		None,
		Idle,
		Thrown,
		Recalling,
		Max,
	};

	enum EAxeHitBehavior : uint8
	{
		KnockBackEnemy,
	};

	// Fired when Kratos catches the axe after a recall completes.
	FOnFlyingAxeCaughtSignature OnFlyingAxeCaught;
	
public:
	AGOWLeviathanAxe();
	
	// Sets new rotation rate for the axe RotatingMovementComponent.
	void SetRotationRate(const FRotator& RotationRate);

	// Launches the axe with the given velocity. HitTypeRequest determines behavior on hit (damage, pin, etc).
	void Throw(const FVector& Velocity, EAxeHitBehavior HitBehavior);
		
	// Calls the axe to return to Kratos. MinSpeed prevents deceleration near the socket; FlightMaxDuration caps total travel time.
	void Recall(const FQuat& PlayerRotation, const FVector& SocketLocation, float MinSpeed, float FlightMaxDuration);
		
	// Aligns axe orientation and world position based on player rotation and hand socket location.	
	void AlignAxeToGrip(const FQuat& PlayerRotation, const FVector& SocketLocation);

private:
	void ThrownTick();

	void RecallTick();

	void OnAxeCaught();
	
	void BounceOffEnemy(const FVector& PrevHitVelocity);

	// Smoothly rotates the axe toward its target orientation using Slerp over the remaining flight duration.
	void InterpolationToTargetRotation();
	
	// Advances the axe along its curved flight path each tick. Applies a sine-based lateral offset to fake an arc.
	void InterpolationToTargetLocation();

	// Kicks off the rotation interp phase. Deactivates spinning movement and snapshots the current rotation as the interp start point.
	void BeginRotationInterpolation();

	void SetAxeState(EAxeState InState);

private:
	/////////////////
	// Components
	///////////////
	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (DisplayName = "Projectile Movement Component"))
	TObjectPtr<UProjectileMovementComponent> m_ProjectileMovementComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (DisplayName = "Rotating Movement Component"))
	TObjectPtr<URotatingMovementComponent> m_RotatingMovementComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components", DisplayName="CollisionComponent")
	TObjectPtr<UGOWCollisionComponent> m_CollisionComponent;

	UPROPERTY(EditAnywhere, Category = "Components", meta = (DisplayName = "Audio Component"))
	TObjectPtr<UAudioComponent> m_AudioComponent{ nullptr };

	// Defines current Axe State of Kratos Character.
	EAxeState m_AxeState{ EAxeState::Idle };

	// Distance at which the axe switches from projectile flight to gravity-driven fall.
	UPROPERTY(EditAnywhere, Category = "Properties", meta = (DisplayName = "Gravity Trigger Distance"))
	float m_GravityActivationRange{ 800.0 };

	// Squared version of m_GravityActivationRange.
	float m_GravityActivationRangeSquared{};

	UPROPERTY(EditAnywhere, Category = "Properties", meta = (DisplayName = "Recall Rotation Lean"))
	FRotator m_AxeRecallRotation{ 0.0, 0.0, 135.0 };

	UPROPERTY(EditAnywhere, Category = "Properties", meta = (DisplayName = "Recall Curve Radius Scale"))
	float m_RecallCurveRadiusScale{ 0.18 };

	// Increases the speed of bouncing off enemies.
	UPROPERTY(EditAnywhere, Category = "Bounce", meta = (DisplayName = "Bounce Speed Multiplier"))
	float m_BounceSpeedMultiplier{ 70.0 };
	
	//Decelerates speed.
	UPROPERTY(EditAnywhere, Category = "Bounce", meta = (DisplayName = "Bounce Speed Damping"))
	float m_BounceSpeedDamping{ 0.5 };

	UPROPERTY(EditAnywhere, Category = "Bounce", meta = (DisplayName = "Bounce Gravity Scale"))
	float BounceGravityScale{ 6.0f };
	
	// Axe rotation when bouncing.
	UPROPERTY(EditAnywhere, Category = "Bounce", meta = (DisplayName = "Bounce Axe Rotation"))
	FRotator m_BounceRotation{ 10.0, 0.0, 0.0 };

	// Mesh socket that defines the point at which will attach to the object it's colliding with.
	UPROPERTY(EditAnywhere, Category = "Properties", meta = (DisplayName = "Hit Socket Name"))
	FName m_HitSocketName{ "HitSocket" };

	// Should the axe maintain a constant velocity throughout its entire flight.
	bool m_bShouldMaintainConstantVelocity{ false };
	
	FVector m_InitialLocation{};

	// Encapsulates all runtime data needed to drive the axe through a throw or recall flight.
	struct FAxeFlightData
	{
		// Captures the rotation of the axe at the start of rotation interp. Used as the Slerp start point.
		FQuat m_InitialRotation{};

		FQuat m_TargetRotation{};
		FVector m_TargetLocation{};

		// Master timer for the full flight. Duration drives the Lerp alpha in InterpolateToTargetLocation.
		FTimerHandle m_FlightTimer{};

		// Secondary timer started by BeginRotationInterpolation. Runs in parallel with m_FlightTimer.
		FTimerHandle m_RotationInterpTimer{};

		// Lateral arc amplitude. Applied as an offset in InterpolateToTargetLocation.
		double m_RecallCurveRadius{};

		float m_Duration{};

		// Mirrors the remaining flight time at the moment BeginRotationInterpolation is called.
		float m_RotationInterpDuration{};

		// Squared distance threshold at which BeginRotationInterpolation is triggered during recall.
		float m_RotationInterpTriggerDistSquared{};

		bool m_bIsInterpolatingRotation{ false };
	};

	// The variable that stores the previous struct.
	FAxeFlightData m_AxeFlightData{};

	EAxeHitBehavior m_AxeHitBehavior{};

	/////////////////
	// Sounds
	///////////////
	UPROPERTY(EditAnywhere, Category = "Sound", meta = (DisplayName = "Sound Attenuation"))
	TObjectPtr<USoundAttenuation> m_SoundAttenuation{ nullptr };
	
	UPROPERTY(EditAnywhere, Category = "Sound", meta = (DisplayName = "Spin Sound"))
	TObjectPtr<USoundBase> m_SpinSound{ nullptr };
	
	UPROPERTY(EditAnywhere, Category = "Sound", meta = (DisplayName = "Bounce Sound"))
	TObjectPtr<USoundBase> m_BounceSound{ nullptr };
	
	UPROPERTY(EditAnywhere, Category = "Sound", meta = (DisplayName = "Recall Sound"))
	TObjectPtr<USoundBase> m_RecallSound{ nullptr };
	
	UPROPERTY(EditAnywhere, Category = "Sound", meta = (DisplayName = "Hit Sound"))
	TObjectPtr<USoundBase> m_HitSound{ nullptr };
	
	UPROPERTY(EditAnywhere, Category = "Sound", meta = (DisplayName = "Catch Sound"))
	TObjectPtr<USoundBase> m_CatchSound{ nullptr };

	// Leviathan Axe trail effect activated when throwing and recalling.
	UPROPERTY(VisibleAnywhere, Category = "VFX", meta = (DisplayName = "Trail Effect"))
	TObjectPtr<UNiagaraComponent> m_TrailVFX{ nullptr };

private:

	// Returns the Current Axe State.
	FORCEINLINE EAxeState GetAxeState() const { return m_AxeState; }

	// Disables all movement and rotation components while the axe is in flight. Re-enabled on Throw and Recall.
	void DeactivateFlightComponents();

	// Handles axe hit against non-damageable objects. The axe embeds into the hit object using the HitSocket as the hit point.
	void OnEnvironmentHit(const FHitResult& Hit);

	// Handles axe hit against a damageable actor. Applies damage and bounces off enemy as the post-hit behavior.
	void OnDamageableHit(const FHitResult& Hit);

	// Activates the Niagara system if valid. Prevents spawning multiple instances of the same effect.
	void TryActivateNiagaraComponent(UNiagaraComponent* NiagaraSystem);

	// Stops and deactivates the Niagara system if it's not active.
	void TryDeactivateNiagaraComponent(UNiagaraComponent* NiagaraSystem);

	// Plays the given sound at the axe's location.
	void PlaySound(USoundBase* Sound = nullptr);

protected:
	virtual void BeginPlay() override;
	
	virtual void PostInitializeComponents() override;
	
	virtual void Tick(float DeltaTime) override;
};
