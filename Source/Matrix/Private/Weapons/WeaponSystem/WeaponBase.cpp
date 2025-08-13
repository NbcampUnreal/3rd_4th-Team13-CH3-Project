#include "Weapons/WeaponSystem/WeaponBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/MainPlayerController.h"
#include "Weapons/WeaponSystem/BulletBase.h"
#include "Weapons/WeaponSystem/BulletPoolManager.h"

#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/MatrixAttributeSet.h"
#include "Weapons/WeaponSystem/Component/WeaponAttachmentComponent.h"

AWeaponBase::AWeaponBase()
	: TriggerTime(1.0f)
	, MaxBulletCount(10)
	, CurrentBulletCount(MaxBulletCount)
	, WeaponType(EWeaponType::None)
	, AttachSocket(TEXT("NONE"))
	, bIsFiring(false)
	, WeaponDamage(100.0f)
	, MeshInitialRotation(FRotator::ZeroRotator)
	, MeshInitialScale(FVector::OneVector)
{
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetupAttachment(RootComp);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	
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
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	SetBulletPool();
}

void AWeaponBase::Shoot()
{
	if (bIsFiring || !BulletPoolManager) return;

	// 발사 가능 상태를 TriggerTime 후에 복구하는 타이머 설정
	GetWorldTimerManager().SetTimer(ShootTriggerTimer, this, &AWeaponBase::SetShootAvailable, TriggerTime, false);
	
	if (CurrentBulletCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("There's No Bullet In Weapon"));
		return;
	}

	bIsFiring = true;

	if (!FireBullet())
	{
		// 발사에 실패하면 즉시 발사 가능 상태로 복귀
		if (ShootTriggerTimer.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(ShootTriggerTimer);
			ShootTriggerTimer.Invalidate();
		}
		bIsFiring = false;
		return;
	}
		
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	if (FireEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, FireEffect, MuzzlePoint->GetComponentLocation(), MuzzlePoint->GetComponentRotation());
	}
	
	CurrentBulletCount--;
	UE_LOG(LogTemp, Warning, TEXT("Weapon's Bullet Count : %d / %d"), CurrentBulletCount, MaxBulletCount);

	if (OwnerPC)
	{
		if (AMainPlayerController* PC = Cast<AMainPlayerController>(OwnerPC))
		{
			PC->NotifyAmmoChanged(CurrentBulletCount, MaxBulletCount);
		}
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

void AWeaponBase::AttachToOwner(USceneComponent* ParentComp)
{
	MeshComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetEnableGravity(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
	FAttachmentTransformRules AttachRules(
	EAttachmentRule::SnapToTarget,
	EAttachmentRule::SnapToTarget,
	EAttachmentRule::SnapToTarget,
	true
	);

	AttachToComponent(ParentComp, AttachRules, AttachSocket);
	MeshComp->AttachToComponent(ParentComp, AttachRules, AttachSocket);

	MeshComp->SetRelativeRotation(MeshInitialRotation);
	MeshComp->SetRelativeScale3D(MeshInitialScale);
}

void AWeaponBase::DetachFromOwner()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetEnableGravity(true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

bool AWeaponBase::FireBullet()
{
	const FRotator FireRotation = GetFireDirection().Rotation();

	if (ABulletBase* Bullet = BulletPoolManager->GetBullet(BulletClass))
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FireDirection.ToString());
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FireRotation.ToString());

		Bullet->ActivateBullet(MuzzlePoint->GetComponentLocation(), FireRotation, OwnerPawn, this);

		return true;
	}

	return false;
}

void AWeaponBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
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

void AWeaponBase::ApplyBulletDamage(AActor* TargetActor, const FHitResult& HitResult)
{
	if (!TargetActor) return;
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerPawn);

	if (SourceASC && TargetASC && BulletDamageEffect)
	{
		FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		
		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(BulletDamageEffect, 1.0f, ContextHandle);

		if (SpecHandle.IsValid())
		{
			FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
			SpecHandle.Data->SetSetByCallerMagnitude(DamageTag, WeaponDamage);
			
			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

			float CurrentHealth = TargetASC->GetNumericAttribute(UMatrixAttributeSet::GetHealthAttribute());
			if (CurrentHealth <= 0.0f)
			{
				SendEventData(HitResult);
			}
		}
	}
}

void AWeaponBase::SendEventData(const FHitResult& HitResult)
{
	FVector NormalImpulse = HitResult.ImpactNormal * 3000.0f;

	FGameplayEventData EventData;
	EventData.Instigator = OwnerPawn;
	EventData.Target = HitResult.GetActor();
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("GameplayEvent.Death"));

	FTargetData_HitWithImpulse* TargetData = new FTargetData_HitWithImpulse();
	TargetData->ImpactPoint = HitResult.ImpactPoint;
	TargetData->Impulse = NormalImpulse;
	EventData.TargetData.Add(TargetData);
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitResult.GetActor(), EventData.EventTag, EventData);
}

void AWeaponBase::SetTargetLocation(const FVector& NewTargetLocation)
{
	TargetLocation = NewTargetLocation;
}

FVector AWeaponBase::GetFireDirection() const
{
	if (!TargetLocation.IsZero())
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
