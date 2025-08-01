#include "Weapons/WeaponBase.h"
#include "Weapons/BulletBase.h"
#include "Weapons/BulletPoolManager.h"

#include "Kismet/GameplayStatics.h"

AWeaponBase::AWeaponBase()
	: IsShootAvailable(true)
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);
	
	TriggerTime = 1.0f;
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	SetBulletPool();
}

void AWeaponBase::Shoot()
{
	if (!IsShootAvailable || !BulletPoolManager) return;
	
	if (ABulletBase* Bullet = BulletPoolManager->GetBullet())
	{
		Bullet->ActivateBullet(GetActorLocation(), GetActorRotation());
	}

	IsShootAvailable = false;
	GetWorldTimerManager().SetTimer(ShootTriggerTimerHandle, this, &AWeaponBase::SetShootAvailable, TriggerTime, false);
}

void AWeaponBase::SetShootAvailable()
{
	IsShootAvailable = true;
}

void AWeaponBase::SetBulletPool()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABulletPoolManager::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		BulletPoolManager = Cast<ABulletPoolManager>(FoundActors[0]);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("There is no BulletPoolManager in the world"));
	}
}
