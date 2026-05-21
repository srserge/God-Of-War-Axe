// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GOWAxeCombatComponent.generated.h"

class AGOWKratos;
class AGOWLeviathanAxe;
class AGOWHUD;

enum class EAxeCombatFlags : uint8
{
	None = 0,
	InterpingCameraSocketOffset = 1,
	InterpingFOV = 1 << 1,
	Aiming = 1 << 2,
	RecallingAxe = 1 << 3,
};
ENUM_CLASS_FLAGS(EAxeCombatFlags);

// Fired when the axe leaves Kratos' hand during the throw montage.
DECLARE_DELEGATE(FOnAxeThrownSignature);

// Fired when the axe reaches Kratos' hand after a recall.
DECLARE_DELEGATE(FOnAxeCaught);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOD_OF_WAR_AXE_API UGOWAxeCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	FOnAxeThrownSignature OnAxeThrown;
	FOnAxeCaught OnAxeCaught;
	
	using enum EAxeCombatFlags;
	
	// Spawned and owned by this component. Kept public so the hero can query axe state directly.
	UPROPERTY()
	TObjectPtr<AGOWLeviathanAxe> m_LeviathanAxe{ nullptr };
	
public:
	UGOWAxeCombatComponent();
	
	void AttachAxeToHand() const;
	
	void Aim();
	
	void StopAiming();
	
	void ThrowAttack() const;

	// Triggers the axe flight back to Kratos' hand. Uses a max duration cap to prevent infinite recall on far distances.
	void RecallAxe();

	FORCEINLINE bool IsAiming() const { return bool(m_AxeCombatFlags & Aiming); }

protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TWeakObjectPtr<AGOWKratos> m_KratosOwner{ nullptr };

	UPROPERTY()
	TWeakObjectPtr<AGOWHUD> m_KratosHUD{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Leviathan Axe", meta = (DisplayName = "Leviathan Axe Class"))
	TSubclassOf<AGOWLeviathanAxe> m_LeviathanAxeClass{};

	UPROPERTY(EditAnywhere, Category = "Leviathan Axe", meta = (DisplayName = "Light Axe Throw Speed"))
	double m_AxeThrowSpeed{ 3600.0 };

	UPROPERTY(EditAnywhere, Category = "Leviathan Axe", meta = (DisplayName = "Axe Throw Rotation Rate"))
	FRotator m_AxeThrowRotationRate{ 1400.0, 0.0, 0.0 };

	UPROPERTY(EditAnywhere, Category = "Leviathan Axe", meta = (DisplayName = "Axe Throw Rotation Lean"))
	FRotator m_AxeSpinRotation{ 0.0, 0.0, 65.0 };
	
	UPROPERTY(EditAnywhere, Category = "Leviathan Axe", meta = (DisplayName = "Light Damage Amount"))
	float m_ThrowDamageAmount{ 25.0f };

	// Max time the recall can take regardless of distance. 
	UPROPERTY(EditAnywhere, Category = "Leviathan Axe", meta = (DisplayName = "Axe Recall Max Duration"))
	float m_AxeRecallMaxDuration{ 1.0 };

	// Minimum guaranteed speed during recall.
	UPROPERTY(EditAnywhere, Category = "Leviathan Axe", meta = (DisplayName = "Axe Recall Min Speed"))
	float m_AxeRecallMinSpeed{ 2600.0f };

	UPROPERTY(EditAnywhere, Category = "Leviathan Axe", meta = (DisplayName = "Axe Recall Rotation Rate"))
	FRotator m_AxeRecallRotationRate{ 750.0, 0.0, 0.0 };

	// Offset pair for camera boom. Swapped on aim/un-aim via InterpCameraSocketOffset.
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Base Camera Socket Offset"))
	FVector m_BaseCameraSocketOffset{ 0.0, 60.0, 70.0 };

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Aim Camera Socket Offset"))
	FVector m_AimCameraSocketOffset{ 0.0, 60.0, 50.0 };

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Camera Interp Speed"))
	float m_CameraInterpSpeed{ 15.0f };

	// FOV pair. Aim FOV is closer to sell the targeting feeling.
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Base Field of View"))
	float m_BaseFOV{ 90.0f };

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (DisplayName = "Aim Field of View"))
	float m_AimFOV{ 80.0f };

	EAxeCombatFlags m_AxeCombatFlags{ None };

	// Runtime targets driven by Aim/StopAiming. Interpolated each tick toward their respective base/aim values.
	FVector m_TargetCameraSocketOffset{ m_BaseCameraSocketOffset };
	float m_TargetFOV{ m_BaseFOV };
	
	// Set before any throw so OnAxeHit knows which damage value entry to use.
	float m_DamageToApply{ 0.0f };

private:
	// Smooths the camera boom offset between base and aim positions. Driven by InterpingCameraSocketOffset flag.
	void InterpCameraSocketOffset(float DeltaTime);

	// Same as above but for FOV. Kept separate so each can be toggled independently.
	void InterpFOV(float DeltaTime);

	// Binds to the throw montage notifies. 
	void SubscribeToAnimNotifies();

	// Notify callback — detaches the axe and applies launch velocity. Only called from the anim notify binding.
	void OnAxeThrowNotified();

	// Runs on axe caught after flight completition.
	void OnAxeCaughtNotified();
	
	void OnAxeHit(const FHitResult& Hit);
};