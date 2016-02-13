// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

UENUM()
enum class EWeaponState
{
	Idle,
	Firing,
	Equipping,
	Reloading
};

UCLASS(ABSTRACT, Blueprintable)
class SURVIVALGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Get weapon mesh */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	USkeletalMeshComponent* GetWeaponMesh() const;

	/** Get pawn owner */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class ASCharacter* GetPawnOwner() const;
	
	/** Set weapon's owning pawn */
	void SetOwningPawn(class ASCharacter* inNewOwner);	
	
	bool IsAttachedToPawn() const;

	FORCEINLINE EInventorySlot GetStorageSlot()
	{
		return StorageSlot;
	}

protected:
	/** Attaches weapon mesh to pawn */
	void AttachMeshToPawn(EInventorySlot inSlot = EInventorySlot::Hands);

	/** Detaches weapon mesh from pawn. */
	void DetachMeshFromPawn();

	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	class ASCharacter* MyPawn;

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* Mesh;

	/** The character socket to store this item at. */
	EInventorySlot StorageSlot;

	UFUNCTION()
	void OnRep_MyPawn();


	/************************************************************************/
	/* Equip Handling                                                       */
	/************************************************************************/
public:
	void OnEquip();
	virtual void OnUnEquip();
	virtual void OnEquipFinished();
	
	bool IsEquipped() const;
	float GetEquipStartedTime() const;
	float GetEquipDuration() const;

	virtual void OnEnterInventory(ASCharacter* inNewOwner);
	virtual void OnLeaveInventory();

	/** Last time when this weapon was switched to */
	float EquipStartedTime;

	/** How much time weapon needs to be equipped */
	float EquipDuration;

	bool bIsEquipped;
	bool bPendingEquip;

	FTimerHandle EquipFinishedTimerHandle;

	/************************************************************************/
	/* Fire & Damage Handling                                               */
	/************************************************************************/
public:
	void StartFire();
	void StopFire();
	EWeaponState GetCurrentState() const;

	FTimerHandle HandleFiringTimerHandle;

protected:
	bool CanFire() const;
	FVector GetAdjustedAim() const;
	FVector GetCameraDamageStartLocation(const FVector& inAimDir) const;
	FHitResult WeaponTrace(const FVector& inTraceFrom, const FVector& inTraceTo) const;

	virtual void FireWeapon() PURE_VIRTUAL(ASWeapon::FireWeapon, );

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float TimeBetweenShots;

private:
	void SetWeaponState(EWeaponState inNewState);
	void DetermineWeaponState();

	virtual void HandleFiring();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStartFire();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStopFire();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerHanleFiring();

	void OnBurstStarted();
	void OnBurstFinished();

	bool bWantsToFire;
	EWeaponState CurrentState;
	bool bRefiring;
	float LastFireTime;

	/************************************************************************/
	/* Simulation & FX                                                      */
	/************************************************************************/
protected:
	virtual void SimulateWeaponFire();
	virtual void StopSimulateWeaponFire();
	FVector GetMuzzleLocation() const;
	FVector GetMuzzleDirection() const;
	UAudioComponent* PlayWeaponSound(USoundCue* inSountToPlay);
	float PlayWepaonAnimation(UAnimMontage* inAnimation, float inPlayRate = 1.f, FName StartSectionName = NAME_None);
	void StopWeaponAnimation(UAnimMontage* inAnimation);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* EquipSound;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* MuzzleFX;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	FName MuzzleAttachPoint;

	UPROPERTY(Transient, Category = "FX")
	UParticleSystem* MuzzlePCS;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* EquipAnim;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FireAnim;

	bool bPlayingFireAnim;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCounter)
	int32 BurstCounter;

	UFUNCTION()
	void OnRep_BurstCounter;
};
