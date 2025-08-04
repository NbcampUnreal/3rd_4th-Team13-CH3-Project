#include "Weapons/Rifle.h"

ARifle::ARifle()
{
	TriggerTime = 0.2f;
	MaxBulletCount = 30;
	CurrentBulletCount = MaxBulletCount;
	AttachSocket = TEXT("rifle_r");
	WeaponType = EWeaponType::Rifle;
}
