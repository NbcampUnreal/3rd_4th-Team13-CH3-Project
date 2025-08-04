#include "GameFramework/MatrixProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

AMatrixProjectile::AMatrixProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AMatrixProjectile::OnHit);
	RootComponent = CollisionComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	InitialLifeSpan = 3.0f;
}

void AMatrixProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AMatrixProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(OtherActor);
		if (TargetASI)
		{
			UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent();
			if (TargetASC)
			{
				if (HasAuthority() && DamageEffectClass)
				{
					FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
					ContextHandle.AddInstigator(GetInstigator(), this);

					TargetASC->ApplyGameplayEffectToSelf(DamageEffectClass->GetDefaultObject<UGameplayEffect>(), 1.0f, ContextHandle);
				}
			}
		}
	}

	Destroy();
}
