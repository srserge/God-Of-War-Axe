// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GOWPlayerController.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class AGOWKratos;
UCLASS()
class GOD_OF_WAR_AXE_API AGOWPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	TObjectPtr<AGOWKratos> CurrentCharacter;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> CurrentMappingContext;

public:
	virtual void SetupInputComponent() override;

protected:
	virtual void OnPossess(APawn* InPawn) override;
};
