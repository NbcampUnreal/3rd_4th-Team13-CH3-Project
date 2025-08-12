#include "Weapons/WeaponSystem/BulletBase.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Weapons/WeaponSystem/WeaponBase.h"

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
	BulletLifeSpan = 2.0f;
}

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->OnComponentHit.AddDynamic(this, &ABulletBase::OnBulletHit);
}

void ABulletBase::OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("ABulletBase OnBulletHit called. Hit Actor: %s"), OtherActor ? *OtherActor->GetName() : TEXT("None")); // Added log

	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherActor != OwnerPawn))
	{
		// Apply GameplayEffect
		if (HasAuthority())
		{
			OwnerWeapon->ApplyBulletDamage(OtherActor, Hit);
		}
	}

	Destroy(); // Destroy bullet on hit
}

void ABulletBase::ActivateBullet(FVector Location, FRotator Rotation, APawn* NewOwner, AWeaponBase* NewWeapon)
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
	OwnerPawn = NewOwner;
	OwnerWeapon = NewWeapon;

	if (TrailEffectComp && TrailEffectComp->GetAsset())
	{
		TrailEffectComp->Activate();
	}
	
	GetWorldTimerManager().SetTimer(DeactivateTimerHandle, this, &ABulletBase::DeactivateBullet, BulletLifeSpan, false);
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
	OwnerPawn = nullptr;
	OwnerWeapon = nullptr;
	
	GetWorldTimerManager().ClearTimer(DeactivateTimerHandle);
}


