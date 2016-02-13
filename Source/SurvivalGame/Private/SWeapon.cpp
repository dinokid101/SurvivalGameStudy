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

ACharacter * ASWeapon::GetPawnOwner() const
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
