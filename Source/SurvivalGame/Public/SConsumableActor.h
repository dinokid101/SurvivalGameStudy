// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SPickupActor.h"
#include "SConsumableActor.generated.h"

/**
 * Base class for consumable(food) items. Comsumable by players to restore energy.
 */
UCLASS(ABSTRACT)
class SURVIVALGAME_API ASConsumableActor : public ASPickupActor
{
	GENERATED_UCLASS_BODY()

public:
	virtual void OnUsed(APawn* inInstigatorPawn) override;

protected:
	/**  Amount of health restored and hunger reduced when consumed. */
	UPROPERTY(EditDefaultsOnly, Category = "Consumable")
	float Nutrition;	
};
