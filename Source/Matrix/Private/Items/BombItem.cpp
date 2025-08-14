#include "Items/BombItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Weapons/WeaponSystem/Component/WeaponDamageComponent.h"
#include "Weapons/WeaponSystem/Component/WeaponEffectComponent.h"

ABombItem::ABombItem()
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	StaticMeshComp->SetSimulatePhysics(false);
	RootComponent = StaticMeshComp;
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetSphereRadius(15.0f);
	CollisionComp->SetupAttachment(RootComponent);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.6f;
	ProjectileMovement->Friction = 0.2f;
	ProjectileMovement->ProjectileGravityScale = 1.1f;
	ProjectileMovement->MaxSpeed = 1100.f;
	ProjectileMovement->InitialSpeed = 1100.f;

	DamageComp = CreateDefaultSubobject<UWeaponDamageComponent>(TEXT("DamageComp"));
	EffectComp = CreateDefaultSubobject<UWeaponEffectComponent>(TEXT("EffectComp"));

	ExplosionRadius = 500.f;
	ExplosionTime = 3.0f;
	InitialSpeed = 2500.f;
}

void ABombItem::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("[BombItem] PC Get Failed."));
		return;
	}

	//플레이어가 바라보는 방향 값 획득
	FRotator CameraRot;
	FVector CameraLoc;
	PC->GetPlayerViewPoint(CameraLoc, CameraRot);

	//회전을 방향벡터로 전환
	FVector LaunchDir = CameraRot.Vector();

	
	ProjectileMovement->SetUpdatedComponent(RootComponent);
	
	FVector LaunchVelocity = LaunchDir * InitialSpeed;
	LaunchVelocity.Z += 150.f;
	
	ProjectileMovement->Velocity = LaunchVelocity;
	
	GetWorldTimerManager().SetTimer(
		TimerHandle_Explosion,
		this,
		&ABombItem::Explode,
		ExplosionTime,
		false
		);
}

void ABombItem::Explode()
{
	if (!DamageComp)
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon Damage Component not found"));
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TArray<APawn*> DamagedPawns = GetPawnInExplosion(GetWorld(), GetActorLocation(), ExplosionRadius);

	for (APawn* DamagedPawn : DamagedPawns)
	{
		DamageComp->ApplyDamage(DamagedPawn, PlayerPawn);
	}
	
	if (EffectComp)
	{
		EffectComp->PlayEffect(GetActorLocation(), GetActorRotation(), FVector(8.0f));
	}
	
	Destroy();
}

TArray<APawn*> ABombItem::GetPawnInExplosion(UWorld* World, const FVector& Origin, float ExplosionRange)
{
	TArray<AActor*> OverlappedActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	//폭발 범위 내 액터에게 데미지 적용 (플레이어 제외)
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(PlayerPawn);

	UKismetSystemLibrary::SphereOverlapActors(
		World,
		Origin,
		ExplosionRange,
		ObjectTypes,
		APawn::StaticClass(),
		IgnoredActors,
		OverlappedActors
	);

	TArray<APawn*> Pawns;
	for (AActor* OverlappedActor : OverlappedActors)
	{
		APawn* OverlappedPawn = Cast<APawn>(OverlappedActor);
		if (OverlappedPawn)
		{
			Pawns.Add(OverlappedPawn);
		}
	}

	return Pawns;
}
