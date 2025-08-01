#include "Weapons/WeaponBase.h"
#include "Weapons/BulletBase.h"
#include "Weapons/BulletPoolManager.h"

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
	if (!IsShootAvailable || !BulletPool) return;
	
	if (ABulletBase* Bullet = BulletPool->GetBullet())
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
	BulletPool = NewObject<UBulletPoolManager>(this);
	BulletPool->BulletClass = BulletClass;
	BulletPool->BulletPoolSize = 30;
	BulletPool->Init();
}
