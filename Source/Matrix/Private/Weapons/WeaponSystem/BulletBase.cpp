#include "Weapons/WeaponSystem/BulletBase.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABulletBase::ABulletBase()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	SetRootComponent(CollisionComp);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionObjectType(ECC_GameTraceChannel1);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CollisionComp);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bShouldBounce = true;
	ProjectileMovementComp->Bounciness = 0.5f;
	ProjectileMovementComp->ProjectileGravityScale = 0.0f;

	TrailEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffectComp"));
	TrailEffectComp->SetupAttachment(CollisionComp);
	TrailEffectComp->bAutoActivate = false;
	
	BulletSpeed = 2000.0f;
	ProjectileMovementComp->InitialSpeed = BulletSpeed;
	ProjectileMovementComp->MaxSpeed = BulletSpeed;
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

	if (TrailEffectComp && TrailEffectComp->GetAsset())
	{
		TrailEffectComp->Activate();
	}
	
	GetWorldTimerManager().SetTimer(DeactivateTimerHandle, this, &ABulletBase::DeactivateBullet, 3.0f, false);
}

void ABulletBase::DeactivateBullet()
{
	if (TrailEffectComp && TrailEffectComp->GetAsset())
	{
		TrailEffectComp->Deactivate();
	}

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

