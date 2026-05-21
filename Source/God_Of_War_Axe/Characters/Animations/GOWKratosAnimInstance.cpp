// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Animations/GOWKratosAnimInstance.h"
#include "Characters/GOWKratos.h"
#include <Kismet/KismetMathLibrary.h>
#include "KismetAnimationLibrary.h"

void UGOWKratosAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	m_KratosCharacter = Cast<AGOWKratos>(TryGetPawnOwner());
	m_GroundSpeed = 0.0f;
	m_Direction = 0.0f;
	m_bIsAiming = false;
	m_KratosAnimationState = EKratosAnimationState::Idle;
}

void UGOWKratosAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!m_KratosCharacter.IsValid())
	{
		m_KratosCharacter = Cast<AGOWKratos>(TryGetPawnOwner());

		if (!m_KratosCharacter.IsValid())
		{
			return;
		}
	}

	const EKratosState KratosState = m_KratosCharacter->GetKratosState();

	switch (KratosState)
	{
	case EKratosState::Idle:
		m_KratosAnimationState = EKratosAnimationState::Idle;
		break;
		
	case EKratosState::Walking:
		m_KratosAnimationState = EKratosAnimationState::Walking;
		break;

	case EKratosState::Sprinting:
		m_KratosAnimationState = EKratosAnimationState::Sprinting;
		break;

	case EKratosState::AimWalking:
		m_KratosAnimationState = EKratosAnimationState::AimWalking;
		break;

	case EKratosState::AimingInPlace:
		m_KratosAnimationState = EKratosAnimationState::AimingInPlace;
		break;

	default:
		break;
	}

	const FVector KratosVelocity = m_KratosCharacter->GetVelocity();
	const FRotator AimRotation = m_KratosCharacter->GetBaseAimRotation();
	m_GroundSpeed = KratosVelocity.Size();
	m_Direction = UKismetAnimationLibrary::CalculateDirection(KratosVelocity, m_KratosCharacter->GetActorRotation());
	
	m_bIsAiming = m_KratosCharacter->IsAiming();
	m_bIsRecallingAxe = m_KratosCharacter->IsRecallingAxe();
	m_bIsAxeCaught = m_KratosCharacter->IsAxeCaught();

	// Aiming in place calculations.
	if (m_bIsAiming)
	{
		const FRotator DeltaRotation{ UKismetMathLibrary::NormalizedDeltaRotator(m_KratosCharacter->GetActorRotation(), AimRotation)};
		m_AimOffset = UKismetMathLibrary::FClamp(DeltaRotation.Pitch, -60, 60);
	}
	else { m_AimOffset = 0; }
}
