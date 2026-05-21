// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GOWCharacterBase.generated.h"

struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class UGOWAxeCombatComponent;
class UInputMappingContext;
class UInputAction;
class AGOWPlayerController;

UCLASS()
class GOD_OF_WAR_AXE_API AGOWCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AGOWPlayerController* CurrentPlayerController;

	/////////////////
	// Components 
	///////////////
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true", DisplayName = "Spring Arm Component"))
	TObjectPtr<USpringArmComponent> m_SpringArmComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true", DisplayName = "Camera Component"))
	TObjectPtr<UCameraComponent> m_CameraComp;

	UPROPERTY(VisibleAnywhere, Category = "Combat", meta = (DisplayName = "Axe Combat Component"))
	TObjectPtr<UGOWAxeCombatComponent> m_AxeCombatComponent{ nullptr };

	/////////////////
	// Input Actions 
	///////////////
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> Move_IA;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> Look_IA;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ThrowAxe_IA;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> Sprint_IA;

	
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (DisplayName = "Max Walk Speed"))
	float m_MaxWalkSpeed{ 200.0f };

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (DisplayName = "Max Aim Walk Speed"))
	float m_MaxAimWalkSpeed{ 200.0f };

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (DisplayName = "Max Sprint Speed"))
	float m_MaxSprintSpeed{ 800.0f };

	UPROPERTY(EditAnywhere, Category = "Properties", meta = (DisplayName = "Right Hand Socket Name"))
	FName m_RightHandSocketName{ "hand_rSocket" };

public:
	AGOWCharacterBase();
	
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return m_SpringArmComp; }
	
	FORCEINLINE UCameraComponent* GetFollowCameraComponent() const { return m_CameraComp; }
	
	FORCEINLINE const FName& GetRightHandSocketName() const { return m_RightHandSocketName; }
	
protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnMoveStarted(const FInputActionValue& Value);
	
	virtual void OnMoveTriggered(const FInputActionValue& Value);
	
	virtual void OnMoveCompleted(const FInputActionValue& Value);
	
	virtual void OnLookTriggered(const FInputActionValue& Value);
	
	virtual void OnSprintStarted(const FInputActionValue& Value);

	virtual void OnSprintTriggered(const FInputActionValue& Value);
	
	virtual void OnSprintCompleted(const FInputActionValue& Value);
};

