#include "Weapons/WeaponSystem/BulletBase.h"

#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

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
	BulletLifeSpan = 3.0f;
}

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABulletBase::OnBulletOverlap);
	CollisionComp->OnComponentHit.AddDynamic(this, &ABulletBase::OnBulletHit);
}

void ABulletBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}

void ABulletBase::OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor == nullptr) || (OtherActor == this) || (OtherActor == OwnerPawn))
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("ABulletBase OnBulletHit called. Hit Actor: %s"), OtherActor ? *OtherActor->GetName() : TEXT("None")); // Added log

	DeactivateBullet();
}

void ABulletBase::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor == nullptr) || (OtherActor == this) || (OtherActor == OwnerPawn))
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ABulletBase OnBulletOverlap called. Hit Actor: %s"), OtherActor ? *OtherActor->GetName() : TEXT("None")); // Added log
	
	ECollisionChannel HitChannel = OtherComp ? OtherComp->GetCollisionObjectType() : OtherActor->GetRootComponent()->GetCollisionObjectType();

	if (HitChannel == ECC_Pawn)
	{
		if (HasAuthority())
		{
			OwnerWeapon->ApplyBulletDamage(OtherActor, SweepResult);
		}
	}
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
		ProjectileMovementComp->Velocity = FireDirection * BulletSpeed;
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

	if (DeactivateTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(DeactivateTimerHandle);
		DeactivateTimerHandle.Invalidate();
	}
}


