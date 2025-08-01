#include "Weapons/BulletPoolManager.h"
#include "Weapons/BulletBase.h"

ABulletPoolManager::ABulletPoolManager()
{
}

void ABulletPoolManager::BeginPlay()
{
	Super::BeginPlay();
	CreateBulletPool();
}

ABulletBase* ABulletPoolManager::GetBullet()
{
	for (ABulletBase* Bullet : BulletPool)
	{
		if (!Bullet->IsActive())
		{
			return Bullet;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Bullet Pool is Empty"));
	
	return nullptr;
}

void ABulletPoolManager::CreateBulletPool()
{
	if (!BulletClass) return;

	if (UWorld* World = GetWorld())
	{
		for (int32 i = 0; i < BulletPoolSize; i++)
		{
			if (ABulletBase* Bullet = World->SpawnActor<ABulletBase>(BulletClass, FVector::ZeroVector, FRotator::ZeroRotator))
			{
				Bullet->DeactivateBullet();
				BulletPool.Add(Bullet);
			}
		}
	}
}

