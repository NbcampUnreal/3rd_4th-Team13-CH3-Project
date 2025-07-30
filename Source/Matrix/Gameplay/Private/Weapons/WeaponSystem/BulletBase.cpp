#include "Weapons/WeaponSystem/BulletBase.h"

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

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileMovementComp)
	{
		FVector FireDirection = GetActorForwardVector();
		ProjectileMovementComp->Velocity = FireDirection * ProjectileMovementComp->InitialSpeed;
	}
	
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &ABulletBase::OnDestroy, 3.0f, false);
}

void ABulletBase::OnDestroy()
{
	Destroy();
}


