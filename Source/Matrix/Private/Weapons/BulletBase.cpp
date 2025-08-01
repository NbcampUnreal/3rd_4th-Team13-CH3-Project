#include "Weapons/BulletBase.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABulletBase::ABulletBase()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	SetRootComponent(CollisionComp);
	CollisionComp->SetCollisionProfileName(TEXT("BlockAll"));
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CollisionComp);

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

	CollisionComp->OnComponentHit.AddDynamic(this, &ABulletBase::OnBulletHit);
}

void ABulletBase::OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor->IsA(APawn::StaticClass())
		&& OtherActor->ActorHasTag(TEXT("Enemy")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Bullet Hit Pawn : %s"), *OtherActor->GetName());
	}
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
		ProjectileMovementComp->SetUpdatedComponent(CollisionComp);
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

