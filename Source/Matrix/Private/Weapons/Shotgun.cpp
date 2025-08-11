#include "Weapons/Shotgun.h"
#include "Weapons/WeaponSystem/BulletBase.h"
#include "Weapons/WeaponSystem/BulletPoolManager.h"

#include "Components/ArrowComponent.h"

AShotgun::AShotgun()
{
	TriggerTime = 2.0f;
	MaxBulletCount = 6;
	CurrentBulletCount = MaxBulletCount;

	NumBulletsPerShot = 5;
	SpreadAngle = 5.0f;

	AttachSocket = TEXT("shotgun_r");
	WeaponType = EWeaponType::Shotgun;
}

bool AShotgun::FireBullet()
{
	const FVector FireLocation = MuzzlePoint->GetComponentLocation();
	const FRotator BaseRotation = GetFireDirection().Rotation();

	for (int32 i = 0; i < NumBulletsPerShot; i++)
	{
		FRotator SpreadRotation = BaseRotation;
		SpreadRotation.Yaw += FMath::FRandRange(-SpreadAngle, SpreadAngle);
		SpreadRotation.Pitch += FMath::FRandRange(-SpreadAngle, SpreadAngle);

		if (ABulletBase* Bullet = BulletPoolManager->GetBullet(BulletClass))
		{
			Bullet->ActivateBullet(FireLocation, SpreadRotation, OwnerPawn, this);
		}
		else
		{
			return false;
		}
	}

	return true;
}
