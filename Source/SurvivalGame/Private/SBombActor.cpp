// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivalGame.h"
#include "SBombActor.h"

ASBombActor::ASBombActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FuzePCS = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("Fuze"));
	FuzePCS->bAutoActivate = false;
	FuzePCS->bAutoDestroy = false;
	FuzePCS->AttachParent = RootComponent;

	ExplosionPCS = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("Explosion"));
	ExplosionPCS->bAutoActivate = false;
	ExplosionPCS->bAutoDestroy = false;
	ExplosionPCS->AttachParent = RootComponent;

	AudioComponent = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->bAutoDestroy = false;
	AudioComponent->AttachParent = RootComponent;

	MeshComponent->SetSimulatePhysics(true);

	MaxFuzeTime = 5.0f;
	ExplosionDamage = 100;
	ExplosionRadius = 512;

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateMovement = true;
}

void ASBombActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASBombActor, bIsFuzeActive);
	DOREPLIFETIME(ASBombActor, bExploded);
}

void ASBombActor::OnUsed(APawn* inInstigatorPawn)
{
	Super::OnUsed(inInstigatorPawn);

	if (!bIsFuzeActive)
	{
		bIsFuzeActive = true;

		SimulatedFuzeFX();

		FTimerHandle lTimer;
		GetWorld()->GetTimerManager().SetTimer(lTimer, this, &ASBombActor::OnExplode, MaxFuzeTime, false);
	}
}

void ASBombActor::OnExplode()
{
	if (!bExploded)
	{
		bExploded = true;
		SimulatedExplosion();

		TArray<AActor*> lIgnores;
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, DamageType, lIgnores, this, NULL);

		// TODO: Deal damage to objects that support it
		// TODO: Apply radial impulse to supporting objects
		// TODO: Prepare to destroy self
	}
}

void ASBombActor::SimulatedFuzeFX()
{
	if (FuzeSound)
	{
		AudioComponent->SetSound(FuzeSound);
		AudioComponent->FadeIn(0.25f);
	}

	if (FuzeFX)
	{
		FuzePCS->SetTemplate(FuzeFX);
		FuzePCS->ActivateSystem();
	}
}

void ASBombActor::SimulatedExplosion()
{
	FuzePCS->DeactivateSystem();
	AudioComponent->Stop();

	MeshComponent->SetVisibility(false, false);

	if (ExplosionSound)
	{
		AudioComponent->SetSound(ExplosionSound);
		AudioComponent->Play();
	}

	if (ExplosionFX)
	{
		ExplosionPCS->SetTemplate(ExplosionFX);
		ExplosionPCS->ActivateSystem();
	}
}

void ASBombActor::OnRep_FuzeActive()
{
	if (bIsFuzeActive && !bExploded)
	{
		SimulatedFuzeFX();
	}
}

void ASBombActor::OnRep_Exploded()
{
	if (bExploded)
	{
		SimulatedExplosion();
	}
}
