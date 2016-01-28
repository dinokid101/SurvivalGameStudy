// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SUsableActor.generated.h"

UCLASS()
class SURVIVALGAME_API ASUsableActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	/** Player is looking at */
	virtual void OnBeginFocus();

	/** Player is no longer looking at */
	virtual void OnEndFocus();

	/** Called when player interacts with object */
	virtual void OnUsed(APawn* inInstigatorPawn) {};

public:
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* MeshComponent;
};
