// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GOWWeaponBase.generated.h"

UCLASS()
class GOD_OF_WAR_AXE_API AGOWWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AGOWWeaponBase();
	
	UPROPERTY(VisibleAnywhere, Category = "Properties", meta = (DisplayName = "Weapon Mesh"))
	
	TObjectPtr<UStaticMeshComponent> m_WeaponMesh; 
};
