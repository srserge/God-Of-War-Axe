// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayComponents/GOWHealthComponent.h"
#include "Components/WidgetComponent.h"
#include "AI/GOWEnemyHealthBarWidget.h"

UGOWHealthComponent::UGOWHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGOWHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Sets Current Health at Max value.
	m_CurrentHealth = m_MaxHealth;

	m_bIsDead = false;

	checkf(m_HealthBarWidgetClass, TEXT("Health Bar Widget Class is not set!"));
	
	UWidgetComponent* const HealthBarWidgetComponent{ Cast<UWidgetComponent>(GetOwner()->AddComponentByClass(UWidgetComponent::StaticClass(), false, FTransform{ FQuat{}, m_HealthBarWidgetRelativeLocation, FVector{ 1.0 } }, false)) };
	HealthBarWidgetComponent->SetWidgetClass(m_HealthBarWidgetClass);
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Stores a reference to the newly created health bar widget.
	m_HealthBarWidget = Cast<UGOWEnemyHealthBarWidget>(HealthBarWidgetComponent->GetUserWidgetObject());
	m_HealthBarVisibilityDistanceSquared = FMath::Square(m_HealthBarVisibilityDistance);

	SetHealthBarVisiblity(false);
	RefreshHealthBar();
}

void UGOWHealthComponent::ReceiveDamage(float InDamage)
{
	if (m_bIsDead)
	{
		m_HealthBarWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		m_CurrentHealth = FMath::Max(0.0f, m_CurrentHealth - InDamage);
		
		OnHealthChange.ExecuteIfBound(m_CurrentHealth, m_MaxHealth);

		if (FMath::IsNearlyZero(m_CurrentHealth))
		{
			m_bIsDead = true;
			OnDeath.ExecuteIfBound();
		}
		
		SetHealthBarVisiblity(true);
		
		RefreshHealthBar();

		// Hides the health bar after a certain amount of time.
		GetWorld()->GetTimerManager().SetTimer(m_DamageHealthBarDisplayTimer, [this]()
		{
			SetHealthBarVisiblity(false);
		},
		m_DamageHealthBarDisplayDuration, false);
	}
}

void UGOWHealthComponent::SetHealthBarVisiblity(bool bVisible)
{
	if (UWorld* const World{ GetWorld() })
	{
		if (World->GetTimerManager().GetTimerRemaining(m_DamageHealthBarDisplayTimer) <= 0.0f)
		{
			m_bHealthbarIsVisible = bVisible;

			if (m_HealthBarWidget)
			{
				m_HealthBarWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			}
		}
	}
}

void UGOWHealthComponent::RefreshHealthBar()
{
	if (m_HealthBarWidget)
	{
		m_HealthBarWidget->SetPercent(m_CurrentHealth / m_MaxHealth);
	}
}


void UGOWHealthComponent::RestoreHealth()
{
	m_CurrentHealth = GetMaxHealth();
}