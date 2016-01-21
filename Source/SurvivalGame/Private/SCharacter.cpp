// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivalGame.h"
#include "SCharacter.h"

ASCharacter::ASCharacter()
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


}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

// Called every frame
void ASCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);

	InputComponent->BindAction("SprintHold", IE_Pressed, this, &ASCharacter::OnStartSprinting);
	InputComponent->BindAction("SprintHold", IE_Released, this, &ASCharacter::OnStopSprinting);
	InputComponent->BindAction("CrouchToggle", IE_Released, this, &ASCharacter::OnCrounchToggle);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::OnStopJump);
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(ASCharacter, bWantsToRun, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(ASCharacter, bIsJumping, COND_SkipOwner);
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
}

//void ASCharacter::ServerSetIsJumping(bool inIsJumping)
//{
//}
//
//void ASCharacter::ServerSetIsJumping_Implementation(bool inIsJumping)
//{
//	SetIsJumping(inIsJumping);
//}
//
//bool ASCharacter::ServerSetIsJumping_Validate(bool inIsJumping)
//{
//	return true;
//}

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

	//if (Role < ROLE_Authority)
	//{
	//	ServerSetSprinting(inIsSprinting);
	//}
}

//void ASCharacter::ServerSetSprinting(bool inIsSprinting)
//{}
//
//void ASCharacter::ServerSetSprinting_Implementation(bool inIsSprinting)
//{
//	SetSprinting(inIsSprinting);
//}
//
//bool ASCharacter::ServerSetSprinting_Validate(bool inIsSprinting)
//{
//	return true;
//}

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
