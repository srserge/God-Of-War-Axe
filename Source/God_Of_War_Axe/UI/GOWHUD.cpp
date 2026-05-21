// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GOWHUD.h"
#include "GOWKratosOverlay.h"

void AGOWHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!ensureAlwaysMsgf(m_KratosOverlayClass, TEXT("Kratos Overlay Class is not set!"))) return;

	m_KratosOverlay = CreateWidget<UGOWKratosOverlay>(GetOwningPlayerController(), m_KratosOverlayClass);

	if (m_KratosOverlay)
	{
		m_KratosOverlay->AddToViewport();
	}
}


void AGOWHUD::DisplayCrosshairs(bool bDisplay)
{
	if (m_KratosOverlay)
	{
		m_KratosOverlay->DisplayCrosshair(bDisplay);
	}
}

FVector2D AGOWHUD::GetCrosshairsScreenPosition() const
{
	if (!ensureAlwaysMsgf(m_KratosOverlay, TEXT("m_KratosOverlay is not set!"))) return {};
	
	return m_KratosOverlay->GetCrosshairScreenPos();
}

