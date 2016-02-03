// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "STypes.h"
#include "SCharacter.generated.h"

UCLASS()
class SURVIVALGAME_API ASCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* CameraBoomComp;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* CameraComp;
	
public:
	virtual void PostInitializeComponents() override;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/***************************************************************************************/
	/* Movement
	/***************************************************************************************/
	virtual void MoveForward(float inVal);
	virtual void MoveRight(float inVal);

	void OnCrounchToggle();
	void OnStartJump();
	void OnStopJump();
	void OnStartSprinting();
	void OnStopSprinting();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsInitiatedJump() const;

	/** Client call to update jumping state */
	void SetIsJumping(bool inIsJumping);
	
	/** Server side call to update actual jumping state */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetIsJumping(bool inIsJumping);
	
	/** It's just reset jumping state */
	virtual void Landed(const FHitResult& inHit) override;

	/** Client call to update sprint state */
	void SetSprinting(bool inIsSprinting);

	/** Server side calll to update actual sprint state */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSprinting(bool inIsSprinting);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsSprinting() const;

	float GetSprintingSpeedModifier() const;

	/** Is character wants to run */
	UPROPERTY(Transient, Replicated)
	bool bWantsToRun;

	/** Is character currently performing a jump action? */
	UPROPERTY(Transient, Replicated)
	bool bIsJumping;

	/** Speed modidy factor when sprinting */
	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintingSpeedModifier;

	/***************************************************************************************/
	/* Object Interaction
	/***************************************************************************************/
	
	/** Use the usable actor currently in focus. */
	virtual void Use();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUse();

	class ASUsableActor* GetUsableInView();

	/** Max distance to use/focus on actors*/
	UPROPERTY(EditDefaultsOnly, Category = "ObjectInteraction")
	float MaxUseDistance;
	
	/** True only in first frame when focused on a new usable actor.*/
	bool bHasNewFocus;

	class ASUsableActor* FocusedUsableActor;
	
	/***************************************************************************************/
	/* Targeting
	/***************************************************************************************/
	void OnStartTargeting();
	void OnEndTargeting();
	void SetTargeting(bool inTargeting);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetTargeting(bool inTargeting);

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	bool IsTargeting() const;

	float GetTargetingSpeedModifier() const;

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	FRotator GetAimOffsets() const;

	UPROPERTY(Transient, Replicated)
	bool bIsTargeting;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingSpeedModifier;


	/***************************************************************************************/
	/* Stat
	/***************************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetHunger() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetMaxHunger() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	void ConsumeFood(float inAmountRestored);

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	bool IsAlive() const;

	void IncrementHunger();

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float MaxHunger;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition", Replicated)
	float Hunger;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float IncrementHungerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float IncrementHungerAmount;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float CriticalHungerThreshold;
	
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition", Replicated)
	float Health;


	/***************************************************************************************/
	/* Damage
	/***************************************************************************************/
	virtual float TakeDamage(float inDamage, struct FDamageEvent const& inDamageEvent, class AController* inEventInstigator, class AActor* inDamageCauser) override;
};
