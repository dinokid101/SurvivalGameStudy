// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivalGame.h"
#include "SPlayerCameraManager.h"
#include "SCharacter.h"


ASPlayerCameraManager::ASPlayerCameraManager(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NormalFOV = 90.0f;
	TargetingFOV = 65.0f;

	ViewPitchMin = -80.0f;
	ViewPitchMax = 87.0f;
	bAlwaysApplyModifiers = true;
}

void ASPlayerCameraManager::UpdateCamera(float inDeltaTime)
{
	ASCharacter* lPawn = PCOwner ? Cast<ASCharacter>(PCOwner->GetPawn()) : NULL;
	if (lPawn)
	{
		const float lTargetFOV = lPawn->IsTargeting() ? TargetingFOV : NormalFOV;
		DefaultFOV = FMath::FInterpTo(DefaultFOV, lTargetFOV, inDeltaTime, 20.0f);
		SetFOV(DefaultFOV);
	}

	Super::UpdateCamera(inDeltaTime);
}

