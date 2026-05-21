// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GOWEnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "NiagaraFunctionLibrary.h"
#include "GameplayComponents/GOWHealthComponent.h"

AGOWEnemyCharacter::AGOWEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	m_HealthComponent = CreateDefaultSubobject<UGOWHealthComponent>(TEXT("HealthComponent"));

	// Sets default collision parameters for Mesh and CapsuleComponent.
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

void AGOWEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	m_bIsAlive = true;
}

void AGOWEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Overrides engine function. 
float AGOWEnemyCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	m_HealthComponent->ReceiveDamage(Damage);

	return Damage;
}

EHitDirection AGOWEnemyCharacter::CalculateHitDirection(const FVector& HitDirection) const
{
	// Dot product gives us the angle between the enemy's forward and the incoming hit.
	// Acos converts it to degrees: 0° = directly in front, 180° = directly behind.
	const double DotProduct{ FVector::DotProduct(GetActorForwardVector(), HitDirection) };
	const double AngleDegrees{ FMath::RadiansToDegrees(FMath::Acos(DotProduct)) };

	if (AngleDegrees <= m_FrontHitAngleThreshold)
	{
		return EHitDirection::Front;
	}
	
	return EHitDirection::Back;
}

bool AGOWEnemyCharacter::OnHit(const FHitResult& HitResult)
{
	const FVector HitDirection{ (HitResult.TraceStart - HitResult.TraceEnd).GetSafeNormal() };
	if (UAnimMontage* const HitMontage{ GetHitDirectionMontage(HitDirection) })
	{
		if (CalculateHitDirection(HitDirection) == EHitDirection::Front)
		{
			if (ACharacter* const PlayerCharacter{ UGameplayStatics::GetPlayerCharacter(this, 0) })
			{
				const FVector DirectionToPlayer{ (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal() };
				// Rotates enemy to face player when hit.
				SetActorRotation(FRotationMatrix::MakeFromX(DirectionToPlayer).Rotator());
			}
		}

		PlayAnimMontage(HitMontage, 1.0f, NAME_None);
	}

	// Randomly plays a Niagara System from the array
	if (m_HitSplashesVFX.Num() > 0)
	{
		const int32 RandomIndex{ FMath::RandRange(0, m_HitSplashesVFX.Num() - 1) };
		TObjectPtr<UNiagaraSystem> RandomParticle{ m_HitSplashesVFX[RandomIndex] };

		if (ensureAlwaysMsgf(RandomParticle, TEXT("Blood Impact Particle at index %d is not assigned!"), RandomIndex))
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, RandomParticle, HitResult.ImpactPoint);
		}
	}

	// Checks if the hit has killed the enemy and execute OnDeath function
	const bool bStartDeathReaction = m_HealthComponent->IsDead();
	if (bStartDeathReaction)
	{
		OnDeath();
		return false;
	}

	if (ensureAlwaysMsgf(m_HitSound, TEXT("Hit Sound is not set!")) && ensureAlwaysMsgf(m_SoundAttenuation, TEXT("Hit Sound Attenuation is not set!")))
	{
		UGameplayStatics::PlaySoundAtLocation(this, m_HitSound, GetActorLocation(), 1.0f, 1.0f, 0.0f, m_SoundAttenuation);
	}

	return true;
}

USkeletalMeshComponent* AGOWEnemyCharacter::GetSkeletalMeshComponent() const
{
	return GetSkeletalMeshComponent();
}

bool AGOWEnemyCharacter::IsAlive() const
{
	return true;
}

void AGOWEnemyCharacter::OnDeath()
{
	m_DeathTransform = GetActorTransform();

	// Disable capsule so the ragdoll isn't blocked by it.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Activate ragdoll.
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetMesh()->SetSimulatePhysics(true);

	// Destroy the actor once the ragdoll has finished playing.
	GetWorldTimerManager().SetTimer(m_RagdollTimer, this, &AGOWEnemyCharacter::OnRagdollFinished, m_RagdollDuration, false);

	if (ensureAlwaysMsgf(m_DeathSound, TEXT("Hit Sound is not set!")) && ensureAlwaysMsgf(m_SoundAttenuation, TEXT("Hit Sound Attenuation is not set!")))
	{
		UGameplayStatics::PlaySoundAtLocation(this, m_DeathSound, GetActorLocation(), 1.0f, 1.0f, 0.0f, m_SoundAttenuation);
	}
}

void AGOWEnemyCharacter::OnRagdollFinished()
{
	// Capture the data needed to respawn before this actor is destroyed.
	const FTransform SpawnTransform{ m_DeathTransform };
	UWorld* const World{ GetWorld() };
	const TSubclassOf<AGOWEnemyCharacter> EnemyClass{ GetClass() };

	// Schedule the respawn on the world timer before destroying self.
	World->GetTimerManager().SetTimer(
		m_RespawnTimer,
		[World, SpawnTransform, EnemyClass]()
		{
			FActorSpawnParameters SpawnParams{};

			World->SpawnActor<AGOWEnemyCharacter>(EnemyClass, SpawnTransform, SpawnParams);
		},
		m_RespawnDelay,
		false
	);
	
	Destroy();
}

UAnimMontage* AGOWEnemyCharacter::GetHitDirectionMontage(const FVector& HitDirection) const
{
	const EHitDirection Direction{ CalculateHitDirection(HitDirection) };
    
	const TObjectPtr<UAnimMontage>* const Montage{ m_HitDirectionMontages.Find(Direction) };
    
	// Notifies if the direction key is not present in the map at all.
	if (!ensureAlwaysMsgf(Montage, TEXT("Empty map in %s"), *GetName()))
	{
		return nullptr;
	}

	// Notifies if the key exists but its montage asset has not been assigned.
	if (!ensureAlwaysMsgf(Montage->Get(), TEXT("Montage entry exists but asset is not assigned for hit direction in %s"), *GetName()))
	{
		return nullptr;
	}
    
	return Montage ? Montage->Get() : nullptr;
}

