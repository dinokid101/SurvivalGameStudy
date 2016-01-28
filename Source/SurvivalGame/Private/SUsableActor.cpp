// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivalGame.h"
#include "SUsableActor.h"


ASUsableActor::ASUsableActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	RootComponent = MeshComponent;
}

void ASUsableActor::OnBeginFocus()
{
	// Used by custom PostProcess to render outline
	MeshComponent->SetRenderCustomDepth(true);
}

void ASUsableActor::OnEndFocus()
{
	// Used by custom PostProcess to render outline
	MeshComponent->SetRenderCustomDepth(false);
}

