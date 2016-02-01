// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SUsableActor.h"
#include "SPickupActor.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class SURVIVALGAME_API ASPickupActor : public ASUsableActor
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void OnUsed(APawn* inInstigatorPawn) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* PickupSound;
};
