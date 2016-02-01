// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivalGame.h"
#include "SConsumableActor.h"
#include "SCharacter.h"


ASConsumableActor::ASConsumableActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Nutrition = 25;
}

void ASConsumableActor::OnUsed(APawn* inInstigatorPawn)
{
	Super::OnUsed(inInstigatorPawn);

	ASCharacter* lPawn = Cast<ASCharacter>(inInstigatorPawn);
	if (lPawn)
	{
		lPawn->ConsumeFood(Nutrition);
	}

	Destroy();
}


