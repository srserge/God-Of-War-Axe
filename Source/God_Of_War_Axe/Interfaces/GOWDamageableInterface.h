// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GOWDamageableInterface.generated.h"

/*The different types of actions that can be performed when hit by a Leviathan Axe.*/
enum class EAxeHitType : uint8
{
	EAHT_None,
	EAHT_KnockBack,
	EAHT_Max
};

DECLARE_DELEGATE(FOnFreezeCompletedSignature);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGOWDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

class GOD_OF_WAR_AXE_API IGOWDamageableInterface
{
	GENERATED_BODY()

public:
	
	// Determines the reaction to being hit by Leviathan Axe.
	virtual bool OnHit(const FHitResult& HitResult) = 0;
	
	virtual USkeletalMeshComponent* GetSkeletalMeshComponent() const = 0;
	
	virtual bool IsAlive() const = 0;
};
