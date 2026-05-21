// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GOWThrowAxeAnimNotify.generated.h"

DECLARE_DELEGATE(FOnNotifiedSignature)

UCLASS()
class GOD_OF_WAR_AXE_API UGOWThrowAxeAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	UGOWThrowAxeAnimNotify();
	
	FOnNotifiedSignature OnNotified;
	
};
