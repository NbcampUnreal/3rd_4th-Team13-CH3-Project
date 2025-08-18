#include "Weapons/Pistol.h"

APistol::APistol()
{
	TriggerTime = 0.3;
	MaxBulletCount = 10;
	CurrentBulletCount = MaxBulletCount;
	WeaponType = EWeaponType::Pistol;
}
