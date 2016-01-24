// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "SCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API USCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	virtual float GetMaxSpeed() const override;
};
