// Fill out your copyright notice in the Description page of Project Settings.


//#include "GOWCharacterBase.h"
#include "Characters/GOWCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Modes/GOWPlayerController.h"
#include "GOWAxeCombatComponent.h"

AGOWCharacterBase::AGOWCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// Avoid rotating the character with the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	TObjectPtr<UCharacterMovementComponent> MovementComponent = this->GetCharacterMovement();
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = m_MaxWalkSpeed;

	this->CurrentPlayerController = Cast<AGOWPlayerController>(this->GetController());

	m_SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArmComponent"));
	m_SpringArmComp->SetupAttachment(this->RootComponent.Get());
	m_SpringArmComp->TargetArmLength = 150;
	m_SpringArmComp->bUsePawnControlRotation = true;

	m_CameraComp = CreateDefaultSubobject<UCameraComponent>(FName("CameraComponent"));
	m_CameraComp->SetupAttachment(this->m_SpringArmComp.Get(), USpringArmComponent::SocketName);
	m_CameraComp->bUsePawnControlRotation = false;
	
	m_AxeCombatComponent = CreateDefaultSubobject<UGOWAxeCombatComponent>(TEXT("AxeCombatComponent"));

	GetCharacterMovement()->MaxAcceleration = 1000;
	GetCharacterMovement()->MaxWalkSpeed = m_MaxWalkSpeed;

}

void AGOWCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGOWCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent) 
	{
		//Move
		EnhancedInputComponent->BindAction(Move_IA.Get(),
			ETriggerEvent::Triggered, this, &AGOWCharacterBase::OnMoveTriggered);
		EnhancedInputComponent->BindAction(Move_IA.Get(),
			ETriggerEvent::Started, this, &AGOWCharacterBase::OnMoveStarted);
		EnhancedInputComponent->BindAction(Move_IA.Get(),
			ETriggerEvent::Completed, this, &AGOWCharacterBase::OnMoveCompleted);
		//Look
		EnhancedInputComponent->BindAction(Look_IA.Get(),
			ETriggerEvent::Triggered, this, &AGOWCharacterBase::OnLookTriggered);
		//Sprint
		EnhancedInputComponent->BindAction(Sprint_IA.Get(),
			ETriggerEvent::Started, this, &AGOWCharacterBase::OnSprintStarted);
		EnhancedInputComponent->BindAction(Sprint_IA.Get(),
			ETriggerEvent::Triggered, this, &AGOWCharacterBase::OnSprintTriggered);
		EnhancedInputComponent->BindAction(Sprint_IA.Get(),
			ETriggerEvent::Completed, this, &AGOWCharacterBase::OnSprintCompleted);
	}
}

void AGOWCharacterBase::OnMoveTriggered(const FInputActionValue& Value)
{
	const FVector2d MovementVector = Value.Get<FVector2d>();

	const FRotator Rotation = this->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	this->AddMovementInput(RightDirection, MovementVector.X);
	this->AddMovementInput(ForwardDirection, MovementVector.Y);
}

// Empty functions in this base character. Each child class will override the function to add its own logic.
void AGOWCharacterBase::OnMoveStarted(const FInputActionValue& Value)
{
}

void AGOWCharacterBase::OnMoveCompleted(const FInputActionValue& Value)
{
}

void AGOWCharacterBase::OnLookTriggered(const FInputActionValue& Value)
{
	const FVector2d LookAxisVector = Value.Get<FVector2d>();

	this->AddControllerYawInput(LookAxisVector.X);
	this->AddControllerPitchInput(LookAxisVector.Y);
}

void AGOWCharacterBase::OnSprintStarted(const FInputActionValue& Value)
{
}

void AGOWCharacterBase::OnSprintTriggered(const FInputActionValue& Value)
{
}

void AGOWCharacterBase::OnSprintCompleted(const FInputActionValue& Value)
{
}
