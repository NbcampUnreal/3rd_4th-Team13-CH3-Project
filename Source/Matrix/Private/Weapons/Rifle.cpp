#include "Weapons/Rifle.h"

ARifle::ARifle()
{
	TriggerTime = 0.2f;
	MaxBulletCount = 30;
	CurrentBulletCount = MaxBulletCount;
	WeaponType = EWeaponType::Rifle;

	if (FireAnim)
	{
		TriggerTime = FireAnim->GetPlayLength();
	}
}
