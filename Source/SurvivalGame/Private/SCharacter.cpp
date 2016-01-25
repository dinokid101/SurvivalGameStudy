// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivalGame.h"
#include "SCharacter.h"
#include "SCharacterMovementComponent.h"


ASCharacter::ASCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->GravityScale = 1.5f;
	MoveComp->JumpZVelocity = 620;
	MoveComp->bCanWalkOffLedgesWhenCrouching = true;
	MoveComp->MaxWalkSpeedCrouched = 200;
	MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;

	CameraBoomComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoomComp->SocketOffset = FVector(0, 35, 0);
	CameraBoomComp->TargetOffset = FVector(0, 0, 55);
	CameraBoomComp->bUsePawnControlRotation = true;
	CameraBoomComp->AttachParent = GetRootComponent();

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->AttachParent = CameraBoomComp;

	SprintingSpeedModifier = 2.5f;
	TargetingSpeedModifier = 0.5f;

	Health = 100;

	MaxHunger = 100;
	Hunger = 0;
	IncrementHungerInterval = 5.0f;
	IncrementHungerAmount = 1.0f;
	CriticalHungerThreshold = 90.0f;
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

// Called every frame
void ASCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (bWantsToRun && !IsSprinting())
		SetSprinting(true);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	// Movement
	InputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);

	InputComponent->BindAction("SprintHold", IE_Pressed, this, &ASCharacter::OnStartSprinting);
	InputComponent->BindAction("SprintHold", IE_Released, this, &ASCharacter::OnStopSprinting);
	InputComponent->BindAction("CrouchToggle", IE_Released, this, &ASCharacter::OnCrounchToggle);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::OnStopJump);

	// Weapon
	InputComponent->BindAction("Targeting", IE_Pressed, this, &ASCharacter::OnStartTargeting);
	InputComponent->BindAction("Targeting", IE_Released, this, &ASCharacter::OnEndTargeting);
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASCharacter, bWantsToRun, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ASCharacter, bIsJumping, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ASCharacter, bIsTargeting, COND_SkipOwner);

	DOREPLIFETIME(ASCharacter, Health);
	DOREPLIFETIME(ASCharacter, Hunger);
}

void ASCharacter::MoveForward(float inVal)
{
	UCharacterMovementComponent* lMovementComp = GetCharacterMovement();
	if (Controller && lMovementComp && inVal != 0.f)
	{
		const bool lbLimitRotation = (lMovementComp->IsMovingOnGround() || lMovementComp->IsFalling());
		const FRotator lRotation = lbLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector lDirection = FRotationMatrix(lRotation).GetScaledAxis(EAxis::X);
		AddMovementInput(lDirection, inVal);
	}
}

void ASCharacter::MoveRight(float inVal)
{
	if (inVal != 0.f)
	{
		const FRotator lRotation = GetActorRotation();
		const FVector lDirection = FRotationMatrix(lRotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(lDirection, inVal);
	}
}

void ASCharacter::OnCrounchToggle()
{
	if (CanCrouch())
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

void ASCharacter::OnStartJump()
{
	bPressedJump = true;
	SetIsJumping(true);
}

void ASCharacter::OnStopJump()
{
	bPressedJump = false;
}

void ASCharacter::OnStartSprinting()
{
	SetSprinting(true);
}

void ASCharacter::OnStopSprinting()
{
	SetSprinting(false);
}

bool ASCharacter::IsInitiatedJump() const
{
	return bIsJumping;
}

void ASCharacter::SetIsJumping(bool inIsJumping)
{
	if (bIsCrouched && inIsJumping)
	{
		UnCrouch();
	}
	else
	{
		bIsJumping = inIsJumping;
	}

	if (Role < ROLE_Authority)
	{
		ServerSetIsJumping(inIsJumping);
	}
}

void ASCharacter::ServerSetIsJumping_Implementation(bool inIsJumping)
{
	SetIsJumping(inIsJumping);
}

bool ASCharacter::ServerSetIsJumping_Validate(bool inIsJumping)
{
	return true;
}

void ASCharacter::Landed(const FHitResult & inHit)
{
	Super::Landed(inHit);
	SetIsJumping(false);
}

void ASCharacter::SetSprinting(bool inIsSprinting)
{
	bWantsToRun = inIsSprinting;
	if (bIsCrouched)
	{
		UnCrouch();
	}

	// TODO: Stop weapon fire

	if (Role < ROLE_Authority)
	{
		ServerSetSprinting(inIsSprinting);
	}
}

void ASCharacter::ServerSetSprinting_Implementation(bool inIsSprinting)
{
	SetSprinting(inIsSprinting);
}

bool ASCharacter::ServerSetSprinting_Validate(bool inIsSprinting)
{
	return true;
}

bool ASCharacter::IsSprinting() const
{
	if (!GetCharacterMovement())
		return false;
	return bWantsToRun && !GetVelocity().IsZero() &&
		   (GetVelocity().GetSafeNormal2D() | GetActorRotation().Vector()) > 0.8;
}

float ASCharacter::GetSprintingSpeedModifier() const
{
	return SprintingSpeedModifier;
}

void ASCharacter::OnStartTargeting()
{
	SetTargeting(true);
}

void ASCharacter::OnEndTargeting()
{
	SetTargeting(false);
}

void ASCharacter::SetTargeting(bool inTargeting)
{
	bIsTargeting = inTargeting;

	if (Role < ROLE_Authority)
	{
		ServerSetTargeting(inTargeting);
	}
}

void ASCharacter::ServerSetTargeting_Implementation(bool inTargeting)
{
	SetTargeting(inTargeting);
}

bool ASCharacter::ServerSetTargeting_Validate(bool inTargeting)
{
	return true;
}

bool ASCharacter::IsTargeting() const
{
	return bIsTargeting;
}

float ASCharacter::GetTargetingSpeedModifier() const
{
	return TargetingSpeedModifier;
}

FRotator ASCharacter::GetAimOffsets() const
{
	const FVector lAimDirWS = GetBaseAimRotation().Vector();
	const FVector lAimDirLS = ActorToWorld().InverseTransformVectorNoScale(lAimDirWS);
	const FRotator lAimRotLS = lAimDirLS.Rotation();
	return lAimRotLS;
}

float ASCharacter::GetHealth() const
{
	return Health;
}

float ASCharacter::GetHunger() const
{
	return Hunger;
}

float ASCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<ASCharacter>()->Health;
}

float ASCharacter::GetMaxHunger() const
{
	return MaxHunger;
}

void ASCharacter::ConsumeFood(float inAmountRestored)
{
	Hunger = FMath::Clamp(Hunger - inAmountRestored, 0.0f, GetMaxHunger());
	Health = FMath::Clamp(Health + inAmountRestored, 0.0f, GetMaxHealth());

	// TODO: Notify to HUD
}

bool ASCharacter::IsAlive() const
{
	return (Health > 0);
}

void ASCharacter::IncrementHunger()
{
	Hunger = FMath::Clamp(Hunger + IncrementHungerAmount, 0.0f, GetMaxHunger());
	if (Hunger > CriticalHungerThreshold)
	{
		// Apply damage to self
		TakeDamage(10.0f, FDamageEvent(), GetController(), this);
	}
}

float ASCharacter::TakeDamage(float inDamage, FDamageEvent const & inDamageEvent, AController * inEventInstigator, AActor * inDamageCauser)
{
	if (!IsAlive())
		return 0.0f;

	const float lActualDamage = Super::TakeDamage(inDamage, inDamageEvent, inEventInstigator, inDamageCauser);
	if (lActualDamage > 0.0f)
	{
		Health -= lActualDamage;
		if (IsAlive())
		{
			// TODO: play hit event
		}
		else
		{
			// TODO: play death event
		}
	}
	return lActualDamage;
}
