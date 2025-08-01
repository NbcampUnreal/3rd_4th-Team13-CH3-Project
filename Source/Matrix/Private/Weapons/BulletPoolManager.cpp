#include "Weapons/BulletPoolManager.h"
#include "Weapons/BulletBase.h"

UBulletPoolManager::UBulletPoolManager()
{
}

void UBulletPoolManager::Init()
{
	CreateBulletPool();
}

ABulletBase* UBulletPoolManager::GetBullet()
{
	for (ABulletBase* Bullet : BulletPool)
	{
		if (!Bullet->IsActive())
		{
			return Bullet;
		}
	}
	return nullptr;
}

void UBulletPoolManager::CreateBulletPool()
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

