#include <Weapons/WeaponBase.h>

#include <Weapons/BulletBase.h>

AWeaponBase::AWeaponBase()
	: IsShootAvailable(true)
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);
	
	TriggerTime = 1.0f;
}

void AWeaponBase::Shoot()
{
	if (!IsShootAvailable) return;
	
	if (Bullet && Bullet->IsChildOf(ABulletBase::StaticClass()))
	{
		GetWorld()->SpawnActor<ABulletBase>(Bullet, GetActorLocation(), GetActorRotation());
	}

	IsShootAvailable = false;
	GetWorldTimerManager().SetTimer(ShootTriggerTimerHandle, this, &AWeaponBase::SetShootAvailable, TriggerTime, false);
}

void AWeaponBase::SetShootAvailable()
{
	IsShootAvailable = true;
}
