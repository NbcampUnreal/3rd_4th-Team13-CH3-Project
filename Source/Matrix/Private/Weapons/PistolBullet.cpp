#include "Weapons/PistolBullet.h"

#include "GameFramework/ProjectileMovementComponent.h"

APistolBullet::APistolBullet()
{
	BulletSpeed = 2000.0f;
	ProjectileMovementComp->InitialSpeed = BulletSpeed;
	ProjectileMovementComp->MaxSpeed = BulletSpeed;
}

