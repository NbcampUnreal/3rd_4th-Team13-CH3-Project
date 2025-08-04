#include "Weapons/ShotgunBullet.h"

#include "GameFramework/ProjectileMovementComponent.h"

AShotgunBullet::AShotgunBullet()
{
	BulletSpeed = 3000.0f;
	ProjectileMovementComp->InitialSpeed = BulletSpeed;
	ProjectileMovementComp->MaxSpeed = BulletSpeed;
}

