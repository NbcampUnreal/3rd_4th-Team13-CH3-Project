#include "Weapons/WeaponSystem/WeaponBase.h"

#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Weapons/WeaponSystem/Component/WeaponAttachmentComponent.h"
#include "Weapons/WeaponSystem/Component/WeaponDamageComponent.h"
#include "Weapons/WeaponSystem/Component/WeaponEffectComponent.h"
#include "Weapons/WeaponSystem/BulletPoolManager.h"
#include "Weapons/WeaponSystem/BulletBase.h"
#include "Characters/MainPlayerController.h"

AWeaponBase::AWeaponBase()
	: WeaponType(EWeaponType::None)
	, TriggerTime(3.0f)
	, MaxBulletCount(30)
	, CurrentBulletCount(MaxBulletCount)
	, bIsFiring(false)
	, BulletClass(nullptr)
	, BulletPoolManager(nullptr)
	, TargetLocation(FVector::ZeroVector)
{
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetupAttachment(RootComp);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetSphereRadius(90.0f);
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComp);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetEnableGravity(true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);

	MuzzlePoint = CreateDefaultSubobject<UArrowComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(RootComp);
	MuzzlePoint->ArrowColor = FColor::Red;
	MuzzlePoint->bHiddenInGame = true;
	MuzzlePoint->bIsScreenSizeScaled = true;

	AttachmentComp = CreateDefaultSubobject<UWeaponAttachmentComponent>(TEXT("AttachmentComp"));
	DamageComp = CreateDefaultSubobject<UWeaponDamageComponent>(TEXT("DamageComp"));
	EffectComp = CreateDefaultSubobject<UWeaponEffectComponent>(TEXT("EffectComp"));
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	SetBulletPool();
}

void AWeaponBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}

void AWeaponBase::Shoot()
{
	if (bIsFiring || !BulletPoolManager) return;
	
	if (CurrentBulletCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("There's No Bullet In Weapon"));
		return;
	}
	
	bIsFiring = true;
	UE_LOG(LogTemp, Error, TEXT("[Weapon] Shoot1."))
	
	if (!FireBullet())
	{
		//bIsFiring = false;
		UE_LOG(LogTemp, Error, TEXT("[Weapon] Shoot3."))
		return ;
	}

	CurrentBulletCount--;
	UE_LOG(LogTemp, Warning, TEXT("Weapon's Bullet Count : %d / %d"), CurrentBulletCount, MaxBulletCount);
	

	if (EffectComp)
	{
		EffectComp->PlayEffect(MuzzlePoint->GetComponentLocation(), MuzzlePoint->GetComponentRotation(), FVector(0.3f));
	}
	
	if (OwnerPC)
	{
		if (AMainPlayerController* PC = Cast<AMainPlayerController>(OwnerPC))
		{
			PC->NotifyAmmoChanged(CurrentBulletCount, MaxBulletCount);
		}
	}
}

void AWeaponBase::SetTargetLocation(const FVector& NewTargetLocation)
{
	TargetLocation = NewTargetLocation;
}

bool AWeaponBase::FireBullet()
{
	const FRotator FireRotation = GetFireDirection().Rotation();

	if (ABulletBase* Bullet = BulletPoolManager->GetBullet(BulletClass))
	{
		Bullet->ActivateBullet(MuzzlePoint->GetComponentLocation(), FireRotation, OwnerPawn, this);

		return true;
	}

	return false;
}

FVector AWeaponBase::GetFireDirection() const
{
	if (!TargetLocation.IsNearlyZero())
	{
		return (TargetLocation - MuzzlePoint->GetComponentLocation()).GetSafeNormal();
	}
	
	if (APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		int32 ViewportX, ViewportY;
		PC->GetViewportSize(ViewportX, ViewportY);

		FVector2D ScreenCrosshair(ViewportX * 0.5f, ViewportY * 0.5f);

		FVector WorldOrigin, WorldDirection;
		if (PC->DeprojectScreenPositionToWorld(ScreenCrosshair.X, ScreenCrosshair.Y, WorldOrigin, WorldDirection))
		{
			FVector TraceEnd = WorldOrigin + WorldDirection * 3000.0f;

			return (TraceEnd - MuzzlePoint->GetComponentLocation()).GetSafeNormal();
		}
	}

	return MuzzlePoint->GetForwardVector();
}

void AWeaponBase::SetShootAvailable()
{
	bIsFiring = false;
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

void AWeaponBase::SetWeaponOwner(AActor* NewOwner)
{
	if (!NewOwner) return;
	
	OwnerPawn = Cast<APawn>(NewOwner);
	if (OwnerPawn)
	{
		OwnerPC = Cast<APlayerController>(OwnerPawn->GetController());
	}
}

void AWeaponBase::ResetWeaponOwner()
{
	if (OwnerPawn)
	{
		OwnerPawn = nullptr;
	}

	if (OwnerPC)
	{
		OwnerPC = nullptr;
	}
}

void AWeaponBase::SetBulletCount(float Amount)
{
	CurrentBulletCount = FMath::Clamp(CurrentBulletCount + (MaxBulletCount * Amount), 0, MaxBulletCount);

	if (OwnerPC)
	{
		if (AMainPlayerController* PC = Cast<AMainPlayerController>(OwnerPC))
		{
			PC->NotifyAmmoChanged(CurrentBulletCount, MaxBulletCount);
		}
	}
}

void AWeaponBase::AttachToOwner(USceneComponent* CharacterMesh)
{
	if (!AttachmentComp)
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon Attachment Component not found"));
		return;
	}
	
	AttachmentComp->AttachToOwner(CharacterMesh, this, MeshComp);
}

void AWeaponBase::DetachFromOwner()
{
	if (!AttachmentComp)
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon Attachment Component not found"));
		return;
	}
	
	AttachmentComp->DetachFromOwner(this, MeshComp);
	SetTargetLocation(FVector::ZeroVector);
}

void AWeaponBase::ApplyBulletDamage(AActor* TargetActor, const FHitResult& HitResult)
{
	if (!DamageComp)
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon Damage Component not found"));
		return;
	}
	
	DamageComp->ApplyDamage(TargetActor, OwnerPawn, HitResult);
}