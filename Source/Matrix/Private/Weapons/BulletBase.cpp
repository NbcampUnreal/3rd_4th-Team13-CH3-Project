#include "Weapons/BulletBase.h"

#include "GameFramework/ProjectileMovementComponent.h"

ABulletBase::ABulletBase()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 3000.0f;
	ProjectileMovementComp->MaxSpeed = 3000.0f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bShouldBounce = true;
	ProjectileMovementComp->Bounciness = 0.5f;
	ProjectileMovementComp->ProjectileGravityScale = 0.0f;
}

void ABulletBase::ActivateBullet(FVector Location, FRotator Rotation)
{
	SetActorLocation(Location);
	SetActorRotation(Rotation);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->SetUpdatedComponent(MeshComp);
		FVector FireDirection = GetActorForwardVector();
		ProjectileMovementComp->Velocity = FireDirection * ProjectileMovementComp->InitialSpeed;
	}

	bIsActive = true;
	
	GetWorldTimerManager().SetTimer(DeactivateTimerHandle, this, &ABulletBase::DeactivateBullet, 3.0f, false);
}

void ABulletBase::DeactivateBullet()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->StopMovementImmediately();
	}

	bIsActive = false;
	
	GetWorldTimerManager().ClearTimer(DeactivateTimerHandle);
}

