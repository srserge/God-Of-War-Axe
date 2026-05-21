// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GOWHealthComponent.generated.h"

class UGOWEnemyHealthBarWidget;

DECLARE_DELEGATE(FOnDeathSignature)
DECLARE_DELEGATE_TwoParams(FOnHealthChangeSignature, float NewHealth, float MaxHealth)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOD_OF_WAR_AXE_API UGOWHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnDeathSignature OnDeath;
	
	FOnHealthChangeSignature OnHealthChange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (DisplayName = "Max Health"))
	float m_MaxHealth = 100.0f;

public:	
	UGOWHealthComponent();
	
	virtual void ReceiveDamage(float InDamage);

	FORCEINLINE bool IsDead() const { return m_bIsDead; }
	
	FORCEINLINE float GetMaxHealth() const { return m_MaxHealth; }
	
	FORCEINLINE float GetCurrentHealth() const { return m_CurrentHealth; }

	// If health bar shoud be visibile or not.
	void SetHealthBarVisiblity(bool bVisible);

	// Restores health to its initial value. By default, current health equals max health.
	void RestoreHealth();
	
	FORCEINLINE bool IsHealthbarVisible() const { return m_bHealthbarIsVisible; }

protected:
	virtual void BeginPlay() override;

private:
	// The widget that will be added to the owner of this component
	UPROPERTY(Transient)
	TObjectPtr<UGOWEnemyHealthBarWidget> m_HealthBarWidget{ nullptr };

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Health Bar Widget Class"), Category = "Properties")
	TSubclassOf<UUserWidget> m_HealthBarWidgetClass{};

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Health Bar Widget Relative Location"), Category = "Properties")
	FVector m_HealthBarWidgetRelativeLocation{0.0, 0.0, 120.0};

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Health Bar Visibility To Target Actor Distance"))
	float m_HealthBarVisibilityDistance{ 500.0f };

	float m_HealthBarVisibilityDistanceSquared{ FMath::Square(m_HealthBarVisibilityDistance) };

	// The seconds that the health bar will be displayed after receiving damage..
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Damage Health Bar DisplayDuration"))
	float m_DamageHealthBarDisplayDuration{ 5.0f };

	FTimerHandle m_DamageHealthBarDisplayTimer;

	bool m_bHealthbarIsVisible{ false };

	UPROPERTY()
	float m_CurrentHealth = m_MaxHealth;

	UPROPERTY()
	bool m_bIsDead = false;

private:
	void RefreshHealthBar();
};
