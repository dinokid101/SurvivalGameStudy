// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SUsableActor.h"
#include "SBombActor.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASBombActor : public ASUsableActor
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void OnUsed(APawn* inIntigatorPawn) override;

	void OnExplode();
	void SimulatedFuzeFX();
	void SimulatedExplosion();

public:
	UPROPERTY(VisibleDefaultsOnly)
	UParticleSystemComponent* ExplosionPCS;

	UPROPERTY(VisibleDefaultsOnly)
	UParticleSystemComponent* FuzePCS;

	UPROPERTY(VisibleDefaultsOnly)
	UAudioComponent* AudioComponent;

	/** Explosion particle */
	UPROPERTY(EditDefaultsOnly,	Category = "Bomb|Effects")
	UParticleSystem* ExplosionFX;

	/** Fuze particle */
	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Effects")
	UParticleSystem* FuzeFX;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Effects")
	USoundCue* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Effects")
	USoundCue* FuzeSound;

	/** Initial time on the fuze */
	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Settings")
	float MaxFuzeTime;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Settings")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Settings")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Settings")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_FuzeActive)
	bool bIsFuzeActive;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_Exploded)
	bool bExploded;

	UFUNCTION()
	void OnRep_FuzeActive();

	UFUNCTION()
	void OnRep_Exploded();
};

