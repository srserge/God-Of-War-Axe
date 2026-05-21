// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/GOWDamageableInterface.h"
#include "GOWEnemyCharacter.generated.h"

class UGOWHealthComponent;
class UAnimMontage;
class UNiagaraSystem;

UENUM()
enum class EHitDirection : uint8
{
	Front   UMETA(DisplayName = "Front"),
	Back    UMETA(DisplayName = "Back"),
};

UCLASS()
class GOD_OF_WAR_AXE_API AGOWEnemyCharacter : public ACharacter, public IGOWDamageableInterface
{
	GENERATED_BODY()

	
public: 
	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (DisplayName = "Health Component"))
	TObjectPtr<UGOWHealthComponent> m_HealthComponent {nullptr};

	UPROPERTY(EditAnywhere, Category = "Animation", meta = (DisplayName = "Death Montage"))
	TObjectPtr<UAnimMontage> m_DeathMontage {nullptr};

	UPROPERTY(EditAnywhere, Category = "Animation", meta = (DisplayName = "Hit Direction Montages"))
	TMap<EHitDirection, TObjectPtr<UAnimMontage>> m_HitDirectionMontages;


public:
	AGOWEnemyCharacter();
	
	// Returns true if it has been hit and determines the animation based on the angle (front or back), false if the enemy is dead.
	virtual bool OnHit(const FHitResult& HitResult) override;

	virtual USkeletalMeshComponent* GetSkeletalMeshComponent() const override;

	// Inherited and overrided from the Damageable Interface.
	virtual bool IsAlive() const override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	
	virtual void PostInitializeComponents() override;

private:
	bool m_bIsAlive = true;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (DisplayName = "Front Hit Angle Threshold"))
	float m_FrontHitAngleThreshold{ 90.f };

	// Delay before the enemy respawns at its death location.
	UPROPERTY(EditAnywhere, Category = "Death", meta = (DisplayName = "Respawn Delay"))
	float m_RespawnDelay{ 5.f };
	
	// Stores the world transform at death — used as the respawn transform.
	FTransform m_DeathTransform{};

	// Timer that triggers the respawn after m_RespawnDelay seconds.
	FTimerHandle m_RespawnTimer;
	FTimerHandle m_RagdollTimer;

	// Duration the ragdoll plays before the actor is destroyed.
	UPROPERTY(EditAnywhere, Category = "Death", meta = (DisplayName = "Ragdoll Duration"))
	float m_RagdollDuration{ 4.f };

	UPROPERTY(EditAnywhere, Category = "Sound", meta = (DisplayName = "Hit Sound"))
	TObjectPtr<USoundBase> m_HitSound{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Sound", meta = (DisplayName = "Death Sound"))
	TObjectPtr<USoundBase> m_DeathSound{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Sound", meta = (DisplayName = "Sound Attenuation"))
	TObjectPtr<USoundAttenuation> m_SoundAttenuation{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Effects", meta = (DisplayName = "Hit Splashes VFX"))
	TArray<TObjectPtr<UNiagaraSystem>> m_HitSplashesVFX;

private: 
	void OnDeath();

	// Destroys this actor and schedules a respawn at m_DeathTransform.
	void OnRagdollFinished();

	UAnimMontage* GetHitDirectionMontage(const FVector& HitDirection) const;

	EHitDirection CalculateHitDirection(const FVector& HitDirection) const;
};
