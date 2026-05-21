// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/GOWLeviathanAxe.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameplayComponents/GOWCollisionComponent.h"
#include "Actors/GOWDestructibleActorBase.h"
#include "Interfaces/GOWDamageableInterface.h"

AGOWLeviathanAxe::AGOWLeviathanAxe()
{
	PrimaryActorTick.bCanEverTick = true;
	
	m_ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	m_ProjectileMovementComponent->SetUpdatedComponent(GetRootComponent());
	m_ProjectileMovementComponent->bAutoActivate = false;

	m_RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Rotating Movement Component"));
	m_RotatingMovementComponent->SetUpdatedComponent(GetRootComponent());
	m_RotatingMovementComponent->RotationRate = FRotator{ 1440.0, 0.0, 0.0 };
	m_RotatingMovementComponent->bAutoActivate = false;

	m_CollisionComponent = CreateDefaultSubobject<UGOWCollisionComponent>(TEXT("CollisionComponent"));

	m_AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	m_AudioComponent->SetupAttachment(GetRootComponent());

	m_TrailVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffect"));
	m_TrailVFX->SetupAttachment(GetRootComponent());
	m_TrailVFX->bAutoActivate = false;
}

void AGOWLeviathanAxe::BeginPlay()
{
	Super::BeginPlay();

	// Subscribes to Collision Component hit delegates. 
	m_CollisionComponent->OnDamageableHit.AddUObject(this, &AGOWLeviathanAxe::OnDamageableHit);
	m_CollisionComponent->OnEnvironmentHit.AddUObject(this, &AGOWLeviathanAxe::OnEnvironmentHit);
}

void AGOWLeviathanAxe::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	m_WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (ensureAlwaysMsgf(m_SoundAttenuation, TEXT("Sound Attenuation is not set!")))
	{
		m_AudioComponent->AttenuationSettings = m_SoundAttenuation;
	}
	
	m_CollisionComponent->ClearIgnoredActors();
	m_GravityActivationRangeSquared = m_GravityActivationRange * m_GravityActivationRange;
	m_AxeFlightData.m_RotationInterpTriggerDistSquared = FMath::Square((m_WeaponMesh->Bounds.BoxExtent.Z * 2.0) * 3.0);
	m_bShouldMaintainConstantVelocity = false;
	SetAxeState(EAxeState::Idle);
}

void AGOWLeviathanAxe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (GetAxeState())
	{
	case EAxeState::Idle:
		break;

	case EAxeState::Thrown:
		ThrownTick();
		break;

	case EAxeState::Recalling:
		RecallTick();
		break;

	default:
		break;
	}
}

void AGOWLeviathanAxe::ThrownTick()
{
	const FVector CurrentLocation{ GetActorLocation() };

	if (!m_CollisionComponent->ActivateThrowCollision())
	{
		const bool bGravityNotApplied{ !m_bShouldMaintainConstantVelocity && !m_ProjectileMovementComponent->ShouldApplyGravity() };

		// If true, apply gravity with the Projectile Movement Component.
		if (bGravityNotApplied)
		{
			if (FVector::DistSquared(m_InitialLocation, GetActorLocation()) >= m_GravityActivationRangeSquared)
			{
				m_ProjectileMovementComponent->ProjectileGravityScale = 1.0f;
			}
		}
	}

	// Aligns the start of the collision line with the axe's position in each frame.
	m_CollisionComponent->SetLastFrameLocation(CurrentLocation);
}

void AGOWLeviathanAxe::RecallTick()
{
	m_CollisionComponent->ActivateRecallCollision();
	
	//When called on recall flight state, interpolates the axe's orientation with the target rotation so that they are aligned when the axe is caught.
	if (m_AxeFlightData.m_bIsInterpolatingRotation)
	{
		InterpolationToTargetRotation();
	}
	else
	{
		const double DistSquaredToTarget{ FVector::DistSquared(GetActorLocation(), m_AxeFlightData.m_TargetLocation) };

		if (DistSquaredToTarget <= m_AxeFlightData.m_RotationInterpTriggerDistSquared)
		{
			BeginRotationInterpolation();
		}
	}

	InterpolationToTargetLocation();
}

void AGOWLeviathanAxe::Throw(const FVector& Velocity, EAxeHitBehavior HitBehavior)
{
	m_AxeHitBehavior = HitBehavior;
	m_bShouldMaintainConstantVelocity = false;
	
	m_RotatingMovementComponent->Activate();

	m_ProjectileMovementComponent->Velocity = Velocity;
	m_ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	m_ProjectileMovementComponent->Activate();

	m_InitialLocation = GetActorLocation();
	m_CollisionComponent->SetLastFrameLocation(m_InitialLocation);
	
	SetAxeState(EAxeState::Thrown);

	PlaySound(m_SpinSound);
	TryActivateNiagaraComponent(m_TrailVFX);
}

void AGOWLeviathanAxe::Recall(const FQuat& PlayerRotation, const FVector& SocketLocation, float MinSpeed,
	float FlightMaxDuration)
{
	SetAxeState(EAxeState::Recalling);
	m_CollisionComponent->ClearIgnoredActors();
	AActor* const AttachedToParentActor{ GetAttachParentActor() };
	
	// Check if recall action is triggered when the axe embeds into an object or is in the air.
	if (!AttachedToParentActor)
	{
		m_ProjectileMovementComponent->Deactivate();
	}
	else
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		m_CollisionComponent->AddIgnoredActor(AttachedToParentActor);

		m_WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		m_RotatingMovementComponent->Activate();
	}
	
	m_InitialLocation = GetActorLocation();

	m_AxeFlightData.m_TargetLocation = SocketLocation;
	m_AxeFlightData.m_TargetRotation = PlayerRotation;
	m_AxeFlightData.m_bIsInterpolatingRotation = false;

	// Determine the path the axe will follow until it reaches the player.
	const double DistanceToTarget{ FVector::Distance(m_AxeFlightData.m_TargetLocation, m_InitialLocation) };

	//Limits the difference between the derived speed value and the maximum duration flight can take. Prevents slow recalls on long distances.
	m_AxeFlightData.m_Duration = FMath::Min(DistanceToTarget / MinSpeed, FlightMaxDuration);

	// Scale arc width proportionally to distance. 
	m_AxeFlightData.m_RecallCurveRadius = DistanceToTarget * m_RecallCurveRadiusScale;

	// Adds that characteristic axe rotation when flying
	static const FQuat AxeLeanOffset{ m_AxeRecallRotation.Quaternion() };
	const FQuat LeanedRotation{ m_AxeFlightData.m_TargetRotation * AxeLeanOffset };
	SetActorRotation(LeanedRotation);
	
	GetWorldTimerManager().SetTimer(m_AxeFlightData.m_FlightTimer, this, &AGOWLeviathanAxe::OnAxeCaught, m_AxeFlightData.m_Duration, false);

	const double DistSquaredToTarget{ FVector::DistSquared(m_AxeFlightData.m_TargetLocation, m_InitialLocation) };

	// If already within interp range, skip straight to rotation interpolation without waiting for the threshold.
	// Used when the axe is within a short distance of Kratos.
	if (DistSquaredToTarget <= m_AxeFlightData.m_RotationInterpTriggerDistSquared)
	{
		BeginRotationInterpolation();
	}
	
	TryActivateNiagaraComponent(m_TrailVFX);
	PlaySound(m_RecallSound);
}

void AGOWLeviathanAxe::OnAxeCaught()
{
	m_WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetActorRotation(m_AxeFlightData.m_TargetRotation);
	SetActorLocation(m_AxeFlightData.m_TargetLocation);
	
	m_CollisionComponent->ClearIgnoredActors();
	OnFlyingAxeCaught.ExecuteIfBound();
	
	TryDeactivateNiagaraComponent(m_TrailVFX);

	m_AudioComponent->Stop();

	SetAxeState(EAxeState::Idle);

	if (ensureAlwaysMsgf(m_CatchSound, TEXT("Axe Catch Sound is not set!")))
	{
		PlaySound(m_CatchSound);
	}
}

void AGOWLeviathanAxe::OnEnvironmentHit(const FHitResult& Hit)
{
	const bool bIsDamageable{ Cast<IGOWDamageableInterface>(Hit.GetActor()) != nullptr };
	if (!bIsDamageable && GetAxeState() == EAxeState::Recalling) return;
	
	// If the hit actor is a destructible, trigger the fracture and let the axe pass through.
	if (AGOWDestructibleActorBase* const DestructibleActor{ Cast<AGOWDestructibleActorBase>(Hit.GetActor()) })
	{
		DestructibleActor->OnHit(Hit);
		return;
	}
	else
	{
		DeactivateFlightComponents();
		{
			// Sets axe's new rotation
			const FVector Z{ (-Hit.ImpactNormal).Cross(GetActorRightVector()) };
			const FRotator ImpactRotation{ FRotationMatrix::MakeFromYZ(GetActorRightVector(), Z).Rotator() };
			SetActorRotation(ImpactRotation);
			AddActorLocalRotation(FRotator{ FMath::FRandRange(-45.0, 3.0), 0.0, 0.0 });

			// Sets the new embedded location of the axe.
			ensureAlwaysMsgf(m_WeaponMesh->GetSocketByName(m_HitSocketName), TEXT("Impact Socket not set! The socket name must be set in the blueprint and must match the socket name on the mesh itself."));
			const FVector ImpactSocketLocation{ m_WeaponMesh->GetSocketLocation(m_HitSocketName) };
			SetActorLocation(GetActorLocation() + (Hit.ImpactPoint - ImpactSocketLocation));
		}

		AttachToComponent(Hit.GetComponent(), FAttachmentTransformRules::KeepWorldTransform);

		if (ensureAlwaysMsgf(m_WeaponMesh, TEXT("Axe Mesh is null!")))
		{
			m_WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}

		m_AudioComponent->Stop();

		if (ensureAlwaysMsgf(m_HitSound, TEXT("Axe Impact Sound is not set!")) && ensureAlwaysMsgf(m_SoundAttenuation, TEXT("Sound Attenuation is not set!")))
		{
			UGameplayStatics::PlaySoundAtLocation(this, m_HitSound, GetActorLocation(), 0.6f, 1.0f, 0.0f, m_SoundAttenuation);
		}		
	}
}

void AGOWLeviathanAxe::OnDamageableHit(const FHitResult& Hit)
{
	const FVector PrevHitVelocity{ m_ProjectileMovementComponent->Velocity };

	// Checks hit actor is valid and is alive.
	IGOWDamageableInterface* const DamageableActor{ Cast<IGOWDamageableInterface>(Hit.GetActor()) };
	if (!ensure(DamageableActor) && !DamageableActor->IsAlive()) return;

	switch (m_AxeHitBehavior)
	{
	case AGOWLeviathanAxe::KnockBackEnemy:
		// Don't bounce off enemy if it's recalling or hit hasn't arrived.
		if (!DamageableActor->OnHit(Hit) || GetAxeState() == EAxeState::Recalling) return;
		BounceOffEnemy(PrevHitVelocity);
		break;

	default:
		break;
	}
}


void AGOWLeviathanAxe::BounceOffEnemy(const FVector& PrevHitVelocity)
{
	const FVector AxisOfRotation{ FVector::CrossProduct(PrevHitVelocity, FVector::UpVector).GetSafeNormal() };

	SetActorRotation(m_BounceRotation);

	const FVector BounceVelocity{ PrevHitVelocity.RotateAngleAxis(m_BounceSpeedMultiplier, AxisOfRotation) * m_BounceSpeedDamping };

	Throw(BounceVelocity, m_AxeHitBehavior);

	// The axe falls quickly after bouncing.
	m_ProjectileMovementComponent->ProjectileGravityScale = BounceGravityScale;
}

void AGOWLeviathanAxe::BeginRotationInterpolation()
{
	ensureMsgf(!m_AxeFlightData.m_bIsInterpolatingRotation, TEXT("BeginRotationInterpolation called while already interping!"));

	m_AxeFlightData.m_bIsInterpolatingRotation = true;
	m_AxeFlightData.m_InitialRotation = GetActorQuat();
	m_AxeFlightData.m_RotationInterpDuration = GetWorldTimerManager().GetTimerRemaining(m_AxeFlightData.m_FlightTimer);

	m_RotatingMovementComponent->Deactivate();

	GetWorldTimerManager().SetTimer(
		m_AxeFlightData.m_RotationInterpTimer,
		m_AxeFlightData.m_RotationInterpDuration,
		false
	);
}

void AGOWLeviathanAxe::InterpolationToTargetRotation()
{
	const float TimeElapsed{ GetWorldTimerManager().GetTimerElapsed(m_AxeFlightData.m_RotationInterpTimer) };
	
	const double Alpha{ TimeElapsed / m_AxeFlightData.m_RotationInterpDuration };

	SetActorRotation(FQuat::Slerp(m_AxeFlightData.m_InitialRotation, m_AxeFlightData.m_TargetRotation, Alpha));
}

void AGOWLeviathanAxe::InterpolationToTargetLocation()
{
	const float TimeElapsed{ GetWorldTimerManager().GetTimerElapsed(m_AxeFlightData.m_FlightTimer) };
	
	const double Alpha{ TimeElapsed / m_AxeFlightData.m_Duration };

	// Sine-based lateral offset curves the axe path proportionally to recall distance.
	const FVector PathCurveOffset{ 
		m_AxeFlightData.m_TargetRotation.GetRightVector() * FMath::Sin(UE_PI * Alpha) * m_AxeFlightData.m_RecallCurveRadius 
	};

	SetActorLocation(FMath::Lerp(m_InitialLocation, m_AxeFlightData.m_TargetLocation, Alpha) + PathCurveOffset);
}

void AGOWLeviathanAxe::AlignAxeToGrip(const FQuat& PlayerRotation, const FVector& SocketLocation)
{
	m_AxeFlightData.m_TargetRotation = PlayerRotation;
	m_AxeFlightData.m_TargetLocation = SocketLocation;
}

void AGOWLeviathanAxe::SetAxeState(EAxeState InState)
{
	m_AxeState = InState;
}

void AGOWLeviathanAxe::SetRotationRate(const FRotator& RotationRate)
{
	m_RotatingMovementComponent->RotationRate = RotationRate;
}

void AGOWLeviathanAxe::DeactivateFlightComponents()
{
	m_RotatingMovementComponent->Deactivate();
	m_ProjectileMovementComponent->Deactivate();

	TryDeactivateNiagaraComponent(m_TrailVFX);

	m_AudioComponent->Stop();
}

void AGOWLeviathanAxe::PlaySound(USoundBase* Sound)
{
	if (Sound)
	{
		m_AudioComponent->SetSound(Sound);
	}

	m_AudioComponent->Play();
}

void AGOWLeviathanAxe::TryActivateNiagaraComponent(UNiagaraComponent* NiagaraSystem)
{
	if (NiagaraSystem) NiagaraSystem->Activate();
}

void AGOWLeviathanAxe::TryDeactivateNiagaraComponent(UNiagaraComponent* NiagaraSystem)
{
	if (NiagaraSystem) NiagaraSystem->Deactivate();
}