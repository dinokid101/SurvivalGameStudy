// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivalGame.h"
#include "SPickupActor.h"


ASPickupActor::ASPickupActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MeshComponent->SetSimulatePhysics(true);

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateMovement = true;
}

void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASPickupActor::OnUsed(APawn* inInstigatorPawn)
{
	Super::OnUsed(inInstigatorPawn);
	UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
}
