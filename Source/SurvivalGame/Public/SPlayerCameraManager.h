// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "SPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_UCLASS_BODY()

public:
	virtual void UpdateCamera(float inDeltaTime) override;
	
public:
	float NormalFOV;
	float TargetingFOV;	
};
