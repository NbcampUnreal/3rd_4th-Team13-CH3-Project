#include "Weapons/WeaponSystem/WeaponBase.h"
#include "Weapons/WeaponSystem/BulletBase.h"
#include "Weapons/WeaponSystem/BulletPoolManager.h"

#include "Kismet/GameplayStatics.h"

AWeaponBase::AWeaponBase()
	: IsShootAvailable(true)
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);
	
	TriggerTime = 1.0f;
	MaxBulletCount = 10;
	CurrentBulletCount = MaxBulletCount;
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	SetBulletPool();
}

void AWeaponBase::Shoot()
{
	if (!IsShootAvailable || !BulletPoolManager) return;
	if (CurrentBulletCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("There's No Bullet In Weapon"));

		IsShootAvailable = false;
		GetWorldTimerManager().SetTimer(ShootTriggerTimerHandle, this, &AWeaponBase::SetShootAvailable, TriggerTime, false);
		
		return;
	}
	
	if (ABulletBase* Bullet = BulletPoolManager->GetBullet(BulletClass))
	{
		Bullet->ActivateBullet(GetActorLocation(), GetActorRotation());
		CurrentBulletCount--;
		UE_LOG(LogTemp, Warning, TEXT("Weapon's Bullet Count : %d / %d"), CurrentBulletCount, MaxBulletCount);
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
