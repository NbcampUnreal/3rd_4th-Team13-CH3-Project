#include "Weapons/WeaponSystem/WeaponBase.h"
#include "Weapons/WeaponSystem/BulletBase.h"
#include "Weapons/WeaponSystem/BulletPoolManager.h"

#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"

AWeaponBase::AWeaponBase()
	: TriggerTime(1.0f)
	, MaxBulletCount(10)
	, CurrentBulletCount(MaxBulletCount)
	, IsShootAvailable(true)
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	SetRootComponent(CollisionComp);
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CollisionComp);

	MuzzlePoint = CreateDefaultSubobject<UArrowComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(CollisionComp);
	MuzzlePoint->ArrowColor = FColor::Red;
	MuzzlePoint->bHiddenInGame = true;
	MuzzlePoint->bIsScreenSizeScaled = true;
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	SetBulletPool();
}

void AWeaponBase::Shoot()
{
	if (!IsShootAvailable || !BulletPoolManager) return;
	
	IsShootAvailable = false;
	GetWorldTimerManager().SetTimer(ShootTriggerTimerHandle, this, &AWeaponBase::SetShootAvailable, TriggerTime, false);
	
	if (CurrentBulletCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("There's No Bullet In Weapon"));
		return;
	}

	FireBullet();
	
	CurrentBulletCount--;
	UE_LOG(LogTemp, Warning, TEXT("Weapon's Bullet Count : %d / %d"), CurrentBulletCount, MaxBulletCount);
}

void AWeaponBase::FireBullet()
{
	if (ABulletBase* Bullet = BulletPoolManager->GetBullet(BulletClass))
	{
		Bullet->ActivateBullet(MuzzlePoint->GetComponentLocation(), MuzzlePoint->GetComponentRotation());
	}
}

void AWeaponBase::SetShootAvailable()
{
	IsShootAvailable = true;
}

void AWeaponBase::SetBulletPool()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABulletPoolManager::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		BulletPoolManager = Cast<ABulletPoolManager>(FoundActors[0]);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("There is no BulletPoolManager in the world"));
	}
}
