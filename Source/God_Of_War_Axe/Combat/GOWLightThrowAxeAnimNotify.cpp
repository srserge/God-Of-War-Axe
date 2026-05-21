// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/GOWLightThrowAxeAnimNotify.h"

UGOWLightThrowAxeAnimNotify::UGOWLightThrowAxeAnimNotify()
{
	
}

void UGOWLightThrowAxeAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference); //May not be used?

	OnNotified.ExecuteIfBound();
}
