#include "Weapons/WeaponSystem/BulletPoolManager.h"
#include "Weapons/WeaponSystem/BulletBase.h"

ABulletPoolManager::ABulletPoolManager()
{
}

void ABulletPoolManager::BeginPlay()
{
	Super::BeginPlay();
	
	CreateBulletPool();
}

ABulletBase* ABulletPoolManager::GetBullet(TSubclassOf<ABulletBase> BulletClass)
{
	if (!BulletClass) return nullptr;

	TArray<ABulletBase*>* PoolPtr = BulletPool.Find(BulletClass);
	if (!PoolPtr) return nullptr;
	
	for (ABulletBase* Bullet : *PoolPtr)
	{
		if (!Bullet->IsActive())
		{
			return Bullet;
		}
	}

	
	return nullptr;
}

void ABulletPoolManager::CreateBulletPool()
{
	if (BulletClasses.Num() == 0) return;

	for (const TSubclassOf<ABulletBase>& BulletClass : BulletClasses)
	{
		if (BulletClass)
		{
			TArray<ABulletBase*>& Pool = BulletPool.FindOrAdd(BulletClass);
			
			for (int32 i = 0; i < BulletPoolSize; i++)
			{
				if (ABulletBase* Bullet = GetWorld()->SpawnActor<ABulletBase>(BulletClass, FVector::ZeroVector, FRotator::ZeroRotator))
				{
					Bullet->DeactivateBullet();
					Pool.Add(Bullet);
				}
			}

		
		}
	}
}

