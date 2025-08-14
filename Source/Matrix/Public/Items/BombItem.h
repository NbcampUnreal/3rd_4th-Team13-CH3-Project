#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BombItem.generated.h"

class UWeaponEffectComponent;
class UWeaponDamageComponent;
class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class MATRIX_API ABombItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ABombItem();

	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; };
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* CollisionComp;
	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* StaticMeshComp;
	UPROPERTY(VisibleAnywhere, Category="Components")
	UProjectileMovementComponent* ProjectileMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UWeaponDamageComponent* DamageComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UWeaponEffectComponent* EffectComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float InitialSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Explosion")
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Explosion")
	float ExplosionTime;
	
	void Explode();

	FTimerHandle TimerHandle_Explosion;

private:
	TArray<APawn*> GetPawnInExplosion(UWorld* World, const FVector& Origin, float ExplosionRange);
};
