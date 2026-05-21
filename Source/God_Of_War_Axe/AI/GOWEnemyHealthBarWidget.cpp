// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GOWEnemyHealthBarWidget.h"
#include "Components/ProgressBar.h"

void UGOWEnemyHealthBarWidget::SetPercent(float InPercent)
{
	if (ensureAlwaysMsgf(HealthBar, TEXT("Health Progress Bar is not set in the widget!")))
	{
		HealthBar->SetPercent(InPercent);
	}
	
	// Sets the timer to interpolate DelayedHealthBar percent to the actual health value once completed. 
	GetWorld()->GetTimerManager().SetTimer(m_DelayedHealthBarDecreaseTimer, [this]() 
	{
		m_bIsDelayedBarDecreasing = true;
	}, s_DelayedHealthBarDecreaseDelay, false);
}

void UGOWEnemyHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	m_bIsDelayedBarDecreasing = false;
}

void UGOWEnemyHealthBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Checks if DelayedHealthBar interpolation can be started.
	if (m_bIsDelayedBarDecreasing)
	{
		const float TargetPercent{ HealthBar->GetPercent() };
		float CurrentPercent{ DelayedHealthBar->GetPercent() };
		CurrentPercent = FMath::FInterpTo(CurrentPercent, TargetPercent, InDeltaTime, s_DelayedHealthBarInterpSpeed);
		
		if (ensureAlwaysMsgf(HealthBar, TEXT("Delayed Progress Bar is not set in the widget!")))
		{
			DelayedHealthBar->SetPercent(CurrentPercent);
		}

		// Stops decreasing the DelayedHealthBar if the value is nearly equal to target.
		if (FMath::IsNearlyEqual(CurrentPercent, TargetPercent))
		{
			m_bIsDelayedBarDecreasing = false;
		}
	}
}
