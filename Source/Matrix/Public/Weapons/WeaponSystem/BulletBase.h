#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "BulletBase.generated.h"

class AWeaponBase;
class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraComponent;
class UNiagaraSystem;

USTRUCT()
struct FTargetData_HitWithImpulse : public FGameplayAbilityTargetData_SingleTargetHit
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FVector ImpactPoint = FVector::ZeroVector;	// 맞은 위치
	UPROPERTY()
	FVector Impulse = FVector::ZeroVector;	// 힘의 방향
};

UCLASS()
class MATRIX_API ABulletBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ABulletBase();

	void ActivateBullet(FVector Location, FRotator Rotation, APawn* NewOwner, AWeaponBase* NewWeapon);
	void DeactivateBullet();

	FORCEINLINE bool IsActive() const { return bIsActive; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USphereComponent* CollisionComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UProjectileMovementComponent* ProjectileMovementComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UNiagaraComponent* TrailEffectComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	float BulletSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	float BulletLifeSpan;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	void OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	UPROPERTY()
	APawn* OwnerPawn;
	UPROPERTY()
	AWeaponBase* OwnerWeapon;

	FTimerHandle DeactivateTimerHandle;
	bool bIsActive;
};
