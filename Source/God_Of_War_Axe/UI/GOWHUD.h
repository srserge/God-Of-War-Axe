// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GOWHUD.generated.h"

class UGOWKratosOverlay;

UCLASS()
class GOD_OF_WAR_AXE_API AGOWHUD : public AHUD
{
	GENERATED_BODY()

public:
	void DisplayCrosshairs(bool bDisplay = true);
	
	FVector2D GetCrosshairsScreenPosition() const;

private:
	
	UPROPERTY(EditAnywhere, Category = "UI", meta = (DisplayName = "Kratos Overlay Class"))
	TSubclassOf<UUserWidget> m_KratosOverlayClass{};

	UPROPERTY()
	TObjectPtr<UGOWKratosOverlay> m_KratosOverlay{ nullptr };

private:
	virtual void BeginPlay() override;
	
};
