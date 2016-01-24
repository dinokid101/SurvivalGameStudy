// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivalGame.h"
#include "SCharacterMovementComponent.h"
#include "SCharacter.h"


float USCharacterMovementComponent::GetMaxSpeed() const
{
	float lMaxSpeed = Super::GetMaxSpeed();

	const ASCharacter* lOwner = Cast<ASCharacter>(PawnOwner);
	if(lOwner)
	{ 
		if (lOwner->IsTargeting() && !lOwner->GetMovementComponent()->IsCrouching())
		{
			lMaxSpeed *= lOwner->GetTargetingSpeedModifier();
		}
		else if (lOwner->IsSprinting())
		{
			lMaxSpeed *= lOwner->GetSprintingSpeedModifier();
		}
	}
	return lMaxSpeed;
}
