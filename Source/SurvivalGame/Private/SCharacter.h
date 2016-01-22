// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

UCLASS()
class ASCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* CameraBoomComp;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* CameraComp;
	
public:
	ASCharacter();

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
	
	///** Server side call to update actual jumping state */
	//UFUNCTION(Server, Server, WithValidation)
	//void ServerSetIsJumping(bool inIsJumping);
	
	/** It's just reset jumping state */
	virtual void Landed(const FHitResult& inHit) override;

	/** Client call to update sprint state */
	void SetSprinting(bool inIsSprinting);

	///** Server side calll to update actual sprint state */
	//UFUNCTION(Server, Reliable, WithValidation)
	//void ServerSetSprinting(bool inIsSprinting);

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
	/* Targeting
	/***************************************************************************************/
	void OnStartTargeting();
	void OnEndTargeting();
	void SetTargeting(bool inTargeting);

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	bool IsTargeting() const;

	float GetTargetingSpeedModifier() const;

	UPROPERTY(Transient, Replicated)
	bool bIsTargeting;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingSpeedModifier;
};
