#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class MATRIX_API ABulletBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ABulletBase();

	FORCEINLINE bool IsActive() const { return bIsActive; }

	void ActivateBullet(FVector Location, FRotator Rotation);
	void DeactivateBullet();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USphereComponent* CollisionComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UProjectileMovementComponent* ProjectileMovementComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX")
	UNiagaraComponent* TrailEffectComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	float BulletSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	float BulletLifeSpan;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	FTimerHandle DeactivateTimerHandle;
	bool bIsActive;
};
