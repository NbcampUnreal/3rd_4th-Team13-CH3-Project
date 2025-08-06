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
	, WeaponType(EWeaponType::None)
	, AttachSocket(TEXT("NONE"))
	, bIsFiring(false)
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
	if (bIsFiring || !BulletPoolManager) return;

	//GetWorldTimerManager().ClearTimer(ShootTriggerTimerHandle);
	//GetWorldTimerManager().SetTimer(ShootTriggerTimerHandle, this, &AWeaponBase::SetShootAvailable, TriggerTime, false);
	
	if (CurrentBulletCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("There's No Bullet In Weapon"));
		return;
	}

	bIsFiring = true;
	//UE_LOG(LogTemp, Warning, TEXT("IsFiring : true"));

	FireBullet();
	
	CurrentBulletCount--;
	UE_LOG(LogTemp, Warning, TEXT("Weapon's Bullet Count : %d / %d"), CurrentBulletCount, MaxBulletCount);
}

void AWeaponBase::FireBullet()
{
	if (ABulletBase* Bullet = BulletPoolManager->GetBullet(BulletClass))
	{
		FVector FireDirection = GetFireDirection();
		FRotator FireRotation = FireDirection.Rotation();

		/*GEngine->AddOnScreenDebugMessage(
			-1,                     
			5.0f,                   
			FColor::Green,          
			FireDirection.ToString()
		);

		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Green,
			FireRotation.ToString()
		);*/

		Bullet->ActivateBullet(MuzzlePoint->GetComponentLocation(), FireRotation);
	//	Bullet->ActivateBullet(MuzzlePoint->GetComponentLocation(), MuzzlePoint->GetComponentRotation());
	}
}

void AWeaponBase::SetShootAvailable()
{
	bIsFiring = false;
	//UE_LOG(LogTemp, Warning, TEXT("IsFiring : false"));
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

FVector AWeaponBase::GetFireDirection() const
{
	if (APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		int32 ViewportX, ViewportY;
		PC->GetViewportSize(ViewportX, ViewportY);

		FVector2D ScreenCrosshair(ViewportX * 0.5f, ViewportY * 0.5f);

		FVector WorldOrigin, WorldDirection;
		if (PC->DeprojectScreenPositionToWorld(ScreenCrosshair.X, ScreenCrosshair.Y, WorldOrigin, WorldDirection))
		{
			// 라인트레이스 끝 3000.0f(30m) 거리로 계산
			FVector TraceEnd = WorldOrigin + WorldDirection * 3000.0f;

			// 총구 기준 방향값 계산후 반환 (1이하 값으로 정규화) 
			return (TraceEnd - MuzzlePoint->GetComponentLocation()).GetSafeNormal();
		}
	}

	// 계산 실패 시 총구 정면 방향값 반환
	return MuzzlePoint->GetForwardVector();
}