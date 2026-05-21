// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/GOWCharacterBase.h"
#include "GOWKratos.generated.h"

class UGOWKratosAnimInstance;

enum class EKratosState : uint8 {Idle, AimingInPlace, AimWalking, Walking, Sprinting};

UENUM(BlueprintType)
enum class EKratosFlags : uint8 { None = 0, IsEquipped = 1, IsRecallingAxe = 1 << 1,};
ENUM_CLASS_FLAGS(EKratosFlags);

enum class EKratosInputFlags : uint8 { None = 0, IsSprintButtonDown = 1, IsAimButtonDown = 1 << 1,};
ENUM_CLASS_FLAGS(EKratosInputFlags);

UCLASS()
class GOD_OF_WAR_AXE_API AGOWKratos : public AGOWCharacterBase
{
	GENERATED_BODY()

public:
	AGOWKratos();
	
	void PlayAxeThrowAnimMontage();

	/////////////////
	// Setters 
	///////////////
	void SetStateIdle();
	
	void SetStateWalking();
	
	void SetStateAimingInPlace();
	
	void SetStateAimWalking();

	void SetStateSprinting();
	
	/////////////////
	// Getters 
	///////////////
	bool IsAnyMontagePlaying() const;
	
	bool IsAiming() const;
	
	FORCEINLINE EKratosState GetKratosState() const { return m_KratosState; }

	FORCEINLINE UAnimMontage* GetAxeThrowAnimMontage() const { return m_AxeThrowMontage; }

	FORCEINLINE bool IsEquipped() const { return static_cast<bool>(m_KratosFlags & EKratosFlags::IsEquipped); }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsRecallingAxe() const { return static_cast<bool>(m_KratosFlags & EKratosFlags::IsRecallingAxe); }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsAxeCaught() const { return m_bIsAxeCaught; }


protected:
	virtual void BeginPlay() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnThrowAttack();

	virtual void OnMoveStarted(const FInputActionValue& Value) override;

	virtual void OnMoveTriggered(const FInputActionValue& Value) override;
	
	virtual void OnMoveCompleted(const FInputActionValue& Value) override;
	
	virtual void OnSprintStarted(const FInputActionValue& Value) override;

	virtual void OnSprintTriggered(const FInputActionValue& Value) override;
	
	virtual void OnSprintCompleted(const FInputActionValue& Value) override;

	void OnAimStarted(const FInputActionValue& Value);
	
	void OnAimCompleted(const FInputActionValue& Value);

	void OnRecallAxeTriggered(const FInputActionValue& Value);
	
	void OnAxeThrown();
	
	void OnAxeRecallCompleted();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> Aim_IA;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> RecallAxe_IA;

	UPROPERTY()
	TObjectPtr<UGOWKratosAnimInstance> m_AnimInstance{ nullptr };
	
	EKratosState m_KratosState = EKratosState::Idle;

	EKratosFlags m_KratosFlags{ EKratosFlags::IsEquipped };
	
	EKratosInputFlags m_KratosInputFlags{ EKratosInputFlags::None };

	// Determines if there is a valid input direction to sprint.
	// You can only sprint in the following directions: Forward / Left Forward / Right Forward
	UPROPERTY(VisibleAnywhere, meta=(DisplayName = "Can Sprint?"))
	bool m_bValidSprintInputDirection { false };

	// Animation montage used when throwing the axe.
	UPROPERTY(EditAnywhere, Category = "Animation", meta = (DisplayName = "Axe Throw Animation Montage"))
	TObjectPtr<UAnimMontage> m_AxeThrowMontage;

	// Camera Shake activated when the axe is caught.
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (DisplayName = "Axe Catch Camera Shake"))
	TSubclassOf<UCameraShakeBase> m_AxeCatchCameraShake;

	// Minimum time the axe must be in flight before recall is allowed.
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (DisplayName = "Min Axe Flight Time Before Recall"))
	float m_MinAxeFlightTimeToRecall{ 0.15f };

	// Active while the axe is within the minimum flight window. Recall is blocked until this timer expires.
	FTimerHandle m_AxeRecallCooldown;

	// Checks whether the axe has been successfully caught to trigger the catch animation.
	bool m_bIsAxeCaught = false;

	UPROPERTY(EditAnywhere, Category = "Sound", meta = (DisplayName = "Throw Axe Effort Sounds"))
	TObjectPtr<USoundBase> m_ThrowAxeEffortSound;

private:
	// Input getter functions
	FORCEINLINE bool AimButtonDown() const { return static_cast<bool>(m_KratosInputFlags & EKratosInputFlags::IsAimButtonDown); }
	
	FORCEINLINE bool SprintButtonDown() const { return static_cast<bool>(m_KratosInputFlags & EKratosInputFlags::IsSprintButtonDown); }

	FORCEINLINE bool ValidSprintInputDirection() const { return m_bValidSprintInputDirection; }
};
