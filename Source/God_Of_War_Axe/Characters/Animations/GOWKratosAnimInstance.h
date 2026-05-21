// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GOWKratosAnimInstance.generated.h"

class AGOWKratos;
class UAnimSequence;

UENUM(BlueprintType)
enum class EKratosAnimationState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Walking UMETA(DisplayName = "Walking"),
	Sprinting UMETA(DisplayName = "Sprinting"),
	AimingInPlace UMETA(DisplayName = "Aiming In Place"),
	AimWalking UMETA(DisplayName = "Aim Walking"),
};

UCLASS()
class GOD_OF_WAR_AXE_API UGOWKratosAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	// Kratos character direction. Used at locomotion BlendSpace.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", DisplayName = "Direction"))
	float m_Direction;
	
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", DisplayName = "Kratos Character"))
	TWeakObjectPtr<AGOWKratos> m_KratosCharacter;

	/** Animation used when the axe is recalled*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", DisplayName = "Recall Axe Animation"))
	TObjectPtr<UAnimSequence> m_RecallAxeAnim;

	/** Animation used when Kratos is aiming*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", DisplayName = "Aim Axe Animation"))
	TObjectPtr<UAnimSequence> m_AimAxeAnim;

	/** Animation used when the axe is caught*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", DisplayName = "Catch Axe Animation"))
	TObjectPtr<UAnimSequence> m_CatchAxeAnim;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", DisplayName = "Aim Offset"))
	float m_AimOffset{ 0.0f };
	
	/** Speed that combined with Direction, determines locomotion BlendSpace animation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", DisplayName = "Ground Speed"))
	float m_GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", DisplayName = "Is Aiming?"))
	bool m_bIsAiming = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", DisplayName = "Is Recalling Axe?"))
	bool m_bIsRecallingAxe = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", DisplayName = "Is Axe Caught?"))
	bool m_bIsAxeCaught = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", DisplayName = "Kratos Animation State"))
	EKratosAnimationState m_KratosAnimationState { EKratosAnimationState::Idle };
};
