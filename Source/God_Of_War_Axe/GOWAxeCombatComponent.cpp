// Fill out your copyright notice in the Description page of Project Settings.


#include "GOWAxeCombatComponent.h"
#include "Characters/GOWKratos.h"
#include "Combat/GOWLeviathanAxe.h"
#include "UI/GOWHUD.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Combat/GOWThrowAxeAnimNotify.h"
#include "GameplayComponents/GOWCollisionComponent.h"

UGOWAxeCombatComponent::UGOWAxeCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGOWAxeCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	m_KratosOwner = Cast<AGOWKratos>(GetOwner());

	APlayerController* const PlayerController{ CastChecked<APlayerController>(m_KratosOwner->GetController()) };
	m_KratosHUD = Cast<AGOWHUD>(PlayerController->GetHUD());

	m_KratosOwner->GetSpringArmComponent()->SocketOffset = m_BaseCameraSocketOffset;
	m_KratosOwner->GetFollowCameraComponent()->FieldOfView = m_BaseFOV;


	if (ensureAlwaysMsgf(m_LeviathanAxeClass, TEXT("Leviathan Axe Class is not set!")))
	{
		m_LeviathanAxe = GetWorld()->SpawnActor<AGOWLeviathanAxe>(m_LeviathanAxeClass);
		m_LeviathanAxe->SetInstigator(Cast<APawn>(GetOwner()));
		m_LeviathanAxe->OnFlyingAxeCaught.BindUObject(this, &UGOWAxeCombatComponent::OnAxeCaughtNotified);
		m_LeviathanAxe->FindComponentByClass<UGOWCollisionComponent>()->OnDamageableHit.AddUObject(this, &UGOWAxeCombatComponent::OnAxeHit);

		AttachAxeToHand();
	}

	SubscribeToAnimNotifies();

	m_AxeCombatFlags = None;
}

void UGOWAxeCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (EnumHasAnyFlags(m_AxeCombatFlags, InterpingCameraSocketOffset))
	{
		InterpCameraSocketOffset(DeltaTime);
	}

	if (EnumHasAnyFlags(m_AxeCombatFlags,InterpingFOV))
	{
		InterpFOV(DeltaTime);
	}

	if (EnumHasAnyFlags(m_AxeCombatFlags, RecallingAxe))
	{
		if (USkeletalMeshComponent* const CharacterMesh{ m_KratosOwner->GetMesh() })
		{
			const FVector HeroRightHandLocation{ CharacterMesh->GetSocketLocation(m_KratosOwner->GetRightHandSocketName()) };
			m_LeviathanAxe->AlignAxeToGrip(m_KratosOwner->GetActorQuat(), HeroRightHandLocation);
		}
	}
}

void UGOWAxeCombatComponent::AttachAxeToHand() const
{
	m_LeviathanAxe->AttachToComponent(m_KratosOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, m_KratosOwner->GetRightHandSocketName());
}

void UGOWAxeCombatComponent::Aim()
{
	if (m_KratosHUD.IsValid())
	{
		m_KratosHUD->DisplayCrosshairs();
	}
	
	m_TargetCameraSocketOffset = m_AimCameraSocketOffset;
	m_TargetFOV = m_AimFOV;
	
	m_AxeCombatFlags |= InterpingCameraSocketOffset | InterpingFOV | Aiming;
}

void UGOWAxeCombatComponent::StopAiming()
{
	m_KratosHUD->DisplayCrosshairs(false);
	
	m_TargetCameraSocketOffset = m_BaseCameraSocketOffset;
	m_TargetFOV = m_BaseFOV;
	
	m_AxeCombatFlags |= InterpingCameraSocketOffset | InterpingFOV;
	m_AxeCombatFlags &= ~Aiming;

	m_KratosOwner->GetMesh()->GetAnimInstance()->StopAllMontages(0.1f);
}

void UGOWAxeCombatComponent::ThrowAttack() const
{
	if (IsAiming() && !m_KratosOwner->IsAnyMontagePlaying())
	{
		m_KratosOwner->PlayAxeThrowAnimMontage();
	}
}

void UGOWAxeCombatComponent::RecallAxe()
{
	USkeletalMeshComponent* const HeroMesh{ m_KratosOwner->GetMesh() };
	if (!HeroMesh) return;

	UAnimInstance* const HeroAnimInstance{ HeroMesh->GetAnimInstance() };

	if (HeroAnimInstance)
	{
		constexpr float MontageBlendOut{ 0.2f };

		HeroAnimInstance->StopAllMontages(MontageBlendOut);
	}

	const FVector HeroRightHandLocation{ HeroMesh->GetSocketLocation(m_KratosOwner->GetRightHandSocketName()) };

	m_LeviathanAxe->SetRotationRate(m_AxeRecallRotationRate);

	m_DamageToApply = m_ThrowDamageAmount;

	m_LeviathanAxe->Recall(m_KratosOwner->GetActorQuat(), HeroRightHandLocation, m_AxeRecallMinSpeed, m_AxeRecallMaxDuration);

	m_AxeCombatFlags |= RecallingAxe;
}

void UGOWAxeCombatComponent::InterpCameraSocketOffset(float DeltaTime)
{
	USpringArmComponent* const CameraBoom{ m_KratosOwner->GetSpringArmComponent() };

	if (!CameraBoom) return;

	FVector& CurrentCameraOffset{ CameraBoom->SocketOffset };
	CurrentCameraOffset = FMath::VInterpTo(CurrentCameraOffset, m_TargetCameraSocketOffset, DeltaTime, m_CameraInterpSpeed);

	const double CurrentProjTargetSocketOffset{ CurrentCameraOffset.Dot(m_TargetCameraSocketOffset) };

	if (FMath::IsNearlyEqual(CurrentProjTargetSocketOffset, 1.0))
	{
		m_AxeCombatFlags &= ~InterpingCameraSocketOffset;
	}
}

void UGOWAxeCombatComponent::InterpFOV(float DeltaTime)
{
	UCameraComponent* const Camera{ m_KratosOwner->GetFollowCameraComponent() };

	if (!Camera) return;

	float& CurrentFOV{ Camera->FieldOfView };
	CurrentFOV = FMath::FInterpTo(CurrentFOV, m_TargetFOV, DeltaTime, m_CameraInterpSpeed);

	if (FMath::IsNearlyEqual(CurrentFOV, m_TargetFOV))
	{
		m_AxeCombatFlags &= ~InterpingFOV;
	}
}

void UGOWAxeCombatComponent::SubscribeToAnimNotifies()
{
	for (const FAnimNotifyEvent& Event : m_KratosOwner->GetAxeThrowAnimMontage()->Notifies)
	{
		if (UGOWThrowAxeAnimNotify* const AxeThrowNotify{ Cast<UGOWThrowAxeAnimNotify>(Event.Notify) })
		{
			AxeThrowNotify->OnNotified.BindUObject(this, &UGOWAxeCombatComponent::OnAxeThrowNotified);
		}
	}
}

void UGOWAxeCombatComponent::OnAxeThrowNotified()
{
	if (!GEngine || !GEngine->GameViewport) return;

	m_LeviathanAxe->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	m_LeviathanAxe->SetActorRotation(m_KratosOwner->GetControlRotation());

	// Adds that characteristic rotation to the axe when thrown.
	m_LeviathanAxe->AddActorLocalRotation(m_AxeSpinRotation); 

	// Converts crosshairs screen position to world space. Works regardless of where the crosshair is placed in the viewport.
	FVector CrosshairsWorldPosition{};
	FVector CrosshairsWorldDirection{};
	UGameplayStatics::DeprojectScreenToWorld(Cast<APlayerController>(m_KratosOwner->GetController()), m_KratosHUD->GetCrosshairsScreenPosition(), CrosshairsWorldPosition, CrosshairsWorldDirection);

	// Position the axe ahead of the reticle at a fixed distance from camera. Preserves the depth offset from the previous frame.
	const FVector CrosshairsToAxe{ m_LeviathanAxe->GetActorLocation() - CrosshairsWorldPosition };
	const double CrosshairsToAxeProjCrosshairsDirection{ CrosshairsToAxe.Dot(CrosshairsWorldDirection) };
	const FVector TargetAxeLaunchLocation{ CrosshairsWorldPosition + CrosshairsWorldDirection * CrosshairsToAxeProjCrosshairsDirection };
	m_LeviathanAxe->SetActorLocation(TargetAxeLaunchLocation);

	FVector Velocity{ CrosshairsWorldDirection };
	Velocity *= m_AxeThrowSpeed;

	m_LeviathanAxe->SetRotationRate(m_AxeThrowRotationRate);

	m_DamageToApply = m_ThrowDamageAmount;
	
	AGOWLeviathanAxe::EAxeHitBehavior ImpactActionRequest{ AGOWLeviathanAxe::KnockBackEnemy };

	m_LeviathanAxe->Throw(Velocity,ImpactActionRequest);

	OnAxeThrown.ExecuteIfBound();
}


void UGOWAxeCombatComponent::OnAxeCaughtNotified()
{
	AttachAxeToHand();
	m_AxeCombatFlags &= ~RecallingAxe;

	OnAxeCaught.ExecuteIfBound();
}

void UGOWAxeCombatComponent::OnAxeHit(const FHitResult& Hit)
{
	UGameplayStatics::ApplyDamage(Hit.GetActor(), m_DamageToApply, m_KratosOwner->GetController(), m_KratosOwner.Get(), UDamageType::StaticClass());
}

