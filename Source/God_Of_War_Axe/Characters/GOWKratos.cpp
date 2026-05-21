// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GOWKratos.h"
#include "Characters/Animations/GOWKratosAnimInstance.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GOWAxeCombatComponent.h"

AGOWKratos::AGOWKratos()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGOWKratos::BeginPlay()
{
	Super::BeginPlay();

	m_AnimInstance = Cast<UGOWKratosAnimInstance>(GetMesh()->GetAnimInstance());

	SetStateIdle();

	// Get notified for when the axe is thrown.
	m_AxeCombatComponent->OnAxeThrown.BindUObject(this, &AGOWKratos::OnAxeThrown);
	
	// Get notified for when the axe is recalled back to the characters hand.
	m_AxeCombatComponent->OnAxeCaught.BindUObject(this, &AGOWKratos::OnAxeRecallCompleted);
	
	m_KratosFlags = EKratosFlags::IsEquipped;
}

void AGOWKratos::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		// Move override
		EnhancedInputComponent->BindAction(Move_IA.Get(),
		ETriggerEvent::Started, this, &AGOWKratos::OnMoveStarted);

		//Sprint
		EnhancedInputComponent->BindAction(Sprint_IA.Get(),
			ETriggerEvent::Started, this, &AGOWKratos::OnSprintStarted);
		EnhancedInputComponent->BindAction(Sprint_IA.Get(),
			ETriggerEvent::Triggered, this, &AGOWKratos::OnSprintTriggered);
		EnhancedInputComponent->BindAction(Sprint_IA.Get(),
			ETriggerEvent::Completed, this, &AGOWKratos::OnSprintCompleted);

		EnhancedInputComponent->BindAction(Move_IA.Get(),
		ETriggerEvent::Completed, this, &AGOWKratos::OnMoveCompleted);
		//Aim Axe
		EnhancedInputComponent->BindAction(Aim_IA.Get(),
		ETriggerEvent::Started, this, &AGOWKratos::OnAimStarted);
		EnhancedInputComponent->BindAction(Aim_IA.Get(),
		ETriggerEvent::Completed, this, &AGOWKratos::OnAimCompleted);
		
		//Throw Axe
		EnhancedInputComponent->BindAction(ThrowAxe_IA.Get(),
		ETriggerEvent::Started, this, &AGOWKratos::OnThrowAttack);

		//Recall Axe
		EnhancedInputComponent->BindAction(RecallAxe_IA.Get(),
		ETriggerEvent::Started, this, &AGOWKratos::OnRecallAxeTriggered);
	}
}

// The Throw Axe Montage uses the UpperBody slot starting from the “spine_02” bone to blend correctly with the locomotion animations.
void AGOWKratos::PlayAxeThrowAnimMontage()
{
	if (IsAnyMontagePlaying()) return;

	if (ensureAlwaysMsgf(m_AxeThrowMontage, TEXT("Axe Throw montage is not set!")))
	{
		PlayAnimMontage(m_AxeThrowMontage, 1.0f);
	}
}

void AGOWKratos::OnMoveStarted(const FInputActionValue& Value)
{
	const FVector2D MoveInput{ Value.Get<FVector2D>() };
	m_bValidSprintInputDirection = MoveInput.Y > 0.1f; 

	switch (m_KratosState)
	{
	case EKratosState::Idle:
		if (SprintButtonDown() && m_bValidSprintInputDirection)
		{
			SetStateSprinting();
		}
		else
		{
			SetStateWalking();
		}
		break;

	case EKratosState::AimingInPlace:
		if (SprintButtonDown() && m_bValidSprintInputDirection)
		{
			m_AxeCombatComponent->StopAiming();
			SetStateSprinting();
		}
		else
		{
			SetStateAimWalking();
		}
		break;

	default:
		break;
	}
}

void AGOWKratos::OnMoveTriggered(const FInputActionValue& Value)
{
	Super::OnMoveTriggered(Value);

	const FVector2D MoveInput{ Value.Get<FVector2D>() };
	m_bValidSprintInputDirection = MoveInput.Y > 0.1f; 
}

void AGOWKratos::OnMoveCompleted(const FInputActionValue& Value)
{
	switch (m_KratosState)
	{
	case EKratosState::AimWalking:
		SetStateAimingInPlace();
		break;
	
	case EKratosState::Walking:
		SetStateIdle();
		break;
	
	case EKratosState::Sprinting:
		SetStateIdle();
		break;
	
	default:
		break;
	}
}

void AGOWKratos::OnSprintStarted(const FInputActionValue& Value)
{
	m_KratosInputFlags |= EKratosInputFlags::IsSprintButtonDown;
	
	Super::OnSprintStarted(Value);
	
	switch (m_KratosState)
	{
	case EKratosState::Idle:
		break;
		
	case EKratosState::AimingInPlace:
		break;

	case EKratosState::AimWalking:
		m_AxeCombatComponent->StopAiming();
		SetStateSprinting();
		break;

	case EKratosState::Walking:
		if (SprintButtonDown())
		{
			SetStateSprinting();
		}
		else
		{
			SetStateWalking();
		}
		break;
	
	default:
		break;
	}
}

void AGOWKratos::OnSprintTriggered(const FInputActionValue& Value)
{
	Super::OnSprintTriggered(Value);

	if (m_KratosState != EKratosState::Sprinting) return;

	if (!ValidSprintInputDirection())
	{
		SetStateWalking();
	}
}

void AGOWKratos::OnSprintCompleted(const FInputActionValue& Value)
{
	Super::OnSprintCompleted(Value);

	m_KratosInputFlags &= ~EKratosInputFlags::IsSprintButtonDown;

	switch (m_KratosState)
	{
	case EKratosState::Sprinting:
		if (IsEquipped() && AimButtonDown())
		{
			m_AxeCombatComponent->Aim();
			SetStateAimWalking();
		}
		else
		{
			SetStateWalking();
		}

		break;

	default:
		break;
	}
}

void AGOWKratos::OnAimStarted(const FInputActionValue& Value)
{
	m_KratosInputFlags |= EKratosInputFlags::IsAimButtonDown;

	m_AxeCombatComponent->Aim();

	if (!IsEquipped()) return;

	switch (m_KratosState)
	{
	case EKratosState::Idle:
		SetStateAimingInPlace();
		break;
	
	case EKratosState::Walking:
		SetStateAimWalking();
		break;
	
	case EKratosState::Sprinting:
		SetStateAimWalking();
		break;
	
	default:
		break;
	}
}

void AGOWKratos::OnAimCompleted(const FInputActionValue& Value)
{
	m_KratosInputFlags &= ~EKratosInputFlags::IsAimButtonDown;
	
	m_AxeCombatComponent->StopAiming();
	
	if (!IsEquipped()) return;
	
	switch (m_KratosState)
	{
	case EKratosState::Idle:
		break;
	
	case EKratosState::AimingInPlace:
		SetStateIdle();
		break;
	
	case EKratosState::AimWalking:
		if (SprintButtonDown())
		{
			SetStateSprinting();
		}
		else
		{
			SetStateWalking();
		}
		break;
	
	default:
		break;
	}
}

void AGOWKratos::OnThrowAttack()
{
	if (IsEquipped())
	{
		m_AxeCombatComponent->ThrowAttack();
	}
}

void AGOWKratos::OnRecallAxeTriggered(const FInputActionValue& Value)
{
	//The AxeRecallCooldown timer must be inactive in order to recall the axe.
	const bool bFlightWindowExpired{ !GetWorldTimerManager().IsTimerActive(m_AxeRecallCooldown) };
    
	if (!IsRecallingAxe() && !IsEquipped() && bFlightWindowExpired) 
	{
		m_KratosFlags |= EKratosFlags::IsRecallingAxe;
		m_AxeCombatComponent->RecallAxe();
	}
}

void AGOWKratos::OnAxeThrown()
{
	m_KratosFlags &= ~EKratosFlags::IsEquipped;
	m_bIsAxeCaught = false;

	// Starts Axe Recall Cooldown in case player wants to cancel throw action.
	GetWorldTimerManager().SetTimer(m_AxeRecallCooldown, m_MinAxeFlightTimeToRecall, false);

	UGameplayStatics::PlaySoundAtLocation(this, m_ThrowAxeEffortSound, GetActorLocation(), 1.0f, 1.0f, 0.0f);
	
	switch (m_KratosState)
	{
	case EKratosState::AimingInPlace:
		SetStateIdle();
		break;

	case EKratosState::AimWalking:
		if (SprintButtonDown())
		{
			SetStateSprinting();
		}
		else
		{
			SetStateWalking();
		}
		break;

	default:
		break;
	}
}

void AGOWKratos::OnAxeRecallCompleted()
{
	m_KratosFlags &= ~EKratosFlags::IsRecallingAxe;
	m_KratosFlags |= EKratosFlags::IsEquipped;
	m_bIsAxeCaught = true;

	// Reproduces the camera shake effect when the axe is received to add more weight to the animation.
	if (ensureAlwaysMsgf(m_AxeCatchCameraShake, TEXT("Axe Catch Camera Shake is not set!")))
	{
		APlayerController* const PlayerController{ CastChecked<APlayerController>(GetController()) };
		PlayerController->ClientStartCameraShake(m_AxeCatchCameraShake);
	}

	if (AimButtonDown())
	{
		switch (m_KratosState)
		{
		case EKratosState::Idle:
			SetStateAimingInPlace();

			break;

		case EKratosState::Walking:
			SetStateAimWalking();

			break;

		default:
			break;
		}
	}
}

// Setters
void AGOWKratos::SetStateIdle()
{
	m_KratosState = EKratosState::Idle;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AGOWKratos::SetStateWalking()
{
	m_KratosState = EKratosState::Walking;
	GetCharacterMovement()->MaxWalkSpeed = m_MaxWalkSpeed;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AGOWKratos::SetStateAimingInPlace()
{
	m_KratosState = EKratosState::AimingInPlace;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AGOWKratos::SetStateAimWalking()
{
	m_KratosState = EKratosState::AimWalking;
	GetCharacterMovement()->MaxWalkSpeed = m_MaxAimWalkSpeed;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AGOWKratos::SetStateSprinting()
{
	m_KratosState = EKratosState::Sprinting;
	GetCharacterMovement()->MaxWalkSpeed = m_MaxSprintSpeed;
	GetCharacterMovement()->MaxAcceleration = 1400;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;	
}

//Getters
bool AGOWKratos::IsAnyMontagePlaying() const
{
	if (!m_AnimInstance) return false;

	return m_AnimInstance->IsAnyMontagePlaying();
}

bool AGOWKratos::IsAiming() const
{
	return (m_KratosState == EKratosState::AimingInPlace || m_KratosState == EKratosState::AimWalking);
}
