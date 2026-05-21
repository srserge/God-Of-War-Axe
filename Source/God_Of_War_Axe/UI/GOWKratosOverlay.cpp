// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GOWKratosOverlay.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Blueprint/SlateBlueprintLibrary.h"

void UGOWKratosOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	DisplayCrosshair(false);
}

void UGOWKratosOverlay::DisplayCrosshair(bool bDisplay)
{
	if (ensureAlwaysMsgf(CrosshairTexture, TEXT("Crosshairs Texture not set!")))
	{
		CrosshairTexture->SetVisibility(bDisplay ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

FVector2D UGOWKratosOverlay::GetCrosshairScreenPos() const
{
	if (!ensureAlwaysMsgf(CrosshairTexture, TEXT("Crosshair Texture not set!"))) return {};

	FVector2D PixelPosition{}; //The crosshair position we need it's described as "PixelPosition" by Unreal Engine.
	FVector2D ViewportPosition{}; // Empty variable, just needed to pass it into the next function as the ViewportPosition parameter.
	const FVector2D CrosshairsTextureHalfSize{ CrosshairTexture->GetDesiredSize() * 0.5f }; // This is the local offset we need to get to the center of our crosshairs texture. The origin (0, 0) will be the top left of the crosshairs texture, which we don't want.

	USlateBlueprintLibrary::LocalToViewport(CrosshairTexture, CrosshairTexture->GetCachedGeometry(), CrosshairsTextureHalfSize, PixelPosition, ViewportPosition);
	return PixelPosition;
}


