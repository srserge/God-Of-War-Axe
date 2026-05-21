// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/GOWThrowAxeAnimNotify.h"
#include "GOWLightThrowAxeAnimNotify.generated.h"

UCLASS()
class GOD_OF_WAR_AXE_API UGOWLightThrowAxeAnimNotify : public UGOWThrowAxeAnimNotify
{
	GENERATED_BODY()

public:
	UGOWLightThrowAxeAnimNotify();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
