// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GOWEnemyHealthBarWidget.generated.h"

class UProgressBar;

UCLASS()
class GOD_OF_WAR_AXE_API UGOWEnemyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPercent(float InPercent);

protected:
	
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override; 

private:
	// Progress bar representing the current health.
	UPROPERTY(VisibleAnywhere, meta = (BindWidget, DisplayName = "Health Bar"))
	TObjectPtr<UProgressBar> HealthBar{ nullptr };
	
	// Progress bar meant for showing an animated representation of reduced health. Delay is intentional and does not represent the current health value.
	UPROPERTY(VisibleAnywhere, meta = (BindWidget, DisplayName = "Delayed Health Bar"))
	TObjectPtr<UProgressBar> DelayedHealthBar{ nullptr };

	// Sets the speed at which the DelayedHealthBar interpolates to the current health percentage.
	static constexpr float s_DelayedHealthBarInterpSpeed{ 15.0f };
	
	// Delay time before DelayedHealthBar starts to decrease to the current health percentage. The lower the value, the sooner it will start to decrease.
	static constexpr float s_DelayedHealthBarDecreaseDelay{ 1.0f };

	FTimerHandle m_DelayedHealthBarDecreaseTimer{};
	
	// If true, DelayedHealthBar starts interpolating its percentage to the current health percentage value.
	bool m_bIsDelayedBarDecreasing{ false };
};
