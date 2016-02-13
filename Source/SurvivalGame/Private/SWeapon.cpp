// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivalGame.h"
#include "SWeapon.h"
#include "SCharacter.h"


// Sets default values
ASWeapon::ASWeapon()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh3P"));
	Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh->bReceivesDecals = true;
	Mesh->CastShadow = true;
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	RootComponent = Mesh;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	StorageSlot = EInventorySlot::Primary;
	bIsEquipped = false;
	CurrentState = EWeaponState::Idle;
	MuzzleAttachPoint = TEXT("MuzzleFlashSocket");
}

void ASWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	DetachMeshFromPawn();
	StopSimulateWeaponFire();
}

USkeletalMeshComponent * ASWeapon::GetWeaponMesh() const
{
	return Mesh;
}

ASCharacter * ASWeapon::GetPawnOwner() const
{
	return MyPawn;
}

void ASWeapon::SetOwningPawn(ASCharacter * inNewOwner)
{
	if (MyPawn != inNewOwner)
	{
		Instigator = inNewOwner;
		MyPawn = inNewOwner;
		SetOwner(inNewOwner);
	}
}

bool ASWeapon::IsAttachedToPawn() const
{
	return bIsEquipped || bPendingEquip;
}

void ASWeapon::AttachMeshToPawn(EInventorySlot inSlot)
{
	if (MyPawn)
	{
		DetachMeshFromPawn();

		USkeletalMeshComponent* pawnMesh = MyPawn->GetMesh();
		//FName attachPoint = MyPawn->GetInventoryAttachPoint(inSlot);
		//Mesh->SetHiddenInGame(false);
		//Mesh->AttachTo(pawnMesh, attachPoint, EAttachLocation::SnapToTarget);
	}
}

void ASWeapon::DetachMeshFromPawn()
{
	Mesh->DetachFromParent();
	Mesh->SetHiddenInGame(true);
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASWeapon, MyPawn);
	DOREPLIFETIME_CONDITION(ASWeapon, BurstCounter, COND_SkipOwner);
}

void ASWeapon::OnRep_MyPawn()
{
	if (MyPawn)
	{
		OnEnterInventory(MyPawn);
	}
	else
	{
		OnLeaveInventory();
	}
}

void ASWeapon::OnEquip()
{
	AttachMeshToPawn();

	bPendingEquip = true;
	DetermineWeaponState();

	float Duration = PlayWeaponAnimation(EquipAnim);
	if (Duration <= 0.0f)
	{
		Duration = 0.5f;
	}

	EquipStartedTime = GetWorld()->TimeSeconds;
	EquipDuration = Duration;

	GetWorldTimerManager().SetTimer(EquipFinishedTimerHandle, this, &ASWeapon::OnEquipFinished,
									Duration, false);

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		PlayWeaponSound(EquipSound);
	}
}

void ASWeapon::OnUnEquip()
{
	if (bPendingEquip)
	{
		StopWeaponAnimation(EquipAnim);
		bPendingEquip = false;
		GetWorldTimerManager().ClearTimer(EquipFinishedTimerHandle);
	}

	StopFire();
	AttachMeshToPawn(StorageSlot);
	bIsEquipped = false;

	DetermineWeaponState();
}

void ASWeapon::OnEquipFinished()
{
	AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;

	DetermineWeaponState();
}

bool ASWeapon::IsEquipped() const
{
	return bIsEquipped;
}

float ASWeapon::GetEquipStartedTime() const
{
	return EquipStartedTime;
}

float ASWeapon::GetEquipDuration() const
{
	return EquipDuration;
}

void ASWeapon::OnEnterInventory(ASCharacter * inNewOwner)
{
	SetOwningPawn(inNewOwner);
	AttachMeshToPawn(StorageSlot);
}

void ASWeapon::OnLeaveInventory()
{
	if (Role == ROLE_Authority)
	{
		SetOwningPawn(nullptr);
	}

	if (IsAttachedToPawn())
	{
		OnUnEquip();
	}

	DetachMeshFromPawn();
}

void ASWeapon::SetWeaponState(EWeaponState inNewState)
{
	const EWeaponState prevState = CurrentState;

	if ((prevState == EWeaponState::Firing) && (inNewState != EWeaponState::Firing))
	{
		OnBurstFinished();
	}

	CurrentState = inNewState;

	if ((prevState != EWeaponState::Firing) && (inNewState == EWeaponState::Firing))
	{
		OnBurstStarted();
	}
}

void ASWeapon::DetermineWeaponState()
{
	EWeaponState newState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		if (bWantsToFire && CanFire())
		{
			newState = EWeaponState::Firing;
		}
	}
	else if (bPendingEquip)
	{
		newState = EWeaponState::Equipping;
	}

	SetWeaponState(newState);
}

void ASWeapon::HandleFiring()
{
	if (CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			FireWeapon();

			// TODO: Consume Ammo

			// Update firing FX on remote clients if this is called on server
			// Cause it is replicated value on server.
			BurstCounter++;
		}
	}

	// TODO: End repeat fire if ammo runs dry

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		if (Role < ROLE_Authority)
		{
			ServerHanleFiring();
		}

		// TODO: if weapon is automatic firing rifle -> Setup refiring timer. (calls HandleFiring())

		bRefiring = (CurrentState == EWeaponState::Firing && TimeBetweenShots > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(HandleFiringTimerHandle, this, &ASWeapon::HandleFiring, TimeBetweenShots, false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

bool ASWeapon::ServerStartFire_Validate()
{
	return true;
}

void ASWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

bool ASWeapon::ServerStopFire_Validate()
{
	return true;
}

void ASWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

bool ASWeapon::ServerHanleFiring_Validate()
{
	return true;
}

void ASWeapon::ServerHanleFiring_Implementation()
{
	const bool bShouldUpdateAmmo = CanFire();

	HandleFiring();

	if (bShouldUpdateAmmo)
	{
		// Update firing FX on remote clients
		BurstCounter++;
	}
}

void ASWeapon::OnBurstStarted()
{
	const float GameTime = GetWorld()->GetTimeSeconds();
	if ((LastFireTime > 0.0f) && (TimeBetweenShots > 0.0f) &&
		(LastFireTime + TimeBetweenShots > GameTime))
	{
		GetWorldTimerManager().SetTimer(HandleFiringTimerHandle, this, &ASWeapon::HandleFiring, LastFireTime + TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
}

void ASWeapon::OnBurstFinished()
{
	BurstCounter = 0;

	if (GetNetMode() != NM_DedicatedServer)
	{
		StopSimulateWeaponFire();
	}

	GetWorldTimerManager().ClearTimer(HandleFiringTimerHandle);
	bRefiring = false;
}

void ASWeapon::StartFire()
{
	if (Role == ROLE_Authority)
	{
		ServerStartFire();
	}

	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}

void ASWeapon::StopFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStopFire();
	}

	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

EWeaponState ASWeapon::GetCurrentState() const
{
	return CurrentState;
}

bool ASWeapon::CanFire() const
{
	// TODO: Ask to pawn can fire.
	const bool bPawnCanFire = MyPawn != nullptr; // && MyPawn->CanFire();
	const bool bStateCheck = (CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing);
	return bPawnCanFire && bStateCheck;
}

FVector ASWeapon::GetAdjustedAim() const
{
	APlayerController* const PC = Instigator ? Cast<APlayerController>(Instigator->GetController()) : nullptr;
	FVector finalAim = FVector::ZeroVector;

	if (PC)
	{
		FVector camLoc;
		FRotator camRot;
		PC->GetPlayerViewPoint(camLoc, camRot);
		finalAim = camRot.Vector();
	}
	else if (Instigator)
	{
		finalAim = Instigator->GetBaseAimRotation().Vector();
	}
	
	return finalAim;
}

FVector ASWeapon::GetCameraDamageStartLocation(const FVector & inAimDir) const
{
	APlayerController* const PC = MyPawn ? Cast<APlayerController>(MyPawn->GetController()) : nullptr;
	FVector outStartTrace = FVector::ZeroVector;

	if (PC)
	{
		FRotator dummyRot;
		PC->GetPlayerViewPoint(outStartTrace, dummyRot);

		// 3인칭의 경우 카메라에서 발사된 광선이 폰에 걸릴수도 있으므로 폰과 카메라와의 거리 및 방향이
		// 고려된 조정된 지점을 계산한다. 이 내용은 ShooterGame에 쉽고 명확하게 코딩되어 있는듯 하니 후에 참조해볼것.
		// TODO: copied from ShooterGame
		outStartTrace = outStartTrace + inAimDir * ((Instigator->GetActorLocation() - outStartTrace) | inAimDir);
	}

	return outStartTrace;
}

FHitResult ASWeapon::WeaponTrace(const FVector & inTraceFrom, const FVector & inTraceTo) const
{
	FCollisionQueryParams traceParams(TEXT("WeaponTrace"), true, Instigator);
	traceParams.bTraceAsyncScene = true;
	traceParams.bReturnPhysicalMaterial = true;

	FHitResult hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(hit, inTraceFrom, inTraceTo, COLLISION_WEAPON, traceParams);
	return hit;
}

void ASWeapon::SimulateWeaponFire()
{
	if (MuzzleFX)
	{
		MuzzlePCS = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh, MuzzleAttachPoint);
	}

	if (!bPlayingFireAnim)
	{
		PlayWeaponAnimation(FireAnim);
		bPlayingFireAnim = true;
	}

	PlayWeaponSound(FireSound);
}

void ASWeapon::StopSimulateWeaponFire()
{
	if (bPlayingFireAnim)
	{
		StopWeaponAnimation(FireAnim);
		bPlayingFireAnim = false;
	}
}

FVector ASWeapon::GetMuzzleLocation() const
{
	return Mesh->GetSocketLocation(MuzzleAttachPoint);
}

FVector ASWeapon::GetMuzzleDirection() const
{
	return Mesh->GetSocketRotation(MuzzleAttachPoint).Vector();
}

UAudioComponent * ASWeapon::PlayWeaponSound(USoundCue * inSountToPlay)
{
	UAudioComponent* AC = nullptr;
	if (inSountToPlay && MyPawn)
	{
		AC = UGameplayStatics::SpawnSoundAttached(inSountToPlay, MyPawn->GetRootComponent());
	}
	return AC;
}

float ASWeapon::PlayWeaponAnimation(UAnimMontage * inAnimation, float inPlayRate, FName StartSectionName)
{
	float duration = 0.0f;
	if (MyPawn && inAnimation)
	{
		duration = MyPawn->PlayAnimMontage(inAnimation, inPlayRate, StartSectionName);
	}
	return duration;
}

void ASWeapon::StopWeaponAnimation(UAnimMontage * inAnimation)
{
	if (MyPawn && inAnimation)
	{
		MyPawn->StopAnimMontage(inAnimation);
	}
}

void ASWeapon::OnRep_BurstCounter()
{
	if (BurstCounter > 0)
	{
		SimulateWeaponFire();
	}
	else
	{
		StopSimulateWeaponFire();
	}
}
