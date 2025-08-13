#include "Items/BombItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

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

	ExplosionRadius = 500.f;
	ExplosionDamage = 100.f;
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
	//폭발 범위 내 액터에게 데미지 적용 (플레이어 제외)
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(PlayerPawn);

	UGameplayStatics::ApplyRadialDamage(
		this,
		ExplosionDamage,
		GetActorLocation(),
		ExplosionRadius,
		UDamageType::StaticClass(),
		IgnoredActors,
		this,
		GetInstigatorController(),
		true
	);
	
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
	
	if (ExplosionParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionParticle, GetActorLocation(), FRotator::ZeroRotator,
		FVector(8.0f), true);
	}
	
	Destroy();
}
