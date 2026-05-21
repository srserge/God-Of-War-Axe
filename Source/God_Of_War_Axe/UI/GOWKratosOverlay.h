// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GOWKratosOverlay.generated.h"

class UImage;
class UProgressBar;

UCLASS()
class GOD_OF_WAR_AXE_API UGOWKratosOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void DisplayCrosshair(bool bDisplay = true);

	FVector2D GetCrosshairScreenPos() const;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CrosshairTexture{ nullptr };

private:
	virtual void NativeConstruct() override;
};
