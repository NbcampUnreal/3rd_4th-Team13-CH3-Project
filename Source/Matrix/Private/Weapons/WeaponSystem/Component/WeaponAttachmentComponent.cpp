#include "Weapons/WeaponSystem/Component/WeaponAttachmentComponent.h"

UWeaponAttachmentComponent::UWeaponAttachmentComponent()
	: AttachSocket(TEXT("NONE"))
	, MeshInitialRotation(FRotator::ZeroRotator)
	, MeshInitialScale(FVector::OneVector)
{
}

void UWeaponAttachmentComponent::AttachToOwner(USceneComponent* CharacterMesh, AActor* WeaponActor,
	UStaticMeshComponent* WeaponMeshComp)
{
	WeaponMeshComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
	WeaponMeshComp->SetSimulatePhysics(false);
	WeaponMeshComp->SetEnableGravity(false);
	WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
	FAttachmentTransformRules AttachRules(
	EAttachmentRule::SnapToTarget,
	EAttachmentRule::SnapToTarget,
	EAttachmentRule::SnapToTarget,
	true
	);

	WeaponActor->AttachToComponent(CharacterMesh, AttachRules, AttachSocket);
	WeaponMeshComp->AttachToComponent(CharacterMesh, AttachRules, AttachSocket);

	WeaponMeshComp->SetRelativeRotation(MeshInitialRotation);
	WeaponMeshComp->SetRelativeScale3D(MeshInitialScale);
}

void UWeaponAttachmentComponent::DetachFromOwner(AActor* WeaponActor, UStaticMeshComponent* WeaponMeshComp)
{
	WeaponActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	WeaponMeshComp->SetSimulatePhysics(true);
	WeaponMeshComp->SetEnableGravity(true);
	WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
