#include "Weapons/Pistol.h"

APistol::APistol()
{
	TriggerTime = 1.0f;
	MaxBulletCount = 10;
	CurrentBulletCount = MaxBulletCount;
	WeaponType = EWeaponType::Pistol;
}
